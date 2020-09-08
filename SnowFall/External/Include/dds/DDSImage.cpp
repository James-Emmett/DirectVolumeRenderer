#include "dds/DDSImage.h"
#include "System/Assert.h"
#include <cstdio>

// Remember to fix fopen and remove _CRT_SECURE_NO_WRANINGS form preprocessor!

DDSFile::DDSFile()
{
}

DDSFile::~DDSFile()
{
}

void DDSFile::LoadFromFile(std::string path)
{
	Byte* data;
	Uint32 size;
	const DDS::DDS_HEADER* header;

	// Open dds for binary read.
	FILE* file = nullptr;
	fopen_s(&file, path.c_str(), "rb");
	assert(file && "Failed too open dds file");

	// Grab file size
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Alocate buffer, read data and close file
	data = new Byte[size];
	fread(data, sizeof(Byte), size, file);
	fclose(file);

	// Check magic number
	Uint32 dwMagicNumber = *reinterpret_cast<const Uint32*>(data);
	assert(dwMagicNumber == DDS::DDS_MAGIC && "Magix not correct may not be dds?");
	// Grab header, starts affter Magic
	header = reinterpret_cast<const DDS::DDS_HEADER*>(data + sizeof(Uint32));
	assert(header->size == sizeof(DDS::DDS_HEADER) || header->ddspf.size != sizeof(DDS::DDS_PIXELFORMAT) && "DDS_Header Corrupt");

	// Check for DX10 extension
	bool isDXT10Header = false;
	if ((header->ddspf.flags & DDS_FOURCC) &&
		(MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC))
	{
		// Must be long enough for both headers and magic value
		assert(size >= (sizeof(DDS::DDS_HEADER) + sizeof(Uint32) + sizeof(DDS::DDS_HEADER_DXT10)) && "DDS file thinks its got DXT10 ehader but doesnt.");
		isDXT10Header = true;
	}

	// Calculate offset to actual data + size of data
	Uint32 offset = sizeof(Uint32) + sizeof(DDS::DDS_HEADER) + (isDXT10Header ? sizeof(DDS::DDS_HEADER_DXT10) : 0u);
	Byte*  startOfData  = data + offset;
	Uint32 sizeOfData   = size - offset;

	// Copy image info data
	Width	  = header->width;
	Height	  = header->height;
	Depth	  = header->depth;
	MipCount  = header->mipMapCount;
	ByteCount = sizeOfData;

	bool isCube = false;

	if (isDXT10Header)
	{
		// Grab the DXT10 HEader
		const DDS::DDS_HEADER_DXT10* DXT10Header = reinterpret_cast<const DDS::DDS_HEADER_DXT10*>(data + sizeof(DDS::DDS_HEADER));
		ArraySize = DXT10Header->arraySize;
		assert(ArraySize != 0 && "DXT10Header is for array images yet no arrays where found.");

		Format = DXGIFormatToFormat(DXT10Header->dxgiFormat);

		if(Format == SurfaceFormat::Unkown)
		{
			assert(0 && "DDS Format Not Supported");
		}

		switch (DXT10Header->resourceDimension)
		{
			case DDS::DDS_DIMENSION_TEXTURE1D:
			{
				assert(((header->flags & DDS_HEIGHT) && Height != 1));
				Height = Depth = 1;
				Type = TextureType::Texture1D;
			}
			break;

			case DDS::DDS_DIMENSION_TEXTURE2D:
			{
				Type = TextureType::Texture2D;
				if (DXT10Header->miscFlag & DDS::DDS_RESOURCE_MISC_TEXTURECUBE)
				{
					MiscFlags = ResourceMiscFlags::TexturedCube;
					ArraySize *= 6;
					isCube = true;
					Type = TextureType::TextureCube;
				}
				Depth = 1;
			}
			break;

			case DDS::DDS_DIMENSION_TEXTURE3D:
			{
				assert(header->flags & DDS_HEADER_FLAGS_VOLUME && "Dimension is 3D but flags is not volume.");
				assert(ArraySize == 1 && "Volume textures do not have arrays.");
				Type = TextureType::Texture3D;
			}
			break;

			default:
			{
				assert(0 && "DDS Dimension not recognised");
			}
		}
	}
	else
	{
		Format = DDS::GetSurfaceFormat(header->ddspf);
		assert(Format != SurfaceFormat::Unkown && "DDS has unsupported surface format");

		if (header->flags & DDS_HEADER_FLAGS_VOLUME)
		{
			Type = TextureType::Texture3D;
		}
		else
		{
			if (header->caps2 & DDS_CUBEMAP)
			{
				//assert((header->caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES && "Cube Missing Faces");
				MiscFlags = ResourceMiscFlags::TexturedCube;
				ArraySize = 6;
				isCube = true;
				Type = TextureType::TextureCube;
			}
			else
			{
				Type = TextureType::Texture2D;
			}

			Depth = 1;
		}
	}

	// Dont trust dds with larger than hardware limited data
	if (MipCount > MAX_MIP_LEVELS)
	{
		delete[] data;
		assert(0 && "Too many mips");
		return;
	}

	// Check resource dimensions add up
	switch (Type)
	{
	case TextureType::Texture1D:
	{
		if ((ArraySize > MAX_TEXTURE1D_ARRAY_AXIS_DIMENSION) ||
			(Width > MAX_TEXTURE1D_U_DIMENSION))
		{
			assert(0 && "Texture1D: Resource Dimensions are too large");
		}
	}
	break;
	case TextureType::Texture2D:
	{
		if ((ArraySize > MAX_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
			(Width > MAX_TEXTURE2D_U_OR_V_DIMENSION) || (Height > MAX_TEXTURE2D_U_OR_V_DIMENSION))
		{
			assert(0 && "Texture2D: Resource Dimensions are too large");
		}
		Pitch = CalculatePitchSize(Format, Width, 1);//header->pitchOrLinearSize;
	}
	break;
	case TextureType::Texture3D:
	{
		if ((ArraySize > 1) ||(Width > MAX_TEXTURE3D_U_V_OR_W_DIMENSION) || 
			(Height > MAX_TEXTURE3D_U_V_OR_W_DIMENSION) ||
			(Depth > MAX_TEXTURE3D_U_V_OR_W_DIMENSION))
		{
			assert(0 && "Texture3D: Resource Dimensions are too large");
		}

		Pitch = CalculatePitchSize(Format, Width, Height);//header->pitchOrLinearSize;
	}
	break;
	case TextureType::TextureCube:
		Pitch = CalculatePitchSize(Format, Width, 1);//header->pitchOrLinearSize;
		break;
	default:
		assert(0 && "Unkown Resource Dimensions");
		break;
	}

	m_Data.resize(sizeOfData);
	memcpy(&m_Data[0], startOfData, sizeOfData);
	delete[] data;
}
