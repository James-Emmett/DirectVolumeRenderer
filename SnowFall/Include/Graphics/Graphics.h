#pragma once
#include "System/Types.h"
#include "Math/Color.h"
#include "System/Assert.h"
#include <string>
#include <vector>

typedef Handle BufferHandle;
typedef Handle TextureHandle;
typedef Handle RenderHandle;
typedef Handle DepthHandle;
typedef Handle PipelineHandle;
typedef Handle ShaderHandle;
typedef HandlePlain SamplerHandle;
typedef Int16  CommandList;

// 2 for double buffer, 3 for tripple
const Uint32 BUFFER_COUNT = 3;
const Uint32 COMMANDLIST_COUNT = 4;
const Uint32 MAX_TEXTURE_DIMENSION = 16384;
const Uint32 MAX_MIP_LEVELS = 15;
const Uint32 MAX_TEXTURE1D_ARRAY_AXIS_DIMENSION = 2048;
const Uint32 MAX_TEXTURE2D_ARRAY_AXIS_DIMENSION = 2048;
const Uint32 MAX_TEXTURECUBE_DIMENSION = 16384;
const Uint32 MAX_TEXTURE1D_U_DIMENSION = 16384;
const Uint32 MAX_TEXTURE2D_U_OR_V_DIMENSION = 16384;
const Uint32 MAX_TEXTURE3D_U_V_OR_W_DIMENSION = 2048;
const Uint32 TEXTURE_SLOTS = 16;
const Uint32 CONSTANT_SLOTS = 14;
const Uint32 UAV_SLOTS = 32;
const Uint32 SAMPLER_SLOTS = 16;
const Uint32 VIEWPORT_SLOTS = 16;

enum class ShaderType
{
	VS,
	PS,
	HS,
	DS,
	GS,
	CS
};

enum class PrimitiveTopology
{
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	PatchList,
};

enum class FillMode
{
	Wireframe,
	Solid
};

enum class CullMode
{
	None,
	Front,
	Back
};

enum class Blend
{
	Zero,
	One,
	Src_Color,
	Inv_Src_Color,
	Src_Alpha,
	Inv_Src_Alpha,
	Dest_Color,
	Inv_Dest_Color,
	Dest_Alpha,
	Inv_Dest_Alpha,
	Blend_Factor,
	Inv_Blend_Factor,
	Src1_Color,
	Inv_Src1_Color,
	Src1_Alpha,
	Inv_Src1_Alpha,
	Src_Alpha_Sat
};

enum class BlendOp
{
	Add,
	Subtract,
	ReverseSubtract,
	Min,
	Max
};

enum ColorWrite
{
	Red,
	Green,
	Blue,
	Alpha,
	All
};

enum class ComparisonFunc
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	GreaterEqual,
	NotEqual,
	Always
};

enum class StencilOp
{
	Zero,
	Keep,
	Replace,
	Increment,
	Decrement,
	IncrementSat,
	DecrementSat,
	Invert
};

enum class DepthWrite
{
	Zero,
	All
};

enum class BufferUsage
{
	Default,
	Immutable,
	Dynamic,
	Staging
};

enum class CpuAccess
{
	Immutable,
	Write,
	Read,
	ReadWrite
};

enum class BindFlag
{
	Unknown = 0,
	VertexBuffer = 0x1L,
	IndexBuffer = 0x2L,
	ConstantBuffer = 0x4L,
	ShaderResource = 0x8L,
	RenderTarget = 0x10L,
	DepthStencil = 0x40L,
	UnorderedAccess = 0x80L
};

enum ResourceMiscFlags
{
	Shared = 0x1L,
	TexturedCube = 0x2L,
	IndirectArgs = 0x4L,
	BufferAllowRawViews = 0x8L,
	BufferStructured = 0x10L,
	MiscTiled = 0x40L,
};

enum class InputType
{
	VertexData,
	InstanceData
};

