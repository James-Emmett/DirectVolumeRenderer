#include "Graphics/Graphics.h"
#include "Math/Mathf.h"

// DXGI_FORMAT CONVERION
const Uint32 FormatMapDXGI[73] =
{
	0,
	2,
	3,
	4,

	6,
	7,
	8,

	10,
	11,
	12,
	13,
	14,

	16,
	17,
	18,

	19,
	20,

	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,

	33,
	34,
	35,
	36,
	37,
	38,

	39,
	40,
	41,
	42,
	43,

	44,
	45,

	48,
	49,
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
	58,
	59,

	60,
	61,
	62,
	63,
	64,

	71,
	72,

	74,
	75,

	77,
	78,

	79,
	80,
	81,

	83,
	84,

	87,
	91,

	95,
	96,
	98,
	99,
};

const SurfaceFormat FormatMapFromDXGI[122] =
{
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::R32G32B32A32_Float,
	SurfaceFormat::R32G32B32A32_Uint,
	SurfaceFormat::R32G32B32A32_Sint,
	SurfaceFormat::Unkown,
	SurfaceFormat::R32G32B32_Float,
	SurfaceFormat::R32G32B32_Uint,
	SurfaceFormat::R32G32B32_Sint,
	SurfaceFormat::Unkown,
	SurfaceFormat::R16G16B16A16_Float,
	SurfaceFormat::R16G16B16A16_Unorm,
	SurfaceFormat::R16G16B16A16_Uint,
	SurfaceFormat::R16G16B16A16_Snorm,
	SurfaceFormat::R16G16B16A16_Sint,
	SurfaceFormat::Unkown,
	SurfaceFormat::R32G32_Float,
	SurfaceFormat::R32G32_Uint,
	SurfaceFormat::R32G32_Sint,
	SurfaceFormat::R32G8X24_Typeless,
	SurfaceFormat::D32_Float_S8X24_Uint,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::R10G10B10A2_Unorm,
	SurfaceFormat::R10G10B10A2_Uint,
	SurfaceFormat::R11G11B10_Float,
	SurfaceFormat::R8G8B8A8_Typeless,
	SurfaceFormat::R8G8B8A8_Unorm,
	SurfaceFormat::R8G8B8A8_Unorm_SRGB,
	SurfaceFormat::R8G8B8A8_Uint,
	SurfaceFormat::R8G8B8A8_Snorm,
	SurfaceFormat::R8G8B8A8_Sint,
	SurfaceFormat::R16G16_Typeless,
	SurfaceFormat::R16G16_Float,
	SurfaceFormat::R16G16_Unorm,
	SurfaceFormat::R16G16_Uint,
	SurfaceFormat::R16G16_Snorm,
	SurfaceFormat::R16G16_Sint,
	SurfaceFormat::R32_Typeless,
	SurfaceFormat::D32_Float,
	SurfaceFormat::R32_Float,
	SurfaceFormat::R32_Uint,
	SurfaceFormat::R32_Sint,
	SurfaceFormat::R24G8_Typeless,
	SurfaceFormat::D24_Unorm_S8_Uint,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::R8G8_Typeless,
	SurfaceFormat::R8G8_Unorm,
	SurfaceFormat::R8G8_Uint,
	SurfaceFormat::R8G8_Snorm,
	SurfaceFormat::R8G8_Sint,
	SurfaceFormat::R16_Typeless,
	SurfaceFormat::R16_Float,
	SurfaceFormat::D16_Unorm,
	SurfaceFormat::R16_Unorm,
	SurfaceFormat::R16_Uint,
	SurfaceFormat::R16_Snorm,
	SurfaceFormat::R16_Sint,
	SurfaceFormat::R8_Typeless,
	SurfaceFormat::R8_Unorm,
	SurfaceFormat::R8_Uint,
	SurfaceFormat::R8_Snorm,
	SurfaceFormat::R8_Sint,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::BC1_Unorm,
	SurfaceFormat::BC1_Unorm_SRGB,
	SurfaceFormat::Unkown,
	SurfaceFormat::BC2_Unorm,
	SurfaceFormat::BC2_Unorm_SRGB,
	SurfaceFormat::Unkown,
	SurfaceFormat::BC3_Unorm,
	SurfaceFormat::BC3_Unorm_SRGB,
	SurfaceFormat::BC4_Typeless,
	SurfaceFormat::BC4_Unorm,
	SurfaceFormat::BC4_Snorm,
	SurfaceFormat::Unkown,
	SurfaceFormat::BC5_Unorm,
	SurfaceFormat::BC5_Snorm,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::B8G8R8A8_Unorm,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::B8G8R8A8_Unorm_SRGB,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::BC6H_UF16,
	SurfaceFormat::BC6H_SF16,
	SurfaceFormat::Unkown,
	SurfaceFormat::BC7_Unorm,
	SurfaceFormat::BC7_Unorm_SRGB,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
	SurfaceFormat::Unkown,
};

