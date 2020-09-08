#pragma once
#include "Content/Texture.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/CommonStates.h"
#include "System/File.h"
#include "System/Logger.h"
#include "System/Window.h"
#include "Math/Mathf.h"
#include "Application/Application.h"
#include "System/Assert.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "dds/DDSImage.h"

Texture::Texture() : Resource()
{
	m_TextureDesc.Type = TextureType::Texture1D;
	m_GraphicsDevice = Application::graphicsDevice;
}

Texture::Texture(Texture&& texture)noexcept : Resource(std::move(texture))
{
	// steal
	m_GraphicsDevice = texture.m_GraphicsDevice;
	m_TextureDesc	= texture.m_TextureDesc;
	m_SamplerDesc	= texture.m_SamplerDesc;
	m_TextureHandle = texture.m_TextureHandle;
	m_SamplerHandle = texture.m_SamplerHandle;
	m_Data			= texture.m_Data;
	m_LookUpTable	= std::move(texture.m_LookUpTable);

	// Clear other.
	texture.m_GraphicsDevice	= nullptr;
	texture.m_Data				= nullptr;
	texture.m_TextureHandle		= TextureHandle();
	texture.m_SamplerHandle		= SamplerHandle();
	texture.m_TextureDesc		= {};
	texture.m_SamplerDesc		= {};
}

Texture::~Texture()
{
	if (m_Data)
	{
		delete[] m_Data;
		m_Data = nullptr;
		m_LookUpTable.clear();
	}

	if (m_GraphicsDevice)
	{
		if (m_TextureHandle.IsValid())
		{
			m_GraphicsDevice->DestroyTexture(m_TextureHandle);
			m_TextureHandle = TextureHandle();
		}

		if (m_SamplerHandle.IsValid())
		{
			// There is no Destory sampler!
			m_SamplerHandle = SamplerHandle();
		}
	}
}

void Texture::CreateTexture(TextureDesc& desc, const SamplerDesc& sampler)
{
	m_TextureDesc.Width = desc.Width;
	m_TextureDesc.Height = desc.Height;
	m_TextureDesc.Depth = desc.Depth;
	m_TextureDesc.ArraySize = desc.ArraySize;
	m_TextureDesc.MipLevels = desc.MipLevels;
	m_TextureDesc.Format = desc.Format;
	m_TextureDesc.Usage = desc.Usage;
	m_TextureDesc.CPUAccessFlags = desc.CPUAccessFlags;
	m_TextureDesc.Type = desc.Type;
	m_TextureDesc.BindFlags = desc.BindFlags;
	m_TextureDesc.MiscFlags = desc.MiscFlags;

	if (m_TextureDesc.Type == TextureType::Texture3D)
	{
		m_TextureDesc.Pitch = CalculatePitchSize(m_TextureDesc.Format, m_TextureDesc.Width);
	}
	else
	{
		m_TextureDesc.Pitch = CalculatePitchSize(m_TextureDesc.Format, m_TextureDesc.Width, m_TextureDesc.Height);
	}

	//--Calculate cpu buffer size--
	m_TextureDesc.ByteCount = CalculateTotalBytes(m_TextureDesc.Width, m_TextureDesc.Height, m_TextureDesc.Depth, m_TextureDesc.MipLevels, m_TextureDesc.ArraySize, m_TextureDesc.Format);
	m_Data = new Byte[m_TextureDesc.ByteCount];

	GenerateLookUpTable();
}

void Texture::Create1D(Uint32 width, Uint32 arraySize, BufferUsage usage, SurfaceFormat format, TextureType type)
{
	m_TextureDesc.Width = width;
	m_TextureDesc.Height = 1;
	m_TextureDesc.Depth = 1;
	m_TextureDesc.ArraySize = arraySize > 0 ? arraySize : 1;
	m_TextureDesc.MipLevels = 1;
	m_TextureDesc.Format = format;
	m_TextureDesc.Type = type;
	m_TextureDesc.BindFlags = (usage != BufferUsage::Staging)? (Uint32)BindFlag::ShaderResource : (Uint32)BindFlag::Unknown;
	m_TextureDesc.Pitch = CalculatePitchSize(format, width);
	m_TextureDesc.Usage = usage;

	switch (usage)
	{
	case BufferUsage::Default:
	case BufferUsage::Immutable:
		m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
		break;
	case BufferUsage::Dynamic:
		m_TextureDesc.CPUAccessFlags = CpuAccess::Write;
		break;
	case BufferUsage::Staging:
		m_TextureDesc.CPUAccessFlags = CpuAccess::ReadWrite;
		break;
	default:
		m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
		break;
	}

	m_TextureDesc.ByteCount = CalculateTotalBytes(width, 1, 1, m_TextureDesc.MipLevels, m_TextureDesc.ArraySize, format);
	m_Data = new Byte[m_TextureDesc.ByteCount];
	GenerateLookUpTable();
}