enum class FilterMode
{
	MinMagMipPoint,
	MinMagPointMipLinear,
	MinPointMagLinearMipPoint,
	MinPointMagMipLinear,
	MininearMagMipPoint,
	MinLinearMagPointMipLinear,
	MinMagLinearMipPoint,
	MinMagMipLinear,
	Anisotropic,
	ComparisonMinMagMipPoint,
	ComparisonMinMagPointMipLinear,
	ComparisonMinPointMagLinearMipPoint,
	ComparisonMinPointMagMipLinear,
	ComparisonMinLinearMagMipPoint,
	ComparisonMinLinearMagPointMipLinear,
	ComparisonMinMagLinearMipPoint,
	ComparisonMinMagMipLinear,
	ComparisonAnisotropic,
	MinimunMinMagMipPoint,
	MinimunMinMagPointMipLinear,
	MinimunMinPointMagLinearMipPoint,
	MinimunMinPointMagMipLinear,
	MinimunMinLinearMagMipPoint,
	MinimunMinLinearMagPointMipLinear,
	MinimunMinMagLinearMipPoint,
	MinimunMinMagMipLinear,
	MinimunAnisotropic,
	MaximunMinMagMipPoint,
	MaximunMinMagPointMipLinear,
	MaximunMinPointMagLinearMipPoint,
	MaximunMinPointMagMipLinear,
	MaximunMinLinearMagMipPoint,
	MaximunMinLinearMagPointMipLinear,
	MaximunMinMagLinearMipPoint,
	MaximunMinMagMipLinear,
	MaximunAnisotropic,
};

enum class SurfaceFormat
{
	Unkown,
	R32G32B32A32_Float,
	R32G32B32A32_Uint,
	R32G32B32A32_Sint,

	R32G32B32_Float,
	R32G32B32_Uint,
	R32G32B32_Sint,

	R16G16B16A16_Float,
	R16G16B16A16_Unorm,
	R16G16B16A16_Uint,
	R16G16B16A16_Snorm,
	R16G16B16A16_Sint,

	R32G32_Float,
	R32G32_Uint,
	R32G32_Sint,

	R32G8X24_Typeless,
	D32_Float_S8X24_Uint,

	R10G10B10A2_Unorm,
	R10G10B10A2_Uint,
	R11G11B10_Float,

	R8G8B8A8_Typeless,
	R8G8B8A8_Unorm,
	R8G8B8A8_Unorm_SRGB,
	R8G8B8A8_Uint,
	R8G8B8A8_Snorm,
	R8G8B8A8_Sint,

	R16G16_Typeless,
	R16G16_Float,
	R16G16_Unorm,
	R16G16_Uint,
	R16G16_Snorm,
	R16G16_Sint,

	R32_Typeless,
	D32_Float,
	R32_Float,
	R32_Uint,
	R32_Sint,

	R24G8_Typeless,
	D24_Unorm_S8_Uint,

	R8G8_Typeless,
	R8G8_Unorm,
	R8G8_Uint,
	R8G8_Snorm,
	R8G8_Sint,
	R16_Typeless,
	R16_Float,
	D16_Unorm,
	R16_Unorm,
	R16_Uint,
	R16_Snorm,
	R16_Sint,

	R8_Typeless,
	R8_Unorm,
	R8_Uint,
	R8_Snorm,
	R8_Sint,

	BC1_Unorm,
	BC1_Unorm_SRGB,

	BC2_Unorm,
	BC2_Unorm_SRGB,

	BC3_Unorm,
	BC3_Unorm_SRGB,

	BC4_Typeless,
	BC4_Unorm,
	BC4_Snorm,

	BC5_Unorm,
	BC5_Snorm,

	B8G8R8A8_Unorm,
	B8G8R8A8_Unorm_SRGB,

	BC6H_UF16,
	BC6H_SF16,

	BC7_Unorm,
	BC7_Unorm_SRGB,
};

enum class WrapMode
{
	Repeat,
	Mirror,
	Clamp,
	Border,
	Mirror_Once
};

enum class RenderFormat
{
	// Used for LDR Targets, i.e Swapchain Back Buffer
	R8G8B8A8_Unorm = (int)SurfaceFormat::R8G8B8A8_Unorm,
	// Use for HDR but stores less color range
	R10G10B10A2_Unorm = (int)SurfaceFormat::R10G10B10A2_Unorm,
	// Use for HDR Targets
	R16G16B16A16_Float = (int)SurfaceFormat::R16G16B16A16_Float,
	// Use for HDR Targets
	R32G32B32A23_Float = (int)SurfaceFormat::R32G32B32A32_Float
};