Uint32 BytesPerBlock(SurfaceFormat format)
{
	switch (format)
	{
	case SurfaceFormat::Unkown:
		return 0;
		break;

	case SurfaceFormat::R32G32B32A32_Float:
	case SurfaceFormat::R32G32B32A32_Uint:
	case SurfaceFormat::R32G32B32A32_Sint:
	case SurfaceFormat::BC2_Unorm:
	case SurfaceFormat::BC2_Unorm_SRGB:
	case SurfaceFormat::BC3_Unorm:
	case SurfaceFormat::BC3_Unorm_SRGB:
	case SurfaceFormat::BC5_Unorm:
	case SurfaceFormat::BC5_Snorm:
	case SurfaceFormat::BC6H_UF16:
	case SurfaceFormat::BC6H_SF16:
	case SurfaceFormat::BC7_Unorm:
	case SurfaceFormat::BC7_Unorm_SRGB:
		return 16;
		break;

	case SurfaceFormat::R32G32B32_Float:
	case SurfaceFormat::R32G32B32_Uint:
	case SurfaceFormat::R32G32B32_Sint:
		return 12;
		break;

	case SurfaceFormat::R16G16B16A16_Float:
	case SurfaceFormat::R16G16B16A16_Unorm:
	case SurfaceFormat::R16G16B16A16_Uint:
	case SurfaceFormat::R16G16B16A16_Snorm:
	case SurfaceFormat::R16G16B16A16_Sint:
	case SurfaceFormat::R32G32_Float:
	case SurfaceFormat::R32G32_Uint:
	case SurfaceFormat::R32G32_Sint:
	case SurfaceFormat::R32G8X24_Typeless:
	case SurfaceFormat::BC1_Unorm:
	case SurfaceFormat::BC1_Unorm_SRGB:
		return 8;
		break;

	case SurfaceFormat::D32_Float_S8X24_Uint:
	case SurfaceFormat::R10G10B10A2_Unorm:
	case SurfaceFormat::R10G10B10A2_Uint:
	case SurfaceFormat::R11G11B10_Float:
	case SurfaceFormat::R8G8B8A8_Unorm:
	case SurfaceFormat::R8G8B8A8_Unorm_SRGB:
	case SurfaceFormat::R8G8B8A8_Uint:
	case SurfaceFormat::R8G8B8A8_Snorm:
	case SurfaceFormat::R8G8B8A8_Typeless:
	case SurfaceFormat::R8G8B8A8_Sint:
	case SurfaceFormat::R16G16_Float:
	case SurfaceFormat::R16G16_Unorm:
	case SurfaceFormat::R16G16_Uint:
	case SurfaceFormat::R16G16_Snorm:
	case SurfaceFormat::R16G16_Sint:
	case SurfaceFormat::R32_Typeless:
	case SurfaceFormat::D32_Float:
	case SurfaceFormat::R32_Float:
	case SurfaceFormat::R32_Uint:
	case SurfaceFormat::R32_Sint:
	case SurfaceFormat::R24G8_Typeless:
	case SurfaceFormat::D24_Unorm_S8_Uint:
	case SurfaceFormat::BC4_Unorm:
	case SurfaceFormat::BC4_Snorm:
	case SurfaceFormat::B8G8R8A8_Unorm:
	case SurfaceFormat::B8G8R8A8_Unorm_SRGB:
		return 4;
		break;

	case SurfaceFormat::R8G8_Unorm:
	case SurfaceFormat::R8G8_Uint:
	case SurfaceFormat::R8G8_Snorm:
	case SurfaceFormat::R8G8_Sint:
	case SurfaceFormat::R16_Typeless:
	case SurfaceFormat::R16_Float:
	case SurfaceFormat::D16_Unorm:
	case SurfaceFormat::R16_Unorm:
	case SurfaceFormat::R16_Uint:
	case SurfaceFormat::R16_Snorm:
	case SurfaceFormat::R16_Sint:
		return 2;
		break;

	case SurfaceFormat::R8_Typeless:
	case SurfaceFormat::R8G8_Typeless:
	case SurfaceFormat::R8_Unorm:
	case SurfaceFormat::R8_Uint:
	case SurfaceFormat::R8_Snorm:
	case SurfaceFormat::R8_Sint:
		return 1;
		break;

	default:
		return 0;
		break;
	}
}

Uint32 CalculateSurfaceSize(SurfaceFormat format, Uint32 width, Uint32 height)
{
	return (BytesPerBlock(format) * width) * height;
}

unsigned int CalculatePitchSize(SurfaceFormat format, Uint32 width, Uint32 height)
{
	switch (format)
	{
	case  SurfaceFormat::BC1_Unorm:
	case  SurfaceFormat::BC1_Unorm_SRGB:
	case  SurfaceFormat::BC3_Unorm:
	case  SurfaceFormat::BC3_Unorm_SRGB:
	{
		//Width needs to be in blocks!
		return ((width + 3) / 4) * CalculateSurfaceSize(format, width) * height;
	}

	default:
		return width * BytesPerBlock(format) * height;
	}
}