void Texture::Create2D(Uint32 width, Uint32 height, Uint32 arraySize, bool mips, BufferUsage usage, SurfaceFormat format, TextureType type)
{
	if (mips)
	{
		GetRoundedSize(&width, &height, RoundMode::Nearest, type);
	}

	m_TextureDesc.Width = width;
	m_TextureDesc.Height = height;
	m_TextureDesc.Depth = 1;
	m_TextureDesc.ArraySize = arraySize > 0 ? arraySize : 1;
	m_TextureDesc.MipLevels = mips ? CalculateMipCount(width, height) : 1;
	m_TextureDesc.Format = format;
	m_TextureDesc.Usage = usage;
	m_TextureDesc.Type = type;
	m_TextureDesc.BindFlags = (Uint32)BindFlag::ShaderResource;
	m_TextureDesc.MiscFlags = type == TextureType::TextureCube ? (Uint32)ResourceMiscFlags::TexturedCube : 0;
	m_TextureDesc.Pitch = CalculatePitchSize(format, width);


	switch (usage)
	{
	case BufferUsage::Default:
	case BufferUsage::Immutable:
		m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
		break;
	case BufferUsage::Dynamic:
		m_TextureDesc.CPUAccessFlags = CpuAccess::Write;
		break;
	case BufferUsage::Staging:
		m_TextureDesc.CPUAccessFlags = CpuAccess::ReadWrite;
		break;
	default:
		m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
		break;
	}

	//--Calculate cpu buffer size--
	m_TextureDesc.ByteCount = CalculateTotalBytes(width, height, 1, m_TextureDesc.MipLevels, m_TextureDesc.ArraySize, format);
	m_Data = new Byte[m_TextureDesc.ByteCount];
	GenerateLookUpTable();
}

void Texture::Create3D( Uint32 width, Uint32 height, Uint32 depth, BufferUsage usage, SurfaceFormat format, TextureType type)
{
	m_TextureDesc.Width = width;
	m_TextureDesc.Height = height;
	m_TextureDesc.Depth = depth;
	m_TextureDesc.ArraySize = 1;
	m_TextureDesc.MipLevels = 1;
	m_TextureDesc.Format = format;
	m_TextureDesc.Usage = usage;
	m_TextureDesc.Type = TextureType::Texture3D;
	m_TextureDesc.BindFlags = (Uint32)BindFlag::ShaderResource;
	m_TextureDesc.Pitch = CalculatePitchSize(format, width);

	switch (usage)
	{
	case BufferUsage::Default:
	case BufferUsage::Immutable:
		m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
		break;
	case BufferUsage::Dynamic:
		m_TextureDesc.CPUAccessFlags = CpuAccess::Write;
		break;
	case BufferUsage::Staging:
		m_TextureDesc.CPUAccessFlags = CpuAccess::ReadWrite;
		break;
	default:
		m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
		break;
	}

	//--Calculate cpu buffer size--
	m_TextureDesc.ByteCount = CalculateTotalBytes(width, height, depth, m_TextureDesc.MipLevels, 1, format);
	m_Data = new Byte[m_TextureDesc.ByteCount]();

	GenerateLookUpTable();
}

void Texture::GetGPUData(Byte* data, Uint32 byteCount)
{
	m_GraphicsDevice->GetTextureData(m_TextureHandle, data, byteCount);
}

void Texture::SetData(Byte* data)
{
	if (m_TextureDesc.ByteCount == 0) { LogError("No Bytes to set."); return; }

	if (!m_Data)
	{
		m_Data = new Byte[m_TextureDesc.ByteCount];
	}

	memcpy(m_Data, data, m_TextureDesc.ByteCount);
}