enum class DepthFormat
{
	Depth16 = (Uint32)SurfaceFormat::R16_Typeless,
	Depth24Stencil8 = (Uint32)SurfaceFormat::R24G8_Typeless,
	Depth32 = (Uint32)SurfaceFormat::R32_Typeless,
	Depth32Stencil8x24 = (Uint32)SurfaceFormat::R32G8X24_Typeless,
};

enum class TextureType
{
	Texture1D,
	Texture2D,
	Texture3D,
	TextureCube
};

enum class VertexType
{
	Vertex = 12,
	VertexTexture = 20,
	VertexColorTexture = 36,
	VertexNormal = 48,
	VertexMesh = 64,
	VertexSkimmedMesh = 92
};

enum class RoundMode
{
	None,
	Next,
	Previous,
	Nearest
};

enum class IndexFormat
{
	I16 = (Uint32)SurfaceFormat::R16_Uint,
	I32 = (Uint32)SurfaceFormat::R32_Uint
};

enum class RenderType
{
	Opaque,
	Transparent,
};

struct BufferDesc
{
	Uint32			ByteWidth = 0;
	BufferUsage		Usage = BufferUsage::Default;
	Uint32			Bind = (Uint32)BindFlag::Unknown;
	CpuAccess		CpuFlags = CpuAccess::Immutable;
	Uint32			MiscFlags = 0;
	Uint32			ByteStride = 0;
	SurfaceFormat	Format = SurfaceFormat::Unkown; // USed by SRV accessed buffer only
};

struct MultiSample
{
	Uint32 Count = 1;
	Uint32 Quality = 0;

	MultiSample()
	{
		Count = 1;
		Quality = 0;
	}
};

struct TextureDesc
{
	Uint32			Width = 1;
	Uint32			Height = 1;
	Uint32			Depth = 1;
	Uint32			ArraySize = 1;
	Uint32			MipLevels = 1;
	SurfaceFormat	Format = SurfaceFormat::Unkown;
	MultiSample		SampleDesc = {};
	BufferUsage		Usage = BufferUsage::Default;
	Uint32			BindFlags = 0;
	CpuAccess		CPUAccessFlags = CpuAccess::Immutable;
	Uint32			MiscFlags = 0;
	TextureType		Type = TextureType::Texture2D;
	Uint32			Pitch = 0;
	Uint32			ByteCount = 0;
	Int16			SRGB = 1;
};

struct DepthOp
{
	StencilOp      StencilFailOp = StencilOp::Keep;
	StencilOp      StencilDepthFailOp = StencilOp::Keep;
	StencilOp      StencilPassOp = StencilOp::Keep;
	ComparisonFunc StencilFunc = ComparisonFunc::Never;
};

struct RenderTargetBlend
{
	bool	BlendEnabled = false;
	Blend	SrcColorBlend = Blend::One;
	Blend	DestColorBlend = Blend::Zero;
	BlendOp ColorBlendOp = BlendOp::Add;
	Blend	SrcAlphaBlend = Blend::One;
	Blend	DestAlphaBlend = Blend::Zero;
	BlendOp AlphaBlendOp = BlendOp::Add;
	Uint8	ColorWriteMask = ColorWrite::All;
};

struct SamplerDesc
{
	FilterMode		Filter = FilterMode::MinMagMipLinear;
	WrapMode		AddressU = WrapMode::Repeat;
	WrapMode		AddressV = WrapMode::Repeat;
	WrapMode		AddressW = WrapMode::Repeat;
	float			MipLodBias = 0;
	Uint32			MaxAnisotropy = 0;
	ComparisonFunc  ComparisonOp = ComparisonFunc::Never;
	float			BorderColor[4] = { 1 };
	float			MinLod = FLT_MIN;
	float			MaxLod = FLT_MAX;
};

struct RasterDesc
{
	FillMode  Fill = FillMode::Solid;
	CullMode  CullMode = CullMode::None;
	bool      FrontClockWise = false; // True: Front clockwise | False: Front counter Clockwise
	int       DepthBias = 0;
	float     DepthBiasClamp = 0.0f;
	float     SlopeDepthBias = 0.0f;
	bool      DepthClipEnable = true;
	bool      MultisampleEnable = false;
	bool      AntialiasedLineEnable = false;
};