Uint32 CalculateTotalBytes(Uint32 width, Uint32 height, Uint32 depth, Uint32 mipCount, Uint32 arraySize, SurfaceFormat format)
{
	Uint32 totalBytes = 0;

	for (Uint32 i = 0; i < mipCount; i++)
	{
		//Shift right == divide each time.
		totalBytes += CalculateSurfaceSize(format, width >> i, height >> i) * (depth >> i);
	}

	return totalBytes * arraySize;
}

Uint32 CalculateTotalBytesFromDesc(TextureDesc& desc)
{
	Uint32 totalBytes = 0;

	for (Uint32 i = 0; i < desc.MipLevels; i++)
	{
		//Shift right == divide each time.
		totalBytes += CalculateSurfaceSize(desc.Format, desc.Width >> i, desc.Height >> i) * (desc.Depth >> i);
	}

	return totalBytes * desc.ArraySize;
}

Uint32 CalculateMipCount(Uint32 width, Uint32 height)
{
	return (Uint32)Mathf::Floor(Mathf::Log2((float)Mathf::Max((int)width, (int)height))) + 1;
}

bool IsCompressed(SurfaceFormat format)
{
	switch (format)
	{
	case SurfaceFormat::BC1_Unorm:
	case SurfaceFormat::BC1_Unorm_SRGB:
	case SurfaceFormat::BC2_Unorm:
	case SurfaceFormat::BC2_Unorm_SRGB:
	case SurfaceFormat::BC3_Unorm:
	case SurfaceFormat::BC3_Unorm_SRGB:
	case SurfaceFormat::BC4_Unorm:
	case SurfaceFormat::BC4_Snorm:
	case SurfaceFormat::BC5_Unorm:
	case SurfaceFormat::BC5_Snorm:
	case SurfaceFormat::BC6H_UF16:
	case SurfaceFormat::BC6H_SF16:
	case SurfaceFormat::BC7_Unorm:
	case SurfaceFormat::BC7_Unorm_SRGB:
		return true;

	default:
		return false;
	}
}

bool IsFormatSRGB(SurfaceFormat format)
{
	switch (format)
	{
	case SurfaceFormat::R8G8B8A8_Unorm_SRGB:
	case SurfaceFormat::BC1_Unorm_SRGB:
	case SurfaceFormat::BC2_Unorm_SRGB:
	case SurfaceFormat::BC3_Unorm_SRGB:
	case SurfaceFormat::B8G8R8A8_Unorm_SRGB:
	case SurfaceFormat::BC7_Unorm_SRGB:
		return true;

	default:
		return false;
	}
}

SurfaceFormat LinearToSRGBFormat(SurfaceFormat format)
{
	switch (format)
	{
	case SurfaceFormat::R8G8B8A8_Unorm:
		return SurfaceFormat::R8G8B8A8_Unorm_SRGB;
		break;

	case SurfaceFormat::BC1_Unorm:
		return SurfaceFormat::BC1_Unorm_SRGB;
		break;

	case SurfaceFormat::BC2_Unorm:
		return SurfaceFormat::BC2_Unorm_SRGB;
		break;

	case SurfaceFormat::BC3_Unorm:
		return SurfaceFormat::BC3_Unorm_SRGB;
		break;

	case SurfaceFormat::B8G8R8A8_Unorm:
		return SurfaceFormat::B8G8R8A8_Unorm_SRGB;
		break;

	case SurfaceFormat::BC7_Unorm:
		return SurfaceFormat::BC7_Unorm_SRGB;
		break;

	case SurfaceFormat::Unkown:
	default:
		break;
	}

	return format;
}

SurfaceFormat SRGBToLinearFormat(SurfaceFormat format)
{
	switch (format)
	{
	case SurfaceFormat::R8G8B8A8_Unorm_SRGB:
		return SurfaceFormat::R8G8B8A8_Unorm;
		break;

	case SurfaceFormat::BC1_Unorm_SRGB:
		return SurfaceFormat::BC1_Unorm;
		break;

	case SurfaceFormat::BC2_Unorm_SRGB:
		return SurfaceFormat::BC2_Unorm;
		break;

	case SurfaceFormat::BC3_Unorm_SRGB:
		return SurfaceFormat::BC3_Unorm;
		break;

	case SurfaceFormat::B8G8R8A8_Unorm_SRGB:
		return SurfaceFormat::B8G8R8A8_Unorm;
		break;

	case SurfaceFormat::BC7_Unorm_SRGB:
		return SurfaceFormat::BC7_Unorm;
		break;

	case SurfaceFormat::Unkown:
	default:
		break;
	}

	return format;
}

bool IsDepthStencil(SurfaceFormat format)
{
	switch (format)
	{
	case SurfaceFormat::R24G8_Typeless:
	case SurfaceFormat::R32G8X24_Typeless:
	case SurfaceFormat::D24_Unorm_S8_Uint:
	case SurfaceFormat::D32_Float_S8X24_Uint:
		return true;
		break;
	}

	return false;
}

Uint32 FormatToDXGIFormat(SurfaceFormat format)
{
	return FormatMapDXGI[(Uint32)format];
}

SurfaceFormat DXGIFormatToFormat(Uint32 format)
{
	return FormatMapFromDXGI[format];
}