void Texture::operator=(Texture&& texture) noexcept
{
	// Copy
	m_GraphicsDevice = texture.m_GraphicsDevice;
	m_TextureDesc	 = texture.m_TextureDesc;
	m_SamplerDesc	 = texture.m_SamplerDesc;
	m_TextureHandle  = texture.m_TextureHandle;
	m_SamplerHandle  = texture.m_SamplerHandle;
	m_Data			 = texture.m_Data;
	m_LookUpTable	 = std::move(texture.m_LookUpTable);

	// Clear other.
	texture.m_GraphicsDevice = nullptr;
	texture.m_Data = nullptr;
	texture.m_TextureHandle = TextureHandle();
	texture.m_SamplerHandle = SamplerHandle();
	texture.m_TextureDesc = {};
	texture.m_SamplerDesc = {};
}

Uint32 Texture::GetWidth() const
{
	return m_TextureDesc.Width;
}

Uint32 Texture::GetHeight() const
{
	return m_TextureDesc.Height;
}

Uint32 Texture::GetDepth() const
{
	return m_TextureDesc.Depth;
}

Uint32 Texture::GetMipCount() const
{
	return m_TextureDesc.MipLevels;
}

Uint32 Texture::GetArraysize() const
{
	return m_TextureDesc.ArraySize;
}

Uint32 Texture::GetRowPitch() const
{
	return m_TextureDesc.Pitch;
}

SurfaceFormat Texture::GetFormat() const
{
	return m_TextureDesc.Format;
}

TextureType Texture::GetTextureType() const
{
	return m_TextureDesc.Type;
}

FilterMode Texture::GetFilter() const
{
	return m_SamplerDesc.Filter;
}

WrapMode Texture::GetWrapMode() const
{
	return m_SamplerDesc.AddressU;
}

void Texture::SetFilter(FilterMode mode)
{
	m_SamplerDesc.Filter = mode;
}

void Texture::SetWrapMode(WrapMode mode)
{
	m_SamplerDesc.AddressU = mode;
	m_SamplerDesc.AddressV = mode;
	m_SamplerDesc.AddressW = mode;
}

bool Texture::IsDisposed() const
{
	return m_GraphicsDevice == nullptr && m_TextureHandle.IsValid() == false;
}

bool Texture::IsCube() const
{
	return m_TextureDesc.MiscFlags == (Uint32)ResourceMiscFlags::TexturedCube;
}

bool Texture::IsSRGB() const
{
	return m_TextureDesc.SRGB;
}

bool Texture::HasMips() const
{
	return m_TextureDesc.MipLevels > 1;
}

Uint32 Texture::GetByteCount()const
{
	return m_TextureDesc.ByteCount;
}

Byte* Texture::GetData()const
{
	return m_Data;
}

void Texture::SetSurfaceData(Byte* data, Uint32 mipLevel, Uint32 arrayLevel)
{
	if (m_TextureDesc.ByteCount == 0) { LogWarning("No Bytes to set."); return; }
	if (mipLevel > m_TextureDesc.MipLevels) { LogWarning("MipLevel is too high."); return; }
	if (arrayLevel > m_TextureDesc.ArraySize) { LogWarning("ArrayLevel is too high."); return; }

	TextureLevel* surface = &m_LookUpTable[(arrayLevel * m_TextureDesc.MipLevels) + mipLevel];
	memcpy(surface->ptr, data, surface->byteCount);

}

Byte* Texture::GetSurfaceData(Uint32 mipLevel, Uint32 arrayLevel)const
{
	if (mipLevel > m_TextureDesc.MipLevels) { LogError("MipLevel is too high."); return nullptr; }
	if (arrayLevel > m_TextureDesc.ArraySize) { LogError("ArrayLevel is too high."); return nullptr; }
	const TextureLevel* surface = &m_LookUpTable[(arrayLevel * m_TextureDesc.MipLevels) + mipLevel];
	return surface->ptr;
}

Byte* Texture::GetSurfaceArrayData(Uint32 arrayLevel) const
{
	if (m_TextureDesc.ByteCount == 0) { LogWarning("No Bytes to set."); return nullptr; }
	if (arrayLevel > m_TextureDesc.ArraySize) { LogWarning("ArrayLevel is too high."); return nullptr; }
	const TextureLevel* surface = &m_LookUpTable[(arrayLevel * m_TextureDesc.MipLevels)];
	return surface->ptr;
}