struct DepthDesc
{
	bool			DepthEnabled = false;
	DepthWrite		DepthWriteMask = DepthWrite::Zero;
	ComparisonFunc	DepthFunc = ComparisonFunc::Never;
	bool			StencilEnabled = false;
	Uint32			StencilRead = 0xFF;
	Uint32			StencilWrite = 0xFF;
	DepthOp			FrontFace;
	DepthOp			BackFace;
};

struct BlendDesc
{
	bool AlphaToCoverageEnabled = false;
	bool IndependentBlendEnabled = false;
	RenderTargetBlend RenderTarget[8];
};

struct InputElement
{
	static const Uint32 ALIGNED_ELEMENT = 0xffffffff; // automatically figure out AlignedByteOffset depending on Format

	std::string		SemanticName;
	Uint32			SemanticIndex = 0;
	SurfaceFormat	Format = SurfaceFormat::Unkown;
	Uint32			InputSlot = 0;
	Uint32			AlignedByteOffset = ALIGNED_ELEMENT;
	InputType		InputSlotClass = InputType::VertexData;
	Uint32			InstanceDataStepRate = 0;

	InputElement() {}
	InputElement(std::string name, Uint32 index, Uint32 slot, SurfaceFormat format, Uint32 alginment, InputType type, Uint32 stepRate)
	{
		SemanticName = name;
		SemanticIndex = index;
		Format = format;
		InputSlot = slot;
		AlignedByteOffset = alginment;
		InputSlotClass = type;
		InstanceDataStepRate = stepRate;
	}
};


struct InputLayoutDesc
{
public:
	std::vector<InputElement> m_LayoutElements;

public:
	InputLayoutDesc() { m_LayoutElements.clear(); }
	InputLayoutDesc(const InputElement* layoutElements, Uint32 elementCount)
	{
		for (Uint32 i = 0; i < elementCount; i++)
		{
			m_LayoutElements.push_back(layoutElements[i]);
		}
	}

	void Add(InputElement layout)
	{
		m_LayoutElements.push_back(layout);
	}

	void Clear()
	{
		m_LayoutElements.clear();
	}

	Uint32 Size()const
	{
		return (Uint32)m_LayoutElements.size();
	}

	bool IsValid()const
	{
		return !m_LayoutElements.empty();
	}
};

struct PipelineDesc
{
	ShaderHandle		VertexShader;
	ShaderHandle		PixelShader;
	ShaderHandle		HullShader;
	ShaderHandle		DomainShader;
	ShaderHandle		GeometryShader;
	BlendDesc			BlendState;
	RasterDesc			RasterState;
	DepthDesc			DepthState;
	InputLayoutDesc		InputLayout;
	PrimitiveTopology	Topology = PrimitiveTopology::TriangleList;
	uint32_t			SampleMask = 0xFFFFFFFF; // I always forget this an it leads to lots of bugs
};

enum class TargetOperation
{
	None,
	Clear
};

struct RenderAttachment
{
	TargetOperation	m_Operation = TargetOperation::Clear;
	TextureHandle	m_Texture = TextureHandle();
	Color			m_ClearColor = Color::Black;

	RenderAttachment()
	{
		m_Operation = TargetOperation::Clear;
		m_Texture = TextureHandle();
		m_ClearColor = Color::Black;
	}

	RenderAttachment(TextureHandle texture, TargetOperation operation = TargetOperation::Clear, Color clear = Color::Black)
	{
		m_Operation = operation;
		m_Texture = texture;
		m_ClearColor = Color::Black;
	}

	~RenderAttachment()
	{

	}
};

struct DepthAttachment
{
	DepthHandle		m_DepthTarget = DepthHandle();
	TargetOperation	m_Operation = TargetOperation::Clear;
	float			m_Depth = 1;
	Uint32			m_Stencil = 0;
};

struct RenderTargetGroup
{
	std::vector<RenderAttachment> m_RenderTargets;
	DepthAttachment m_DepthAttachment;

	~RenderTargetGroup()
	{
	}

	void AddAttachment(RenderAttachment attachment)
	{
		assert(m_RenderTargets.size() < 8 && "Too many render targets bound");
		m_RenderTargets.push_back(attachment);
	}

	void SetDepth(DepthHandle handle, float depth = 1, Uint32 stencil = 0)
	{
		m_DepthAttachment.m_DepthTarget = handle;
		m_DepthAttachment.m_Depth = depth;
		m_DepthAttachment.m_Stencil = stencil;
	}

