#pragma once
//Note::
/*
	Replace with slim down GPU only version, use ContentLoaders too load
	different supported formats and return a TextureData. Use specific managers like
	TextureManager too support this and have a register loader function.

	Will leave alone for now!
*/

#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Resource.h"
#include <vector>

// Meta data for mips
struct TextureLevel
{
	Byte* ptr = nullptr;
	Uint32 width = 0;
	Uint32 height = 0;
	Uint32 depth = 0;
	Uint32 byteCount = 0;
};

class GraphicsDevice;
class Texture : public Resource
{
protected:
	GraphicsDevice* m_GraphicsDevice = nullptr;
	//--Descriptors--
	TextureDesc m_TextureDesc;
	SamplerDesc m_SamplerDesc;

	//--Gpu Memory--
	TextureHandle m_TextureHandle = TextureHandle();
	SamplerHandle m_SamplerHandle = SamplerHandle();

	//--CPU Data--
	Byte* m_Data = nullptr;
	std::vector<TextureLevel> m_LookUpTable;

public:
	Texture();
	Texture(const Texture& texture) = delete;
	Texture(Texture&& texture) noexcept;
	~Texture();

	void CreateTexture(TextureDesc& desc, const SamplerDesc& sampler);
	void Create1D(Uint32 width, Uint32 arraySize, BufferUsage usage, SurfaceFormat SurfaceFormat, TextureType type = TextureType::Texture1D);
	void Create2D(Uint32 width, Uint32 height, Uint32 arraySize = 1, bool mips = false, BufferUsage usage = BufferUsage::Immutable, SurfaceFormat SurfaceFormat = SurfaceFormat::R8G8B8A8_Unorm, TextureType type = TextureType::Texture2D);
	void Create3D(Uint32 width, Uint32 height, Uint32 depth, BufferUsage usage, SurfaceFormat SurfaceFormat, TextureType type = TextureType::Texture2D);

public:
	void operator=(const Texture& texture) = delete;
	void operator=(Texture&& texture) noexcept;

public:
	Uint32			GetWidth()const;
	Uint32			GetHeight()const;
	Uint32			GetDepth()const;
	Uint32			GetMipCount()const;
	Uint32			GetArraysize()const;
	Uint32			GetRowPitch()const;
	SurfaceFormat	GetFormat()const;
	TextureType		GetTextureType()const;
	FilterMode		GetFilter()const;
	void			SetFilter(FilterMode mode);
	WrapMode		GetWrapMode()const;
	void			SetWrapMode(WrapMode mode);
	bool			IsDisposed()const;
	bool			IsCube()const;
	bool			IsSRGB()const;
	bool			HasMips()const;
	Uint32			GetByteCount()const;
	Byte*			GetData()const;
	void			GetGPUData(Byte* data, Uint32 byteCount);
	void			SetData(Byte* data);
	Byte*			GetSurfaceData(Uint32 mipLevel = 0, Uint32 arrayLevel = 0)const;
	void			SetSurfaceData(Byte* data, Uint32 mipLevel = 0, Uint32 arrayLevel = 0);
	Byte*			GetSurfaceArrayData(Uint32 arrayLevel = 0)const;
	void			SetSurfaceArrayData(Byte* data, Uint32 arrayLevel = 0);
	Color			GetPixel(Uint32 x, Uint32 y, Uint32 z = 1, Uint32 array = 1, Uint32 mip = 1);
	void			SetPixel(Color& color, Uint32 x, Uint32 y, Uint32 z = 1, Uint32 array = 1, Uint32 mip = 1);
	void			SetUnorderedAccess();
	void			ClearCPUData();

	// Submits data to low-level GPU interface if required.
	void Apply();
	void Bind(int slot, ShaderType stage = ShaderType::PS);
	void LoadFromFile(const std::string& fileName);
	bool SaveToFile(std::string filePath);
	void Reload();
	void Release();
	void Linearize();  // Used to remove SRGB
	void ToSRGB();	   // Used to Apply SRGB
	void MarkAsLinear();

	TextureHandle			 GetTextureHandle()const;
	SamplerHandle			 GetSampleHandle()const;
	TextureDesc				 GetTextureInfo()const;
	SamplerDesc				 GetSamplerDesc()const;
	const GraphicsTexture*	 GetTextureResource()const;
	static inline bool		 GetRoundedSize(Uint32* width, Uint32* height, RoundMode roundMode, TextureType type);

protected:
	void LoadFromSource(std::string fileName);
	void LoadFromRaw(std::string fileName);
	void LoadFromDDS(std::string fileName);
	void LoadFromBinary(std::string fileName);
	void GenerateLookUpTable();

};