void Texture::SetSurfaceArrayData(Byte* data, Uint32 arrayLevel)
{
	if (m_TextureDesc.ByteCount == 0) { LogWarning("No Bytes to set."); return; }
	if (arrayLevel > m_TextureDesc.ArraySize) { LogWarning("ArrayLevel is too high."); return; }

	// How many bytes to copy over
	Uint32 byteCount = CalculateTotalBytes(m_TextureDesc.Width, m_TextureDesc.Height, m_TextureDesc.Depth, m_TextureDesc.MipLevels, 1, m_TextureDesc.Format);
	// The start location to overwrite from
	TextureLevel* surface = &m_LookUpTable[(arrayLevel * m_TextureDesc.MipLevels)];
	memcpy(surface->ptr, data, byteCount);
}

Color Texture::GetPixel(Uint32 x, Uint32 y, Uint32 z, Uint32 array, Uint32 mip)
{
	Uint32 index = ((y * m_TextureDesc.Width) + x) * z * BytesPerBlock(m_TextureDesc.Format);
	float R = 1.0f / 255;

	Color color;
	color.r = m_Data[index]		* R;
	color.g = m_Data[index + 1] * R;
	color.b = m_Data[index + 2] * R;
	color.a = m_Data[index + 3] * R;

	return color;
}

void Texture::SetPixel(Color& color, Uint32 x, Uint32 y, Uint32 z, Uint32 array, Uint32 mip)
{
	Uint32 index = ((y * m_TextureDesc.Width) + x) * z * BytesPerBlock(m_TextureDesc.Format);

	m_Data[index] = (Byte)(color.r * 255);
	m_Data[index + sizeof(Byte)] = (Byte)(color.g * 255);
	m_Data[index + (sizeof(Byte) * 2)] = (Byte)(color.b * 255);
	m_Data[index + (sizeof(Byte) * 3)] = (Byte)(color.a * 255);
}

void Texture::SetUnorderedAccess()
{
	m_TextureDesc.BindFlags = (Uint32)BindFlag::UnorderedAccess;
}

void Texture::ClearCPUData()
{
	if (m_Data)
	{
		delete[] m_Data;
		m_LookUpTable.clear();
	}
}

void Texture::Apply(bool keepResident)
{
	if (m_GraphicsDevice == nullptr) { return; }
	if (m_TextureDesc.ByteCount == 0) { LogWarning("No Bytes to set."); return; }
	if (m_TextureDesc.Usage == BufferUsage::Immutable && m_TextureHandle.IsValid()) { LogWarning("Cannot update Immutable Texture."); return; }

	if (m_TextureHandle.IsValid() == false)
	{
		m_TextureHandle = m_GraphicsDevice->CreateTexture(&m_TextureDesc, m_Data);
	}
	else
	{
		m_GraphicsDevice->UpdateTexture(m_TextureHandle, m_Data, m_TextureDesc.ByteCount);
	}

	if (m_SamplerHandle.IsValid() == false)
	{
		m_SamplerHandle = m_GraphicsDevice->CreateSamplerState(&m_SamplerDesc);
	}

	if (keepResident == false)
	{
		delete[] m_Data;
		m_Data = nullptr;
	}
}

void Texture::Bind(int slot, ShaderType stage)
{
	assert(m_GraphicsDevice != nullptr);

	if (m_TextureDesc.BindFlags & (Uint32)BindFlag::ShaderResource)
	{
		m_GraphicsDevice->BindTexture(m_TextureHandle, slot);
		m_GraphicsDevice->BindSampler(m_SamplerHandle, slot);
	}
	else if (m_TextureDesc.BindFlags& (Uint32)BindFlag::UnorderedAccess)
	{
		m_GraphicsDevice->BindTextureUAV(m_TextureHandle, stage, slot);
	}
}