	Uint32 Count()const
	{
		return (Uint32)m_RenderTargets.size();
	}
};

struct ViewPort
{
	float X = 0.0f;
	float Y = 0.0f;
	float Width = 0.0f;
	float Height = 0.0f;
	float MinDepth = 0.0f;
	float MaxDepth = 1.0f;

	ViewPort() {}
	ViewPort(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1)
	{
		X = x;
		Y = y;
		Width = width;
		Height = height;
		MinDepth = minDepth;
		MaxDepth = maxDepth;
	}

	float AspectRatio()const
	{
		return Width / Height;
	}
};

//8 bytes
struct AdapterID
{
	Dword LowPart;
	long  HighPart;

	AdapterID()
	{
		LowPart = 0;
		HighPart = 0;
	}

	AdapterID(Dword low, long high)
	{
		LowPart = low;
		HighPart = high;
	}

	bool operator==(const AdapterID& id)const
	{
		return LowPart == id.LowPart && HighPart == id.HighPart;
	}

	bool operator!=(const AdapterID& id)const
	{
		return !(*this == id);
	}
};

struct VsyncData
{
	bool   m_Enabled = true;
	Uint8  m_Value = 1;
};

struct GraphicsParameters
{
	Uint32          Width = 1280;
	Uint32			Height = 720;
	RenderFormat    BackBufferFormat = RenderFormat::R8G8B8A8_Unorm;
	DepthFormat     DepthStencilFormat = DepthFormat::Depth32;
	MultiSample     MultiSampleing;
	WindowHandle    Handle = nullptr;
	VsyncData       Vsync;
	bool            Fullscreen = false;
	bool            Stero = false;
	bool			HDR = false;
	int             RefreshRate = 0;
};

// Represents memory allocations allowed by the device
struct GPUMemory
{
	Uint32 m_MaxTextures = 2048;	// Total Generic textures allowed.
	Uint32 m_MaxBuffers = 2048; // Total Generic buffers allowed.
	Uint32 m_MaxRenderTarget = 1024; // Max Textures that can be Render target Bound
	Uint32 m_MaxDepthTarget = 1024; // Max Textures that can be Depth target Bound
	Uint32 m_MaxConstants = 1024;	// Max Buffers that can be Constant Bound
	Uint32 m_MaxUnordered = 1024;	// Max Buffers that can be Unordered Access Buffers
	Uint32 m_MaxPipelineStates = 1024; // Max PSO's that can be created by engine
	Uint32 m_MaxSamplerStates = 64;	// Max samplers that can be created
	Uint32 m_MaxShaders = 1024;
};

// Returns Byte count Per block
Uint32 BytesPerBlock(SurfaceFormat format);
// Returns Size of Single Surface in bytes in memory.
inline Uint32 CalculateSurfaceSize(SurfaceFormat format, Uint32 width, Uint32 height = 1);
// Pitch of the textue, the size in bytes of a single row
unsigned int CalculatePitchSize(SurfaceFormat format, Uint32 width, Uint32 height = 1);
// Total Memory consumed by a texture
Uint32 CalculateTotalBytes(Uint32 width, Uint32 height, Uint32 depth, Uint32 mipCount, Uint32 arraySize, SurfaceFormat format);
Uint32 CalculateTotalBytesFromDesc(TextureDesc& desc);
// How many mips the texture contains including top level.
Uint32 CalculateMipCount(Uint32 width, Uint32 height);
// Retruns true if the format is a compressed format
bool IsCompressed(SurfaceFormat format);
// Return true if the Format is that of SRGB
bool IsFormatSRGB(SurfaceFormat format);
// Maps Non-SRGB into SRGB otherwise returns input as output.
SurfaceFormat LinearToSRGBFormat(SurfaceFormat format);
// Maps SRGB to a Linear format, otherwise returns input as output.
SurfaceFormat SRGBToLinearFormat(SurfaceFormat format);
// Returns true if the format can be (RG24B8) or is a depth format (D24S8)
bool IsDepthStencil(SurfaceFormat format);
Uint32 FormatToDXGIFormat(SurfaceFormat format);
SurfaceFormat DXGIFormatToFormat(Uint32 format);