void Texture::LoadFromFile(const std::string& fileName)
{
	// Breaks SRP Completely
	// Make sure this object isnt loaded
	Release();

	//--Get Extension--
	std::string ext = fileName.c_str();
	ext = ext.substr(ext.find_last_of(".") + 1);
	for (int i = 0; i < ext.size(); ++i)
	{
		ext[i] = tolower(ext[i]);
	}

	std::string filePath = fileName;
	if (ext != "texture")
	{
		std::string filePath = fileName;
		filePath = filePath.substr(0, filePath.find_last_of("."));
		filePath += ".texture";

		if (File::Exists(filePath))
		{
			LoadFromBinary(filePath);
		}
		else
		{
			if (ext == "jpg" || ext == "png" || ext == "bmp" || ext == "tga")
			{
				LoadFromSource(fileName);
				// save for optimisation
				SaveToFile(filePath);
			}
			else if (ext == "raw")
			{
				LoadFromRaw(fileName);
			}
			else if (ext == "dds")
			{
				LoadFromDDS(fileName);
			}

			filePath = fileName;
		}
	}
	else if (ext == "texture")
	{
		LoadFromBinary(filePath);
	}

	m_FilePath = filePath;
	m_Name = filePath; // Change at some point
	m_Size = m_TextureDesc.ByteCount;
	m_LoadState = LoadState::Loaded;
	GenerateLookUpTable();
	Apply(); // Submit to GPU
}

void Texture::Reload()
{
	if (!m_FilePath.empty())
	{
		// cache file string because it might be blanked!
		std::string filePath = m_FilePath;
		bool wasOnGpu = m_TextureHandle.IsValid(); // Weather or not the texture was bound to GPU

		Release();
		LoadFromFile(filePath.c_str());
		if (wasOnGpu) 
		{ 
			Apply();
			ClearCPUData();
		} // Re-upload to GPU after load!
	}

	assert(0 && "Failed to reload texture.");
}

void Texture::Release()
{
	if (m_Data != nullptr)
	{
		delete[] m_Data;
		m_Data = nullptr;
		m_LookUpTable.clear();
	}

	if (m_TextureHandle.IsValid())
	{
		m_GraphicsDevice->DestroyTexture(m_TextureHandle);
		m_TextureHandle = TextureHandle();
	}

	if (m_SamplerHandle.IsValid())
	{
		// Just set to default, samplers cached so no destorying!!!
		m_SamplerHandle = SamplerHandle();
	}
}

void Texture::Linearize()
{
	if (m_Data != nullptr && m_TextureDesc.Format == SurfaceFormat::R8G8B8A8_Unorm_SRGB)
	{
		for (size_t i = 0; i < m_TextureDesc.ByteCount; i += 4)
		{
			m_Data[i] = (Byte)Mathf::ToLinear(m_Data[i]);
			m_Data[i + 1] = (Byte)Mathf::ToLinear(m_Data[i + 1]);
			m_Data[i + 2] = (Byte)Mathf::ToLinear(m_Data[i + 2]);
			// Alpha isnt SRGB encoded?
		}

		// Indicate its a linear texture buffer!
		m_TextureDesc.Format = SurfaceFormat::R8G8B8A8_Unorm;
	}
}

void Texture::ToSRGB()
{
	if (m_Data != nullptr && m_TextureDesc.Format == SurfaceFormat::R8G8B8A8_Unorm)
	{
		for (size_t i = 0; i < m_TextureDesc.ByteCount; i += 4)
		{
			m_Data[i] = (Byte)Mathf::ToSrgbFast(m_Data[i]);
			m_Data[i + 1] = (Byte)Mathf::ToSrgbFast(m_Data[i + 1]);
			m_Data[i + 2] = (Byte)Mathf::ToSrgbFast(m_Data[i + 2]);
			// Alpha isnt SRGB encoded?
		}

		// Indicate its a linear texture buffer!
		m_TextureDesc.Format = SurfaceFormat::R8G8B8A8_Unorm;
	}
}

void Texture::MarkAsLinear()
{
	m_TextureDesc.SRGB = 0;
	// Get format as a linear format!
	m_TextureDesc.Format = SRGBToLinearFormat(m_TextureDesc.Format);
}

bool Texture::SaveToFile(std::string filePath)
{
	if (m_Data)
	{
		std::string ext = filePath;
		ext = ext.substr(ext.find_last_of(".") + 1);

		if (ext == "texture" | ext == "Texture")
		{
			//--Get Extension--
			BinaryFile file(filePath.c_str(), FileMode::Write);
			assert(file.IsOpen() && "Failed to open texture file");

			//--Write Header--
			file.WriteDword(48 + m_TextureDesc.ByteCount); // FileSize
			file.WriteDword(m_TextureDesc.Width);
			file.WriteDword(m_TextureDesc.Height);
			file.WriteDword(m_TextureDesc.Depth);
			file.WriteDword(m_TextureDesc.ArraySize);
			file.WriteDword(m_TextureDesc.MipLevels);
			file.WriteDword(m_TextureDesc.Pitch);
			file.WriteDword(m_TextureDesc.ByteCount);
			file.WriteDword((Dword)m_TextureDesc.Format);
			file.WriteDword((Dword)m_TextureDesc.Type);
			file.WriteDword(m_TextureDesc.MiscFlags);
			file.WriteWord(m_TextureDesc.SRGB);
			file.WriteDword((Dword)m_SamplerDesc.Filter);
			file.WriteDword((Dword)m_SamplerDesc.AddressU);

			//--Write Data--
			if (file.WriteBuffer(m_Data, m_TextureDesc.ByteCount))
			{
				file.Close();
				return true;
			}
			file.Close();
		}
		else if (ext == "png" | ext == "PNG")
		{
			stbi_write_png(filePath.c_str(), m_TextureDesc.Width, m_TextureDesc.Height, 4, m_Data, m_TextureDesc.Pitch);
		}
		else if (ext == "jpg" | ext == "JPG")
		{
			stbi_write_jpg(filePath.c_str(), m_TextureDesc.Width, m_TextureDesc.Height, 4, m_Data, 80);
		}
		else if (ext == "bmp" | ext == "BMP")
		{
			stbi_write_bmp(filePath.c_str(), m_TextureDesc.Width, m_TextureDesc.Height, 4, m_Data);
		}
	}
	return false;
}

TextureHandle Texture::GetTextureHandle() const
{
	return m_TextureHandle;
}

SamplerHandle Texture::GetSampleHandle() const
{
	return m_SamplerHandle;
}

TextureDesc Texture::GetTextureInfo() const
{
	return m_TextureDesc;
}

SamplerDesc Texture::GetSamplerDesc() const
{
	return m_SamplerDesc;
}

const GraphicsTexture* Texture::GetTextureResource() const
{
	return m_GraphicsDevice->GetTexture(m_TextureHandle);
}

inline bool Texture::GetRoundedSize(Uint32* width, Uint32* height, RoundMode roundMode, TextureType type)
{
	if (width == nullptr || height == nullptr)
	{
		LogError("Require Width and Height for resize");
		return false;
	}

	if (type == TextureType::Texture3D)
	{
		LogError("Cannot round a Volume Texture.");
		return false;
	}

	int w = *width;
	int h = *height;

	if (type == TextureType::TextureCube)
	{
		//Cube maps HAVE to be square.
		w = h = (w + h) / 2;
	}

	//--Do the rounding math--
	if (roundMode == RoundMode::Next)
	{
		w = Mathf::NextPowerOfTwo((Uint32)w);
		h = Mathf::NextPowerOfTwo((Uint32)h);
	}
	else if (roundMode == RoundMode::Previous)
	{
		w = Mathf::PreviousPowerOfTwo((Uint32)w);
		h = Mathf::PreviousPowerOfTwo((Uint32)h);
	}
	else if (roundMode == RoundMode::Nearest)
	{
		w = Mathf::NearestPowerOfTwo((Uint32)w);
		h = Mathf::NearestPowerOfTwo((Uint32)h);
	}

	//--Set the out ptrs--
	*width = w;
	*height = h;

	return true;
}

void Texture::LoadFromSource(std::string fileName)
{
	int width = 0;
	int height = 0;
	int comp = 0;

	unsigned char* p = stbi_load(fileName.c_str(), &width, &height, &comp, STBI_rgb_alpha);

	if (p != nullptr)
	{
		if (width != 0 && height != 0)
		{
			// i mean loading a source is already slow anyway, so do a check for non SRGB and covnert.
			m_TextureDesc.Format = SurfaceFormat::R8G8B8A8_Unorm_SRGB;

			size_t found = fileName.find_last_of("_");
			if (found != std::string::npos)
			{
				std::string type = fileName.substr(found + 1, fileName.find_last_of(".") - (found + 1));

				if (type == "Normal" || type == "Roughness" || "type" == "Metalness")
				{
					m_TextureDesc.Format = SurfaceFormat::R8G8B8A8_Unorm;
				}
			}

			m_TextureDesc.Width			 = width;
			m_TextureDesc.Height		 = height;
			m_TextureDesc.Depth			 = 1;
			m_TextureDesc.ArraySize		 = 1;
			m_TextureDesc.MipLevels		 = 1;
			m_TextureDesc.Pitch			 = CalculatePitchSize(m_TextureDesc.Format, width);
			m_TextureDesc.ByteCount		 = CalculateTotalBytes(width, height, 1, 1, 1, m_TextureDesc.Format);
			m_TextureDesc.Type			 = TextureType::Texture2D;
			m_TextureDesc.BindFlags		 = (Uint32)BindFlag::ShaderResource;
			m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
			m_TextureDesc.Usage			 = BufferUsage::Immutable;
			m_SamplerDesc				 = CommonStates::LinearWrap; // Just use a default!

			m_Data = new Byte[m_TextureDesc.ByteCount];
			memcpy(m_Data, p, m_TextureDesc.ByteCount);
		}

		stbi_image_free(p);
	}
	else
	{
		assert(0 && "Failed to load texture.");
		m_LoadState = LoadState::Failed;
	}
}

void Texture::LoadFromRaw(std::string fileName)
{
	std::string metaPath = fileName;
	metaPath += ".meta";

	File metaFile;
	if (metaFile.Open(metaPath.c_str(), FileMode::Read))
	{
		std::string line = "";
		std::string key = "";
		std::string value = "";
		while (metaFile.ReadLine(line, true))
		{
			key = "";
			value = "";
			int seperator_Pos = (int)line.find_first_of('=', (size_t)0);
			if (seperator_Pos > 0)
			{
				key = line.substr(0, seperator_Pos);
				value = line.substr((size_t)seperator_Pos + 1);

				if (key == "Width")
				{
					m_TextureDesc.Width = std::stoi(value);
				}
				else if (key == "Height")
				{
					m_TextureDesc.Height = std::stoi(value);
				}
				else if (key == "Depth")
				{
					m_TextureDesc.Depth = std::stoi(value);
				}
				else if (key == "NumDims")
				{
					if (value == "1")
					{
						m_TextureDesc.Type = TextureType::Texture1D;
					}
					else if (value == "2")
					{
						m_TextureDesc.Type = TextureType::Texture2D;
					}
					else if (value == "3")
					{
						m_TextureDesc.Type = TextureType::Texture3D;
					}
				}
				else if (key == "Format")
				{
					if (value == "Uint8")
					{
						m_TextureDesc.Format = SurfaceFormat::R8_Uint;
						continue;
					}
					else if (value == "Sint8")
					{
						m_TextureDesc.Format = SurfaceFormat::R8_Sint;
					}
					else if (value == "Uint16")
					{
						m_TextureDesc.Format = SurfaceFormat::R16_Uint;
					}
					else if (value == "Sint16")
					{
						m_TextureDesc.Format = SurfaceFormat::R16_Sint;
					}
				}
			}
		}
		metaFile.Close();
	}
	else
	{
		m_LoadState = LoadState::Failed;
		Window::ShowMessageBox("No Meta File");
		Application::Quit();
		return;
	}

	m_TextureDesc.ArraySize = 1;
	m_TextureDesc.MipLevels = 1;
	m_TextureDesc.Pitch = CalculatePitchSize(m_TextureDesc.Format, m_TextureDesc.Width);
	m_TextureDesc.ByteCount = CalculateTotalBytes(m_TextureDesc.Width, m_TextureDesc.Height, m_TextureDesc.Depth, 1, 1, m_TextureDesc.Format);
	m_TextureDesc.BindFlags = (Uint32)BindFlag::ShaderResource;
	m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
	m_TextureDesc.Usage = BufferUsage::Immutable;

	BinaryFile file(fileName.c_str(), FileMode::Read);

	if (file.IsOpen())
	{
		m_Data = new Byte[m_TextureDesc.ByteCount];
		file.ReadBuffer(m_Data, m_TextureDesc.ByteCount);
		file.Close();
	}
	else
	{
		assert(0 && "Failed to load texture.");
		m_LoadState = LoadState::Failed;
	}
}

void Texture::LoadFromDDS(std::string fileName)
{
	DDSFile image;
	image.LoadFromFile(fileName);
	assert(image.m_Data.size() > 0 && "Failed to open texture file");

	m_TextureDesc.Width		= image.Width;
	m_TextureDesc.Height	= image.Height;
	m_TextureDesc.Depth		= image.Depth;
	m_TextureDesc.ArraySize = image.ArraySize;
	m_TextureDesc.MipLevels = image.MipCount;
	m_TextureDesc.MiscFlags = image.MiscFlags;
	m_TextureDesc.Pitch		= image.Pitch;
	m_TextureDesc.ByteCount = image.ByteCount;
	m_TextureDesc.Format	= image.Format;
	m_TextureDesc.Type		= image.Type;
	m_TextureDesc.SRGB		= IsFormatSRGB(image.Format);
	m_TextureDesc.BindFlags = (Uint32)BindFlag::ShaderResource;
	m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
	m_TextureDesc.Usage = BufferUsage::Immutable;
	m_SamplerDesc			= CommonStates::LinearWrap; // Just use a default!

	// Should make more effecient?
	m_Data = new Byte[m_TextureDesc.ByteCount];
	memcpy(m_Data, &image.m_Data[0], m_TextureDesc.ByteCount);
}

void Texture::LoadFromBinary(std::string fileName)
{
	BinaryFile file(fileName.c_str(), FileMode::Read);

	if (file.IsOpen())
	{
		//--Read Header--

		m_Size = file.ReadDword();
		m_TextureDesc.Width = file.ReadDword();
		m_TextureDesc.Height = file.ReadDword();
		m_TextureDesc.Depth = file.ReadDword();
		m_TextureDesc.ArraySize = file.ReadDword();
		m_TextureDesc.MipLevels = file.ReadDword();
		m_TextureDesc.Pitch = file.ReadDword();
		m_TextureDesc.ByteCount = file.ReadDword();
		m_TextureDesc.Format = (SurfaceFormat)file.ReadDword();
		m_TextureDesc.Type = (TextureType)file.ReadDword();
		m_TextureDesc.MiscFlags = file.ReadDword();
		m_TextureDesc.SRGB = file.ReadWord();
		m_SamplerDesc.Filter = (FilterMode)file.ReadDword();
		m_SamplerDesc.AddressU = (WrapMode)file.ReadDword();
		m_SamplerDesc.AddressV = m_SamplerDesc.AddressU;
		m_SamplerDesc.AddressW = m_SamplerDesc.AddressU;
		m_TextureDesc.BindFlags = (Uint32)BindFlag::ShaderResource;
		m_TextureDesc.CPUAccessFlags = CpuAccess::Immutable;
		m_TextureDesc.Usage = BufferUsage::Immutable;

		//--Read Data--
		m_Data = new Byte[m_TextureDesc.ByteCount];
		file.ReadBuffer(m_Data, m_TextureDesc.ByteCount);
		file.Close();
	}
	else
	{
		assert(0 && "Failed to load texture.");
	}
}

void Texture::GenerateLookUpTable()
{
	Uint32 w;
	Uint32 h;
	Uint32 d;

	Byte* ptr = m_Data;
	for (Uint32 i = 0; i < m_TextureDesc.ArraySize; i++)
	{
		w = m_TextureDesc.Width;
		h = m_TextureDesc.Height;
		d = m_TextureDesc.Depth;

		for (Uint32 j = 0; j < m_TextureDesc.MipLevels; j++)
		{
			TextureLevel level;
			level.width = w;
			level.height = h;
			level.depth = d;
			level.ptr = ptr;
			level.byteCount = CalculateSurfaceSize(m_TextureDesc.Format, w, h) * d;
			m_LookUpTable.push_back(level);

			ptr += level.byteCount;

			w = Mathf::Max(w >> 1, 1);
			h = Mathf::Max(h >> 1, 1);
			d = Mathf::Max(d >> 1, 1);
		}
	}
}

// This works, but is probably too expensive to justify it...
inline Uint32 TextureOffset(const TextureDesc& info, Uint32 mipLevel, Uint32 arraySlice, Uint32 byteCount)
{
	Uint32 pow = (Uint32)Mathf::Pow(4, (float)mipLevel);
	Uint32 A = byteCount / info.ArraySize; // Total bytes to a surface + mips
	Uint32 B = CalculateSurfaceSize(info.Format, info.Width, info.Height); // Total bytes to top level surface
	return  (arraySlice * A) + B + (B * ((Uint32)Mathf::Floor((float)pow / 3) / pow)); // offset bytes
}