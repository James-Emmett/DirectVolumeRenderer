#include "Graphics/D3D11/GraphicsDevice_D3D11.h"
#include "Graphics/CommonStates.h"
#include "Graphics/VertexTypes.h"
#include "System/StringUtil.h"
#include "System/Logger.h"
#include "System/Window.h"
#include <sstream>
#include <System\Hash32.h>


// Saw this in wicked, a few Game dev net posts seem to do the same.
const void* const g_NullResources[128] = {};

const D3D11_FILTER FilterTable[36] =
{
    D3D11_FILTER_MIN_MAG_MIP_POINT,
    D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
    D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
    D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    D3D11_FILTER_ANISOTROPIC,
    D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
    D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
    D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
    D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
    D3D11_FILTER_COMPARISON_ANISOTROPIC,
    D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT,
    D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
    D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
    D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
    D3D11_FILTER_MINIMUM_ANISOTROPIC,
    D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
    D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
    D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
    D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
    D3D11_FILTER_MAXIMUM_ANISOTROPIC
};

const D3D11_TEXTURE_ADDRESS_MODE WrapModeTable[5] =
{
    D3D11_TEXTURE_ADDRESS_WRAP,
    D3D11_TEXTURE_ADDRESS_MIRROR,
    D3D11_TEXTURE_ADDRESS_CLAMP,
    D3D11_TEXTURE_ADDRESS_BORDER,
    D3D11_TEXTURE_ADDRESS_MIRROR_ONCE
};

const D3D11_COMPARISON_FUNC ComparisonTable[8] =
{
    D3D11_COMPARISON_NEVER,
    D3D11_COMPARISON_LESS,
    D3D11_COMPARISON_EQUAL,
    D3D11_COMPARISON_LESS_EQUAL,
    D3D11_COMPARISON_GREATER,
    D3D11_COMPARISON_GREATER_EQUAL,
    D3D11_COMPARISON_NOT_EQUAL,
    D3D11_COMPARISON_ALWAYS
};

const D3D11_STENCIL_OP StencilOpTable[8] =
{
    D3D11_STENCIL_OP_ZERO,
    D3D11_STENCIL_OP_KEEP,
    D3D11_STENCIL_OP_REPLACE,
    D3D11_STENCIL_OP_INCR,
    D3D11_STENCIL_OP_DECR,
    D3D11_STENCIL_OP_INCR_SAT,
    D3D11_STENCIL_OP_DECR_SAT,
    D3D11_STENCIL_OP_INVERT
};

const D3D11_BLEND BlendTable[17] =
{
    D3D11_BLEND_ZERO,
    D3D11_BLEND_ONE,
    D3D11_BLEND_SRC_COLOR,
    D3D11_BLEND_INV_SRC_COLOR,
    D3D11_BLEND_SRC_ALPHA,
    D3D11_BLEND_INV_SRC_ALPHA,
    D3D11_BLEND_DEST_COLOR,
    D3D11_BLEND_INV_DEST_COLOR,
    D3D11_BLEND_DEST_ALPHA,
    D3D11_BLEND_INV_DEST_ALPHA,
    D3D11_BLEND_BLEND_FACTOR,
    D3D11_BLEND_INV_BLEND_FACTOR,
    D3D11_BLEND_SRC1_COLOR,
    D3D11_BLEND_INV_SRC1_COLOR,
    D3D11_BLEND_SRC1_ALPHA,
    D3D11_BLEND_INV_SRC1_ALPHA,
    D3D11_BLEND_SRC_ALPHA_SAT
};

const D3D11_BLEND_OP BlendOpTable[5] =
{
    D3D11_BLEND_OP_ADD,
    D3D11_BLEND_OP_SUBTRACT,
    D3D11_BLEND_OP_REV_SUBTRACT,
    D3D11_BLEND_OP_MIN,
    D3D11_BLEND_OP_MAX
};

const D3D11_FILL_MODE FillModeTable[2] =
{
    D3D11_FILL_WIREFRAME,
    D3D11_FILL_SOLID
};

const D3D11_CULL_MODE CullModeTable[3] =
{
    D3D11_CULL_NONE,
    D3D11_CULL_FRONT,
    D3D11_CULL_BACK
};

const D3D11_DEPTH_WRITE_MASK DepthWriteTable[2] =
{
    D3D11_DEPTH_WRITE_MASK_ZERO,
    D3D11_DEPTH_WRITE_MASK_ALL
};

const D3D11_INPUT_CLASSIFICATION InputClassTable[2] =
{
    D3D11_INPUT_PER_VERTEX_DATA,
    D3D11_INPUT_PER_INSTANCE_DATA
};

const D3D11_USAGE UsageTable[4] =
{
    D3D11_USAGE_DEFAULT,
    D3D11_USAGE_IMMUTABLE,
    D3D11_USAGE_DYNAMIC,
    D3D11_USAGE_STAGING
};

const Uint32 CpuFlagTable[4] =
{
    0,
    D3D11_CPU_ACCESS_WRITE,
    D3D11_CPU_ACCESS_READ,
    D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ
};

const D3D11_PRIMITIVE_TOPOLOGY TopologyTable[6] =
{
    D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
    D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
    D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST,
};


Uint32 ConvertColorMask(Uint32 value)
{
    Uint32 flag = 0;

    if (value == ColorWrite::All)
    {
        return D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    else
    {
        if (value & ColorWrite::Red)
        {
            flag |= D3D11_COLOR_WRITE_ENABLE_RED;
        }

        if (value & ColorWrite::Green)
        {
            flag |= D3D11_COLOR_WRITE_ENABLE_GREEN;
        }

        if (value & ColorWrite::Blue)
        {
            flag |= D3D11_COLOR_WRITE_ENABLE_BLUE;
        }

        if (value & ColorWrite::Alpha)
        {
            flag |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
        }
    }

    return flag;
}

Uint32 ConvertBindFlags(Uint32 value)
{
    Uint32 flag = 0;

    if (value & (Uint32)BindFlag::VertexBuffer)
    {
        flag |= D3D11_BIND_VERTEX_BUFFER;
    }

    if (value & (Uint32)BindFlag::IndexBuffer)
    {
        flag |= D3D11_BIND_INDEX_BUFFER;
    }

    if (value & (Uint32)BindFlag::ConstantBuffer)
    {
        flag |= D3D11_BIND_CONSTANT_BUFFER;
    }

    if (value & (Uint32)BindFlag::ShaderResource)
    {
        flag |= D3D11_BIND_SHADER_RESOURCE;
    }

    if (value & (Uint32)BindFlag::RenderTarget)
    {
        flag |= D3D11_BIND_RENDER_TARGET;
    }

    if (value & (Uint32)BindFlag::DepthStencil)
    {
        flag |= D3D11_BIND_DEPTH_STENCIL;
    }

    if (value & (Uint32)BindFlag::UnorderedAccess)
    {
        flag |= D3D11_BIND_UNORDERED_ACCESS;
    }

    return flag;
}

Uint32 ConvertMiscFlags(Uint32 value)
{
    Uint32 flag = 0;

    if (value & ResourceMiscFlags::Shared)
    {
        flag |= D3D11_RESOURCE_MISC_SHARED;
    }

    if (value & ResourceMiscFlags::TexturedCube)
    {
        flag |= D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    if (value & ResourceMiscFlags::IndirectArgs)
    {
        flag |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    }

    if (value & ResourceMiscFlags::BufferAllowRawViews)
    {
        flag |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    }

    if (value & ResourceMiscFlags::BufferStructured)
    {
        flag |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    }

    if (value & ResourceMiscFlags::MiscTiled)
    {
        flag |= D3D11_RESOURCE_MISC_TILED;
    }

    return flag;
}

bool GraphicsDevice::Initialize(const GraphicsParameters& info, const GPUMemory& memoryLimits, const GraphicsAdapter* adapter)
{
    HRESULT hr = S_OK;
    m_Parameters = info;
    m_MemoryLimits = memoryLimits;
    m_Adapter = adapter != nullptr ? adapter : GraphicsAdapter::GetDefaultAdapter();
    m_ClearColor = Color::CornflowerBlue;

#if defined(DEBUG)
    m_CompileFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Create Factory
    hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory));
    assert(SUCCEEDED(hr));

    m_Factory->MakeWindowAssociation((HWND)m_Parameters.Handle, DXGI_MWA_NO_ALT_ENTER);

    // Grab adapter/default from LUID
    AdapterID id = m_Adapter->ID();

    LUID adapterID;
    adapterID.HighPart = id.HighPart;
    adapterID.LowPart = id.LowPart;

    IDXGIAdapter* pAdapter = nullptr;
    hr = m_Factory->EnumAdapterByLuid(adapterID, __uuidof(IDXGIAdapter), (void**)&pAdapter);
    assert(SUCCEEDED(hr));

    D3D_FEATURE_LEVEL desiredLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };

    // Create device for GPU interface
    hr = D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, 0, m_CompileFlags, desiredLevels, 1, D3D11_SDK_VERSION, &m_Device, &m_FeatureLevel, &m_DeviceContexts[0]);
    assert(SUCCEEDED(hr));

    pAdapter->Release();

    // Debug Device
#ifdef DEBUG
    hr = m_Device->QueryInterface(IID_PPV_ARGS(&m_Debug));
    assert(SUCCEEDED(hr));
#endif

    //--Begin SwapChain--
    DXGI_MODE_DESC backDesc = {};
    backDesc.Width = m_Parameters.Width;
    backDesc.Height = m_Parameters.Height;
    backDesc.Format = (DXGI_FORMAT)FormatToDXGIFormat((SurfaceFormat)m_Parameters.BackBufferFormat); // Non SRGB or it crashes!
    backDesc.RefreshRate.Numerator = 0;
    backDesc.RefreshRate.Denominator = 1;

    DXGI_SAMPLE_DESC sampleDesc = {};
    sampleDesc.Count = m_Parameters.MultiSampleing.Count;
    sampleDesc.Quality = m_Parameters.MultiSampleing.Quality;

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferDesc = backDesc;
    swapDesc.BufferCount = BUFFER_COUNT;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.OutputWindow = (HWND)m_Parameters.Handle;
    swapDesc.SampleDesc = sampleDesc;
    swapDesc.Windowed = true;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    IDXGISwapChain* sc = nullptr;
    hr = m_Factory->CreateSwapChain(m_Device, &swapDesc, &sc);
    assert(SUCCEEDED(hr));
    m_SwapChain = static_cast<IDXGISwapChain3*>(sc);
    //--End SwapChain--

    //--Get Render Target--
    hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&m_BackBuffer));
    assert(SUCCEEDED(hr));

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = (DXGI_FORMAT)FormatToDXGIFormat((SurfaceFormat)m_Parameters.BackBufferFormat);
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    if (FAILED(m_Device->CreateRenderTargetView(m_BackBuffer, &rtvDesc, &m_RenderTarget)))
    {
        ShutDown();
        LogError("Failed to Create Back Buffer.");
        return false;
    }

    //--Create the Depth Target--
    CreateDefaultDetphTarget();

    //--Set Targets--
    m_DeviceContexts[0]->OMSetRenderTargets(1, &m_RenderTarget, m_DepthTarget);

    // Set view/scissor to size of back buffer
    BindDefaultViewPortAndScissor();

    m_Buffers.Initialize(m_MemoryLimits.m_MaxBuffers);
    m_Textures.Initialize(m_MemoryLimits.m_MaxTextures);
    m_Pipelines.Initialize(m_MemoryLimits.m_MaxPipelineStates);
    m_Shaders.Initialize(m_MemoryLimits.m_MaxShaders);
    m_SamplerMap.Initialize(m_MemoryLimits.m_MaxSamplerStates);
    m_InputMap.Initialize(m_MemoryLimits.m_MaxShaders);

    // Shouldnt need more than 64 its overkill engine
    // defaults should be more than enough!
    m_BlendMap.Initialize(64);
    m_DepthMap.Initialize(64);
    m_RasterMap.Initialize(64);

    return InitializeBlitter();
}

void GraphicsDevice::Reset(GraphicsParameters info)
{
    m_Parameters = info;
    if (m_RenderTarget) { m_RenderTarget->Release(); }
    if (m_BackBuffer) { m_BackBuffer->Release(); }

    // Re-set swp chain
    assert(SUCCEEDED(m_SwapChain->ResizeBuffers(BUFFER_COUNT, m_Parameters.Width, m_Parameters.Height, (DXGI_FORMAT)FormatToDXGIFormat((SurfaceFormat)m_Parameters.BackBufferFormat), 0)));

    // Set Back buffer
    m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&m_BackBuffer));

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    // This should always be non SRGB? We tone map manually?
    rtvDesc.Format = (DXGI_FORMAT)FormatToDXGIFormat((SurfaceFormat)m_Parameters.BackBufferFormat);
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    assert(SUCCEEDED(m_Device->CreateRenderTargetView(m_BackBuffer, &rtvDesc, &m_RenderTarget)));
    CreateDefaultDetphTarget();

    //--Set defaults on Immediate context--
    m_DeviceContexts[0]->OMSetRenderTargets(1, &m_RenderTarget, m_DepthTarget);

    //--Update viewport and scissor--
    ViewPort vp;
    vp.Width = (float)m_Parameters.Width;
    vp.Height = (float)m_Parameters.Height;
    vp.X = 0;
    vp.Y = 0;
    vp.MinDepth = 0;
    vp.MaxDepth = 1.0f;

    BindViewports(&vp, 1);

    //--Set Default scissor, due to dx12--
    Rect<float> sr = Rect<float>(0, 0, (float)m_Parameters.Width, (float)m_Parameters.Height);
    BindScissorRects(&sr, 1);
}


void GraphicsDevice::FlushGPU()
{
    // Does Nothing.
}

void GraphicsDevice::ShutDown()
{
    //--Release Blit--
    DestroyPipeline(m_BlitPipeline);

    //--Release Graphcis Objects--
    m_Buffers.Clear();
    m_Textures.Clear();
    m_Pipelines.Clear();
    m_Shaders.Clear();

    //--Release State Objects--
    m_RasterMap.Clear();
    m_DepthMap.Clear();
    m_BlendMap.Clear();
    m_InputMap.Clear();
    m_SamplerMap.Clear();

    for (size_t i = 0; i < COMMANDLIST_COUNT; i++)
    {
        if (m_DeviceContexts[i] != nullptr)
        {
            m_DeviceContexts[i]->Release();
        }

        if (m_CommandLists[i] != nullptr)
        {
            m_CommandLists[i]->Release();
        }
    }

    //--Clear Core Objects--
    if (m_BackBuffer) { m_BackBuffer->Release(); }
    if (m_DepthTarget) { m_DepthTarget->Release(); }
    if (m_RenderTarget) { m_RenderTarget->Release(); }
    if (m_Factory) { m_Factory->Release(); }
    if (m_SwapChain) { m_SwapChain->Release(); }

    if (m_Debug) { ReportLiveObjects(); m_Debug->Release(); }
    if (m_Device) { m_Device->Release(); }
}

BufferHandle GraphicsDevice::CreateBuffer(const BufferDesc* pDesc, const Byte* data)
{
    assert(m_Buffers.IsFull() == false && "Memory limit reached");

    ID3D11Buffer* buffer = nullptr;
    ID3D11ShaderResourceView* viewBuffer = nullptr;
    ID3D11UnorderedAccessView* unorderedBuffer = nullptr;

    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = ConvertBindFlags(pDesc->Bind);
    desc.ByteWidth = pDesc->ByteWidth;
    desc.Usage = UsageTable[(Uint32)pDesc->Usage];
    desc.CPUAccessFlags = CpuFlagTable[(Uint32)pDesc->CpuFlags];
    desc.StructureByteStride = pDesc->ByteStride;
    desc.MiscFlags = ConvertMiscFlags(pDesc->MiscFlags);

    if (data)
    {
        D3D11_SUBRESOURCE_DATA initData = { 0 };
        initData.pSysMem = data;
        assert(SUCCEEDED(m_Device->CreateBuffer(&desc, &initData, &buffer)));
    }
    else
    {
        assert(SUCCEEDED(m_Device->CreateBuffer(&desc, nullptr, &buffer)));
    }

    // Create various views.
    if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
        if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
        {
            // Raw Buffer, d3d11.2 supports R32, d3d12 supports more now...
            viewDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
            viewDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
            viewDesc.BufferEx.FirstElement = 0;
            viewDesc.BufferEx.NumElements = desc.ByteWidth / 4;
        }
        else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
        {
            viewDesc.Format = DXGI_FORMAT_UNKNOWN; // MSDN says this!
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
            viewDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
            viewDesc.BufferEx.FirstElement = 0;
            viewDesc.BufferEx.NumElements = desc.ByteWidth / desc.StructureByteStride;
        }
        else // Standard Shader resource view
        {
            viewDesc.Format = (DXGI_FORMAT)FormatToDXGIFormat(pDesc->Format);
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            viewDesc.Buffer.FirstElement = 0;
            viewDesc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
        }
        assert(SUCCEEDED(m_Device->CreateShaderResourceView(buffer, &viewDesc, &viewBuffer)));
    }

    if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc = {};
        viewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        viewDesc.Buffer.FirstElement = 0;

        if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
        {
            // Only supports R32, thus 4 Bytes per block
            viewDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            viewDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
            viewDesc.Buffer.NumElements = desc.ByteWidth / 4;
        }
        else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
        {
            // no format as its a custom struct
            viewDesc.Format = DXGI_FORMAT_UNKNOWN;
            viewDesc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
        }
        else
        {
            // Standard UAV using a format 
            viewDesc.Format = (DXGI_FORMAT)FormatToDXGIFormat(pDesc->Format);
            viewDesc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
        }

        assert(SUCCEEDED(m_Device->CreateUnorderedAccessView(buffer, &viewDesc, &unorderedBuffer)));
    }

    // Allocate resource
    Buffer* pBuffer;
    BufferHandle handle = m_Buffers.Allocate(pBuffer);

    pBuffer->m_Resource = buffer;
    pBuffer->m_SRV = viewBuffer;
    pBuffer->m_UAV = unorderedBuffer;
    pBuffer->m_Desc = *pDesc;

    return handle;
}

TextureHandle GraphicsDevice::CreateTexture(const TextureDesc* pDesc, const Byte* data)
{
    assert(m_Textures.IsFull() == false && "Memory limit reached");

    HRESULT hr = S_OK;
    ID3D11Resource* texture = nullptr;
    ID3D11ShaderResourceView* viewBuffer = nullptr;
    ID3D11UnorderedAccessView* unorderedBuffer = nullptr;
    ID3D11RenderTargetView* renderTarget = nullptr;
    ID3D11DepthStencilView* depthTarget = nullptr;

    // Need enough Subresource as there are arrays * mips
    D3D11_SUBRESOURCE_DATA* subData = nullptr;

    if (data)
    {
        subData = new D3D11_SUBRESOURCE_DATA[(size_t)pDesc->MipLevels * pDesc->ArraySize];
        Uint32 src = 0;
        Uint32 end = pDesc->ByteCount;

        int index = 0;
        for (Uint32 i = 0; i < pDesc->ArraySize; ++i)
        {
            Uint32 w = pDesc->Width;
            Uint32 h = pDesc->Height;
            Uint32 d = pDesc->Depth;

            for (Uint32 j = 0; j < pDesc->MipLevels; ++j)
            {
                Uint32 pitch = CalculatePitchSize(pDesc->Format, w);
                subData[index].pSysMem = data + src;
                subData[index].SysMemPitch = pitch;
                subData[index].SysMemSlicePitch = pitch * h;
                ++index;

                // 3D mip is a cube i.e w * h * d Bytes
                src += pitch * h * d;
                if (src > end)
                {
                    delete[] subData;
                    return TextureHandle();
                }

                // Advance to next mip
                w = w >> 1;
                h = h >> 1;
                d = d >> 1;
                if (w == 0) { w = 1; }
                if (h == 0) { h = 1; }
                if (d == 0) { d = 1; }
            }
        }
        assert(index != 0);
    }

    if (pDesc->Type == TextureType::Texture1D)
    {
        D3D11_TEXTURE1D_DESC desc = {};
        desc.Width = pDesc->Width;
        desc.MipLevels = (pDesc->MipLevels > 1) ? pDesc->MipLevels : 1;
        desc.ArraySize = (pDesc->ArraySize > 1) ? pDesc->ArraySize : 1;
        desc.Format = (DXGI_FORMAT)FormatToDXGIFormat(pDesc->Format);
        desc.BindFlags = ConvertBindFlags(pDesc->BindFlags);
        desc.MiscFlags = ConvertMiscFlags(pDesc->MiscFlags);
        desc.Usage = UsageTable[(Uint32)pDesc->Usage];
        desc.CPUAccessFlags = CpuFlagTable[(Uint32)pDesc->CPUAccessFlags];

        hr = m_Device->CreateTexture1D(&desc, subData, (ID3D11Texture1D**)&texture);
    }
    else if (pDesc->Type == TextureType::Texture2D || pDesc->Type == TextureType::TextureCube)
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = pDesc->Width;
        desc.Height = pDesc->Height;
        desc.MipLevels = (pDesc->MipLevels > 1) ? pDesc->MipLevels : 1;
        desc.ArraySize = (pDesc->ArraySize > 1) ? pDesc->ArraySize : 1;
        desc.Format = (DXGI_FORMAT)FormatToDXGIFormat(pDesc->Format);
        desc.BindFlags = ConvertBindFlags(pDesc->BindFlags);
        desc.MiscFlags = ConvertMiscFlags(pDesc->MiscFlags);
        desc.Usage = UsageTable[(Uint32)pDesc->Usage];
        desc.CPUAccessFlags = CpuFlagTable[(Uint32)pDesc->CPUAccessFlags];
        desc.SampleDesc.Count = pDesc->SampleDesc.Count;
        desc.SampleDesc.Quality = pDesc->SampleDesc.Quality;

        hr = m_Device->CreateTexture2D(&desc, subData, (ID3D11Texture2D**)&texture);
    }
    else if (pDesc->Type == TextureType::Texture3D)
    {
        D3D11_TEXTURE3D_DESC desc = {};
        desc.Width = pDesc->Width;
        desc.Height = pDesc->Height;
        desc.Depth = pDesc->Depth;
        desc.MipLevels = (pDesc->MipLevels > 1) ? pDesc->MipLevels : 1;
        desc.Format = (DXGI_FORMAT)FormatToDXGIFormat(pDesc->Format);
        desc.BindFlags = ConvertBindFlags(pDesc->BindFlags);
        desc.MiscFlags = ConvertMiscFlags(pDesc->MiscFlags);
        desc.Usage = UsageTable[(Uint32)pDesc->Usage];
        desc.CPUAccessFlags = CpuFlagTable[(Uint32)pDesc->CPUAccessFlags];

        hr = m_Device->CreateTexture3D(&desc, subData, (ID3D11Texture3D**)&texture);
    }

    delete[] subData;
    assert(SUCCEEDED(hr) && "D3DDevice Creating Texture Failed");

    // Bind Resource Views

    if (pDesc->BindFlags & (Uint32)BindFlag::ShaderResource)
    {
        viewBuffer = CreateShaderResourceView(pDesc, texture);
    }

    if (pDesc->BindFlags & (Uint32)BindFlag::RenderTarget)
    {
        renderTarget = CreateRenderTargetView(pDesc, texture);
    }

    if (pDesc->BindFlags & (Uint32)BindFlag::DepthStencil)
    {
        depthTarget = CreateDepthStencilView(pDesc, texture);
    }

    if (pDesc->BindFlags & (Uint32)BindFlag::UnorderedAccess)
    {
        unorderedBuffer = CreateUnorderedAccessView(pDesc, texture);
    }

    // Allocate internal resource

    GraphicsTexture* pTexture = nullptr;
    TextureHandle handle = m_Textures.Allocate(pTexture);
    pTexture->m_Resource = texture;
    pTexture->m_SRV = viewBuffer;       // Nullptr if non created
    pTexture->m_UAV = unorderedBuffer;  // Nullptr if non created
    pTexture->m_RTV = renderTarget;
    pTexture->m_DSV = depthTarget;
    pTexture->m_Desc = *pDesc;           // Copy the desc

    return handle;
}

ShaderHandle GraphicsDevice::CreateShader(ShaderType stage, Byte* pByteCode, Uint32 length)
{
    HRESULT hr = S_OK;
    ID3D11Resource* shader;
    switch (stage)
    {
    case ShaderType::VS:
        hr = m_Device->CreateVertexShader(pByteCode, length, nullptr, (ID3D11VertexShader**)&shader);
        break;
    case ShaderType::HS:
        hr = m_Device->CreateHullShader(pByteCode, length, nullptr, (ID3D11HullShader**)&shader);
        break;
    case ShaderType::DS:
        hr = m_Device->CreateDomainShader(pByteCode, length, nullptr, (ID3D11DomainShader**)&shader);
        break;
    case ShaderType::GS:
        hr = m_Device->CreateVertexShader(pByteCode, length, nullptr, (ID3D11VertexShader**)&shader);
        break;
    case ShaderType::PS:
        hr = m_Device->CreatePixelShader(pByteCode, length, nullptr, (ID3D11PixelShader**)&shader);
        break;
    case ShaderType::CS:
        hr = m_Device->CreateComputeShader(pByteCode, length, nullptr, (ID3D11ComputeShader**)&shader);
        break;
    default:
        return ShaderHandle();
        break;
    }

    if (FAILED(hr))
    {
        return ShaderHandle();
    }

    GraphicsShader* pShader = nullptr;
    ShaderHandle handle = m_Shaders.Allocate(pShader);
    pShader->m_Shader = shader;
    pShader->m_Length = length;
    pShader->m_ByteCode = new Byte[length];
    memcpy(pShader->m_ByteCode, pByteCode, length);
    return handle;
}

SamplerHandle GraphicsDevice::CreateSamplerState(const SamplerDesc* pDesc)
{
    Uint32 hash = Hash32::ComputeHash((Byte*)&pDesc, sizeof(SamplerDesc));
    SamplerState* pSampler = nullptr;
    SamplerHandle handle = m_SamplerMap.FindByHash(hash, &pSampler);

    // Create New if non exists.
    if (pSampler == nullptr)
    {
        ID3D11SamplerState* sampler;
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = FilterTable[(Uint32)pDesc->Filter];
        desc.AddressU = WrapModeTable[(Uint32)pDesc->AddressU];
        desc.AddressV = WrapModeTable[(Uint32)pDesc->AddressV];
        desc.AddressW = WrapModeTable[(Uint32)pDesc->AddressW];
        desc.MipLODBias = pDesc->MipLodBias;
        desc.MaxAnisotropy = pDesc->MaxAnisotropy;
        desc.ComparisonFunc = ComparisonTable[(Uint32)pDesc->ComparisonOp];
        desc.BorderColor[0] = pDesc->BorderColor[0];
        desc.BorderColor[1] = pDesc->BorderColor[1];
        desc.BorderColor[2] = pDesc->BorderColor[2];
        desc.BorderColor[3] = pDesc->BorderColor[3];
        desc.MinLOD = pDesc->MinLod;
        desc.MaxLOD = pDesc->MaxLod;

        assert(SUCCEEDED(m_Device->CreateSamplerState(&desc, &sampler)));
        handle = m_SamplerMap.Allocate(hash, &pSampler);
        pSampler->m_Sampler = sampler;
    }

    return handle;
}

PipelineHandle GraphicsDevice::CreatePipeline(const PipelineDesc* pDesc)
{
    PipelineState* pPipeline = nullptr;
    PipelineHandle handle = m_Pipelines.Allocate(pPipeline);
    assert(pPipeline != nullptr);

    //--SHADERS--
    pPipeline->m_VertexShader = pDesc->VertexShader;
    pPipeline->m_PixelShader = pDesc->PixelShader;
    pPipeline->m_GeometaryShader = pDesc->GeometryShader;
    pPipeline->m_DomainShader = pDesc->DomainShader;
    pPipeline->m_HullShader = pDesc->HullShader;

    //--STATES--

    pPipeline->m_BlendState = GetBlendState(pDesc->BlendState);
    pPipeline->m_RasterState = GetRasterState(pDesc->RasterState);
    pPipeline->m_DepthStencilState = GetDepthState(pDesc->DepthState);
    pPipeline->m_InputLayout = GetInputLayout(pDesc->InputLayout, pDesc->VertexShader);
    pPipeline->m_TopologyType = pDesc->Topology;

    return handle;
}


BufferHandle GraphicsDevice::CreateVertexBuffer(Uint32 vertexCount, Uint32 stride, BufferUsage usage, const Byte* data)
{
    BufferDesc desc = {};
    desc.Bind = (Uint32)BindFlag::VertexBuffer;
    desc.ByteStride = stride;
    desc.ByteWidth = vertexCount * stride;
    desc.Usage = usage;
    desc.MiscFlags = 0;

    // Easier to just match usage, if they want immutable GPU they probably want Immutable CPU so on.
    switch (usage)
    {
    case BufferUsage::Default:
    case BufferUsage::Immutable:
        desc.CpuFlags = CpuAccess::Immutable;
        break;
    case BufferUsage::Dynamic:
        desc.CpuFlags = CpuAccess::Write;
        break;
    case BufferUsage::Staging:
        desc.CpuFlags = CpuAccess::ReadWrite;
        break;
    default:
        desc.CpuFlags = CpuAccess::Immutable;
        break;
    }

    return CreateBuffer(&desc, data);
}

BufferHandle GraphicsDevice::CreateIndexBuffer(Uint32 indexCount, IndexFormat format, BufferUsage usage, const Byte* data)
{
    BufferDesc desc = {};
    desc.Bind = (Uint32)BindFlag::IndexBuffer;
    desc.ByteStride = (format == IndexFormat::I16) ? sizeof(Uint16) : sizeof(Uint32);
    desc.ByteWidth = indexCount * desc.ByteStride;
    desc.Usage = usage;
    desc.MiscFlags = 0;
    desc.Format = (format == IndexFormat::I16) ? SurfaceFormat::R16_Uint : SurfaceFormat::R32_Uint;

    // Easier to just match usage, if they want immutable GPU they probably want Immutable CPU so on.
    switch (usage)
    {
    case BufferUsage::Default:
    case BufferUsage::Immutable:
        desc.CpuFlags = CpuAccess::Immutable;
        break;
    case BufferUsage::Dynamic:
        desc.CpuFlags = CpuAccess::Write;
        break;
    case BufferUsage::Staging:
        desc.CpuFlags = CpuAccess::ReadWrite;
        break;
    default:
        desc.CpuFlags = CpuAccess::Immutable;
        break;
    }

    return CreateBuffer(&desc, data);
}

BufferHandle GraphicsDevice::CreateConstantBuffer(Uint32 byteWidth, const Byte* data)
{
    BufferDesc desc = {};
    desc.Bind = (Uint32)BindFlag::ConstantBuffer;
    desc.ByteStride = 0;
    desc.ByteWidth = byteWidth;
    desc.Usage = BufferUsage::Dynamic;
    desc.MiscFlags = 0;
    desc.CpuFlags = CpuAccess::Write;

    return CreateBuffer(&desc, data);
}


TextureHandle GraphicsDevice::CreateRenderTarget(Uint32 width, Uint32 height, RenderFormat renderFormat, Uint32 sampleCount, Uint32 sampleQuality)
{
    TextureDesc desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Depth = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = (SurfaceFormat)renderFormat;
    desc.Usage = BufferUsage::Default;
    desc.CPUAccessFlags = CpuAccess::Immutable;
    desc.MiscFlags = 0;
    desc.BindFlags = (Uint32)BindFlag::RenderTarget | (Uint32)BindFlag::ShaderResource;
    desc.SampleDesc.Count = sampleCount;
    desc.SampleDesc.Quality = sampleQuality;

    return CreateTexture(&desc, nullptr);
}

TextureHandle GraphicsDevice::CreateDepthTarget(Uint32 width, Uint32 height, DepthFormat format, Uint32 sampleCount, Uint32 sampleQuality)
{
    TextureDesc desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Depth = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = (SurfaceFormat)format;
    desc.Usage = BufferUsage::Default;
    desc.BindFlags = (Uint32)BindFlag::DepthStencil | (Uint32)BindFlag::ShaderResource;
    desc.CPUAccessFlags = CpuAccess::Immutable;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = sampleCount;
    desc.SampleDesc.Quality = sampleQuality;

    return CreateTexture(&desc, nullptr);
}

void GraphicsDevice::UpdateBuffer(const BufferHandle buffer, const Byte* data, Uint32 byteCount, CommandList cmd)
{
    Buffer* pBuffer = m_Buffers[buffer];

    if (pBuffer == nullptr)
    {
        LogError("Buffer is NULL.");
        return;
    }

    if (pBuffer->m_Desc.Usage == BufferUsage::Immutable)
    {
        LogError("Buffer is Immutable");
        return;
    }

    if (byteCount > pBuffer->m_Desc.ByteWidth)
    {
        LogError("Buffer update size too large");
        return;
    }

    if (pBuffer->m_Desc.Usage == BufferUsage::Dynamic)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if (SUCCEEDED(m_DeviceContexts[cmd]->Map((ID3D11Resource*)pBuffer->m_Resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
        {
            memcpy(mappedResource.pData, data, byteCount);
            m_DeviceContexts[cmd]->Unmap((ID3D11Resource*)pBuffer->m_Resource, 0);
        }
    }
    else if (pBuffer->m_Desc.Bind & D3D11_BIND_CONSTANT_BUFFER || byteCount < 0)
    {
        // Constant buffers cant partially update i.e no box!
        m_DeviceContexts[cmd]->UpdateSubresource((ID3D11Resource*)pBuffer->m_Resource, 0, nullptr, data, 0, 0);
    }
    else
    {
        // Update catch all, uses default box offset, Default and stating catch
        D3D11_BOX box;
        box.left = 0; box.right = byteCount;
        box.top = 0; box.bottom = 1;
        box.front = 0; box.back = 1;
        m_DeviceContexts[cmd]->UpdateSubresource((ID3D11Resource*)pBuffer->m_Resource, 0, &box, data, 0, 0);
    }
}

void GraphicsDevice::UpdateTexture(const TextureHandle texture, const Byte* data, Uint32 byteCount, CommandList cmd)
{
    GraphicsTexture* pTexture = m_Textures[texture];
    if (pTexture && pTexture->m_Desc.Usage != BufferUsage::Immutable && byteCount > 0)
    {
        if (byteCount > pTexture->m_Desc.ByteCount)
        {
            LogError("Texture update size too large");
            return;
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if (SUCCEEDED(m_DeviceContexts[cmd]->Map((ID3D11Resource*)pTexture->m_Resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
        {
            // Set the data and unmap
            Byte* sptr = (Byte*)data;
            Byte* dptr = (Byte*)mappedResource.pData;
            Uint32 rowCount = mappedResource.DepthPitch / mappedResource.RowPitch;
            Uint32 sliceCount = byteCount / mappedResource.DepthPitch;

            size_t msize = std::min<size_t>(pTexture->m_Desc.Pitch, mappedResource.RowPitch);
            for (size_t d = 0; d < pTexture->m_Desc.Depth; d++)
            {
                for (size_t h = 0; h < rowCount; h++)
                {
                    memcpy_s(dptr, pTexture->m_Desc.Pitch, sptr, msize);
                    dptr += mappedResource.RowPitch;
                    sptr += pTexture->m_Desc.Pitch;
                }
            }
            m_DeviceContexts[cmd]->Unmap((ID3D11Resource*)pTexture->m_Resource, 0);
        }
        else
        {
            LogError("Texture Update Failed to Map Resource.");
        }
    }
}

void GraphicsDevice::CopyTextureResource(const TextureHandle dest, const TextureHandle src, CommandList cmd)
{
    GraphicsTexture* pDest = m_Textures[dest];
    GraphicsTexture* pSrc = m_Textures[src];

    if (pDest && pSrc)
    {
        m_DeviceContexts[cmd]->CopyResource(pDest->m_Resource, pSrc->m_Resource);
    }
}

void GraphicsDevice::CopyBufferResource(const BufferHandle dest, const BufferHandle src, CommandList cmd)
{
    Buffer* pDest = m_Buffers[dest];
    Buffer* pSrc = m_Buffers[dest];

    if (pDest && pSrc)
    {
        m_DeviceContexts[cmd]->CopyResource(pDest->m_Resource, pSrc->m_Resource);
    }
}

void GraphicsDevice::GetTextureData(const TextureHandle texture, Byte* data, Uint32 byteCount, CommandList cmd)
{
    GraphicsTexture* pTexture = m_Textures[texture];
    TextureDesc* pDesc = &pTexture->m_Desc;
    HRESULT hr = S_OK;

    // Create staging texture, should pool ?
    ID3D11Resource* staging = nullptr;
    if (pDesc->Type == TextureType::Texture1D)
    {
        D3D11_TEXTURE1D_DESC desc = {};
        desc.Width = pDesc->Width;
        desc.MipLevels = (pDesc->MipLevels > 1) ? pDesc->MipLevels : 1;
        desc.ArraySize = (pDesc->ArraySize > 1) ? pDesc->ArraySize : 1;
        desc.Format = (DXGI_FORMAT)FormatToDXGIFormat(pDesc->Format);
        desc.BindFlags = 0;
        desc.MiscFlags = 0;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        hr = m_Device->CreateTexture1D(&desc, nullptr, (ID3D11Texture1D**)&staging);
    }
    else if (pDesc->Type == TextureType::Texture2D || pDesc->Type == TextureType::TextureCube)
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = pDesc->Width;
        desc.Height = pDesc->Height;
        desc.MipLevels = (pDesc->MipLevels > 1) ? pDesc->MipLevels : 1;
        desc.ArraySize = (pDesc->ArraySize > 1) ? pDesc->ArraySize : 1;
        desc.Format = (DXGI_FORMAT)FormatToDXGIFormat(pDesc->Format);
        desc.BindFlags = 0;
        desc.MiscFlags = 0;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.SampleDesc.Count = pDesc->SampleDesc.Count;
        desc.SampleDesc.Quality = pDesc->SampleDesc.Quality;

        hr = m_Device->CreateTexture2D(&desc, nullptr, (ID3D11Texture2D**)&staging);
    }
    else if (pDesc->Type == TextureType::Texture3D)
    {
        D3D11_TEXTURE3D_DESC desc = {};
        desc.Width = pDesc->Width;
        desc.Height = pDesc->Height;
        desc.Depth = pDesc->Depth;
        desc.MipLevels = (pDesc->MipLevels > 1) ? pDesc->MipLevels : 1;
        desc.Format = (DXGI_FORMAT)FormatToDXGIFormat(pDesc->Format);
        desc.BindFlags = 0;
        desc.MiscFlags = 0;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        hr = m_Device->CreateTexture3D(&desc, nullptr, (ID3D11Texture3D**)&staging);
    }

    assert(SUCCEEDED(hr) && "Failed To Create Staging Texture.");

    // Copy data too staging texture
    m_DeviceContexts[0]->CopyResource(staging, pTexture->m_Resource);

    // Copy data to users buffer
    D3D11_MAPPED_SUBRESOURCE stream;
    hr = m_DeviceContexts[cmd]->Map(staging, 0, D3D11_MAP_READ, 0, &stream);

    if (FAILED(hr))
    {
        staging->Release();
        assert(0 && "Failed To Map Staging Texturer.");
    }

    // Grab the data and unmap
    Byte* sptr = (Byte*)stream.pData;
    Byte* dptr = (Byte*)data;
    Uint32 rowCount = stream.DepthPitch / stream.RowPitch;
    Uint32 sliceCount = byteCount / stream.DepthPitch;

    size_t msize = std::min<size_t>(pDesc->Pitch, stream.RowPitch);
    for (size_t d = 0; d < pDesc->Depth; d++)
    {
        for (size_t h = 0; h < rowCount; h++)
        {
            memcpy_s(dptr, pDesc->Pitch, sptr, msize);
            sptr += stream.RowPitch;
            dptr += pDesc->Pitch;
        }
    }

    m_DeviceContexts[cmd]->Unmap(staging, 0);

    staging->Release();
}

void GraphicsDevice::PresentBegin()
{
    BindDefaultViewPortAndScissor();

    //--Clear the Targets--
    float color[4] = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a };
    m_DeviceContexts[0]->ClearRenderTargetView(m_RenderTarget, color);
    m_DeviceContexts[0]->ClearDepthStencilView(m_DepthTarget, D3D11_CLEAR_DEPTH, m_ClearDepth, 0);

    //--Set Default Targets, flip model requires reset--
    m_DeviceContexts[0]->OMSetRenderTargets(1, &m_RenderTarget, m_DepthTarget);
}

void GraphicsDevice::PresentEnd()
{
    // CommandList 0 is the immediate context!
    for (CommandList cmd = 1; cmd < (Int16)m_CommandListCount; ++cmd)
    {
        m_DeviceContexts[cmd]->FinishCommandList(false, &m_CommandLists[cmd]);
        m_DeviceContexts[0]->ExecuteCommandList(m_CommandLists[cmd], false);
        m_CommandLists[cmd]->Release();
    }

    if (m_Parameters.Vsync.m_Enabled)
    {
        m_SwapChain->Present(m_Parameters.Vsync.m_Value, 0);
    }
    else
    {
        m_SwapChain->Present(0, 0);
    }

    // Immediate context clear
    m_DeviceContexts[0]->ClearState();

    std::memset(m_PrevVertexShader, 0, sizeof(m_PrevVertexShader));
    std::memset(m_PrevPixelShader, 0, sizeof(m_PrevPixelShader));
    std::memset(m_PrevHullShader, 0, sizeof(m_PrevHullShader));
    std::memset(m_PrevDomainShader, 0, sizeof(m_PrevDomainShader));
    std::memset(m_PrevGeometryShader, 0, sizeof(m_PrevGeometryShader));
    std::memset(m_PrevComputeShader, 0, sizeof(m_PrevComputeShader));
    std::memset(m_PrevBlendState, 0, sizeof(m_PrevBlendState));
    std::memset(m_PrevRasterState, 0, sizeof(m_PrevRasterState));
    std::memset(m_PrevDepthState, 0, sizeof(m_PrevDepthState));
    std::memset(m_PrevInputLayout, 0, sizeof(m_PrevInputLayout));
    std::memset(m_PrevTopology, 0, sizeof(m_PrevTopology));
    std::memset(m_PrevSamplerState, 0, sizeof(m_PrevSamplerState));
    m_FrameCount++;
}

void GraphicsDevice::Draw(Uint32 vertexCount, Uint32 startVertex, CommandList cmd)
{
    m_DeviceContexts[cmd]->Draw(vertexCount, startVertex);
}

void GraphicsDevice::DrawIndexed(Uint32 startIndex, Uint32 indexCount, Uint32 startVertex, CommandList cmd)
{
    m_DeviceContexts[cmd]->DrawIndexed(indexCount, startIndex, startVertex);
}

void GraphicsDevice::BindRenderTarget(RenderHandle renderTarget, DepthHandle depthTarget, CommandList cmd)
{
    // Set Defaults
    ID3D11RenderTargetView* rtv = m_RenderTarget;
    ID3D11DepthStencilView* dsv = m_DepthTarget;

    // Check for valid Render Target handle
    if (renderTarget.IsValid())
    {
        GraphicsTexture* texture = m_Textures[renderTarget];
        assert(texture != nullptr);
        rtv = texture->m_RTV;
    }

    if (depthTarget.IsValid())
    {
        GraphicsTexture* texture = m_Textures[depthTarget];
        assert(texture != nullptr);
        dsv = texture->m_DSV;
    }

    // Unbinds any bound ones?
    m_DeviceContexts[cmd]->OMSetRenderTargets(1, &rtv, dsv);
}

void GraphicsDevice::BindRenderTarget(const RenderTargetGroup* renderTargetGroup, CommandList cmd)
{
    if (renderTargetGroup == nullptr)
    {
        m_DeviceContexts[cmd]->OMSetRenderTargets(1, &m_RenderTarget, m_DepthTarget);
        return;
    }

    // Set Defaults
    ID3D11RenderTargetView* rtv[8] = { nullptr };
    ID3D11DepthStencilView* dsv = m_DepthTarget;
    float color[4] = { 0,0,0,0 };

    // Check for valid Render Target handle
    for (Uint32 i = 0; i < renderTargetGroup->Count(); i++)
    {
        const RenderAttachment* attachment = &renderTargetGroup->m_RenderTargets[i];
        GraphicsTexture* texture = m_Textures[attachment->m_Texture];
        assert(texture != nullptr);

        rtv[i] = (ID3D11RenderTargetView*)texture->m_RTV;

        if (attachment->m_Operation == TargetOperation::Clear)
        {
            color[0] = attachment->m_ClearColor.r; color[1] = attachment->m_ClearColor.g;
            color[2] = attachment->m_ClearColor.b; color[3] = attachment->m_ClearColor.a;

            m_DeviceContexts[cmd]->ClearRenderTargetView(texture->m_RTV, color);
        }
    }

    // Check for valid Depth handle
    GraphicsTexture* texture = m_Textures[renderTargetGroup->m_DepthAttachment.m_DepthTarget];
    if (texture != nullptr)
    {
        dsv = (ID3D11DepthStencilView*)texture->m_DSV;

        if (renderTargetGroup->m_DepthAttachment.m_Operation == TargetOperation::Clear)
        {
            UINT flags = D3D11_CLEAR_DEPTH;
            if (IsDepthStencil(texture->m_Desc.Format))
            {
                flags |= D3D11_CLEAR_STENCIL;
            }

            m_DeviceContexts[cmd]->ClearDepthStencilView(texture->m_DSV, flags, renderTargetGroup->m_DepthAttachment.m_Depth, (Uint8)renderTargetGroup->m_DepthAttachment.m_Stencil);
        }
    }

    m_DeviceContexts[cmd]->OMSetRenderTargets(renderTargetGroup->Count(), rtv, dsv);
}

void GraphicsDevice::BindVertexBuffer(BufferHandle buffer, Uint32 offset, CommandList cmd)
{
    Buffer* pBuffer = m_Buffers[buffer];
    assert(pBuffer != nullptr);

    UINT stride = pBuffer->m_Desc.ByteStride;
    UINT Offset = (UINT)offset;

    m_DeviceContexts[cmd]->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&pBuffer->m_Resource, &stride, &Offset);
}

void GraphicsDevice::BindVertexBuffers(BufferHandle* buffers, Uint32* offsets, Uint32 count, CommandList cmd)
{
    ID3D11Buffer* bufferList[8] = { nullptr };
    UINT strides[8] = { 0 };
    UINT offset[8] = { 0 };

    for (Uint32 i = 0; i < count; ++i)
    {
        Buffer* pBuffer = m_Buffers[buffers[i]];
        if (pBuffer == nullptr)
        {
            Window::ShowMessageBox("VertexBuffer Invalid");
            return;
        }

        bufferList[i] = (ID3D11Buffer*)pBuffer->m_Resource;
        strides[i] = pBuffer->m_Desc.ByteStride;
        offsets[i] = offsets[i];
    }

    m_DeviceContexts[cmd]->IASetVertexBuffers(0, 1, bufferList, strides, offset);
}

void GraphicsDevice::BindIndexBuffer(BufferHandle buffer, Uint32 offset, CommandList cmd)
{
    Buffer* pBuffer = m_Buffers[buffer];
    assert(pBuffer != nullptr);

    m_DeviceContexts[cmd]->IASetIndexBuffer((ID3D11Buffer*)pBuffer->m_Resource, (DXGI_FORMAT)FormatToDXGIFormat(pBuffer->m_Desc.Format), offset);
}

void GraphicsDevice::BindConstantBuffer(BufferHandle buffer, Uint32 slot, CommandList cmd)
{
    Buffer* pBuffer = m_Buffers[buffer];
    assert(pBuffer != nullptr);

    m_DeviceContexts[cmd]->VSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&pBuffer->m_Resource);
    m_DeviceContexts[cmd]->HSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&pBuffer->m_Resource);
    m_DeviceContexts[cmd]->DSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&pBuffer->m_Resource);
    m_DeviceContexts[cmd]->GSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&pBuffer->m_Resource);
    m_DeviceContexts[cmd]->PSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&pBuffer->m_Resource);
    m_DeviceContexts[cmd]->CSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&pBuffer->m_Resource);
}

void GraphicsDevice::BindTexture(TextureHandle texture, Uint32 slot, CommandList cmd)
{
    GraphicsTexture* pTexture = m_Textures[texture];
    assert(pTexture != nullptr);

    m_DeviceContexts[cmd]->VSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&pTexture->m_SRV);
    m_DeviceContexts[cmd]->HSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&pTexture->m_SRV);
    m_DeviceContexts[cmd]->DSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&pTexture->m_SRV);
    m_DeviceContexts[cmd]->GSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&pTexture->m_SRV);
    m_DeviceContexts[cmd]->PSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&pTexture->m_SRV);
    m_DeviceContexts[cmd]->CSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&pTexture->m_SRV);
}

void GraphicsDevice::BindBufferUAV(BufferHandle handle, ShaderType stage, Uint32 slot, CommandList cmd)
{
    Buffer* buffer = m_Buffers[handle];

    if (buffer != nullptr)
    {
        ID3D11UnorderedAccessView* uav = buffer->m_UAV;

        if (stage == ShaderType::CS)
        {
            m_DeviceContexts[cmd]->CSSetUnorderedAccessViews(slot, 1, &uav, nullptr);
        }

        // I dont support RTV UAV yet
    }
}

void GraphicsDevice::BindTextureUAV(TextureHandle handle, ShaderType stage, Uint32 slot, CommandList cmd)
{
    GraphicsTexture* texture = m_Textures[handle];

    if (texture != nullptr)
    {
        ID3D11UnorderedAccessView* uav = texture->m_UAV;

        if (stage == ShaderType::CS)
        {
            m_DeviceContexts[cmd]->CSSetUnorderedAccessViews(slot, 1, &uav, nullptr);
        }

        // I dont support RTV UAV yet
    }
}

void GraphicsDevice::BindSampler(SamplerHandle sampler, Uint32 slot, CommandList cmd)
{
    SamplerState* pSampler = m_SamplerMap.FindByIndex(sampler.Index);
    assert(pSampler != nullptr);
    int prevIndex = (cmd * 16) + slot;

    if (m_PrevSamplerState[prevIndex] != pSampler->m_Sampler)
    {
        m_PrevSamplerState[prevIndex] = pSampler->m_Sampler;
        m_DeviceContexts[cmd]->VSSetSamplers(slot, 1, &pSampler->m_Sampler);
        m_DeviceContexts[cmd]->HSSetSamplers(slot, 1, &pSampler->m_Sampler);
        m_DeviceContexts[cmd]->DSSetSamplers(slot, 1, &pSampler->m_Sampler);
        m_DeviceContexts[cmd]->GSSetSamplers(slot, 1, &pSampler->m_Sampler);
        m_DeviceContexts[cmd]->PSSetSamplers(slot, 1, &pSampler->m_Sampler);
        m_DeviceContexts[cmd]->CSSetSamplers(slot, 1, &pSampler->m_Sampler);
    }
}

void GraphicsDevice::BindPipelineState(PipelineHandle pipeline, CommandList cmd)
{
    const PipelineState* pPipelineState = GetPiplineObject(pipeline);
    assert(pPipelineState != nullptr);


    if (pPipelineState->m_VertexShader.IsValid())
    {
        ID3D11VertexShader* vs = (ID3D11VertexShader*)m_Shaders[pPipelineState->m_VertexShader]->m_Shader;
        if (vs && vs != m_PrevVertexShader[cmd])
        {
            m_DeviceContexts[cmd]->VSSetShader(vs, nullptr, 0);
            m_PrevVertexShader[cmd] = vs;
        }
    }

    if (pPipelineState->m_PixelShader.IsValid())
    {
        ID3D11PixelShader* ps = (ID3D11PixelShader*)m_Shaders[pPipelineState->m_PixelShader]->m_Shader;
        if (ps && ps != m_PrevPixelShader[cmd])
        {
            m_DeviceContexts[cmd]->PSSetShader(ps, nullptr, 0);
            m_PrevPixelShader[cmd] = ps;
        }
    }

    if (pPipelineState->m_HullShader.IsValid())
    {
        ID3D11HullShader* hs = (ID3D11HullShader*)m_Shaders[pPipelineState->m_HullShader]->m_Shader;
        if (hs && hs != m_PrevHullShader[cmd])
        {
            m_DeviceContexts[cmd]->HSSetShader(hs, nullptr, 0);
            m_PrevHullShader[cmd] = hs;
        }
    }

    if (pPipelineState->m_DomainShader.IsValid())
    {
        ID3D11DomainShader* ds = (ID3D11DomainShader*)m_Shaders[pPipelineState->m_DomainShader]->m_Shader;
        if (ds && ds != m_PrevDomainShader[cmd])
        {
            m_DeviceContexts[cmd]->DSSetShader(ds, nullptr, 0);
            m_PrevDomainShader[cmd] = ds;
        }
    }

    if (pPipelineState->m_GeometaryShader.IsValid())
    {
        ID3D11GeometryShader* gs = (ID3D11GeometryShader*)m_Shaders[pPipelineState->m_GeometaryShader]->m_Shader;
        if (gs && gs != m_PrevGeometryShader[cmd])
        {
            m_DeviceContexts[cmd]->GSSetShader(gs, nullptr, 0);
            m_PrevGeometryShader[cmd] = gs;
        }
    }

    ID3D11BlendState* bs = (ID3D11BlendState*)pPipelineState->m_BlendState;
    if (bs && bs != m_PrevBlendState[cmd])
    {
        m_DeviceContexts[cmd]->OMSetBlendState(bs, pPipelineState->m_BlendFactor, pPipelineState->m_SampleMask);
        m_PrevBlendState[cmd] = bs;
    }

    ID3D11DepthStencilState* depthState = (ID3D11DepthStencilState*)pPipelineState->m_DepthStencilState;
    if (depthState && depthState != m_PrevDepthState[cmd])
    {
        m_DeviceContexts[cmd]->OMSetDepthStencilState(depthState, 0xFF);
        m_PrevDepthState[cmd] = depthState;
    }

    ID3D11RasterizerState* rasterState = (ID3D11RasterizerState*)pPipelineState->m_RasterState;
    if (rasterState && rasterState != m_PrevRasterState[cmd])
    {
        m_DeviceContexts[cmd]->RSSetState(rasterState);
        m_PrevRasterState[cmd] = rasterState;
    }

    ID3D11InputLayout* inputLayout = (ID3D11InputLayout*)pPipelineState->m_InputLayout;
    if (inputLayout && inputLayout != m_PrevInputLayout[cmd])
    {
        m_DeviceContexts[cmd]->IASetInputLayout(inputLayout);
        m_PrevInputLayout[cmd] = inputLayout;
    }

    D3D11_PRIMITIVE_TOPOLOGY topology = TopologyTable[(Uint32)pPipelineState->m_TopologyType];
    if (topology != m_PrevTopology[cmd])
    {
        m_DeviceContexts[cmd]->IASetPrimitiveTopology(topology);
        m_PrevTopology[cmd] = topology;
    }
}

void GraphicsDevice::BindComputeShader(ShaderHandle shader, CommandList cmd)
{
    ID3D11ComputeShader* computeShader = (ID3D11ComputeShader*)m_Shaders[shader]->m_Shader;
    if (computeShader != m_PrevComputeShader[cmd])
    {
        m_DeviceContexts[cmd]->CSSetShader(computeShader, nullptr, 0);
        m_PrevComputeShader[cmd] = computeShader;
    }
}

void GraphicsDevice::BindScissor(Uint32 x, Uint32 y, Uint32 width, Uint32 height, CommandList cmd)
{
    Rect<float> rect = Rect<float>((float)x, (float)y, (float)width, (float)height);
    BindScissorRects(&rect, 1, cmd);
}

void GraphicsDevice::BindScissorRects(const Rect<float>* pRects, Uint32 numRects, CommandList cmd)
{
    D3D11_RECT rects[8];
    for (Uint32 i = 0; i < numRects; i++)
    {
        // My rectangles go up not down like msofts
        rects[i].left = (long)pRects[i].Left();
        rects[i].top = (long)pRects[i].Bottom();
        rects[i].right = (long)pRects[i].Right();
        rects[i].bottom = (long)pRects[i].Top();
    }

    m_DeviceContexts[cmd]->RSSetScissorRects(numRects, rects);
}

void GraphicsDevice::BindViewPort(const ViewPort& viewport, CommandList cmd)
{
    BindViewports(&viewport, 1);
}

void GraphicsDevice::BindViewports(const ViewPort* pViewports, Uint32 count, CommandList cmd)
{
    D3D11_VIEWPORT viewPorts[6];

    for (Uint32 i = 0; i < count; i++)
    {
        viewPorts[i].Width = pViewports[i].Width;
        viewPorts[i].Height = pViewports[i].Height;
        viewPorts[i].TopLeftX = pViewports[i].X;
        viewPorts[i].TopLeftY = pViewports[i].Y;
        viewPorts[i].MinDepth = pViewports[i].MinDepth;
        viewPorts[i].MaxDepth = pViewports[i].MaxDepth;
    }

    m_DeviceContexts[cmd]->RSSetViewports(count, viewPorts);
}

void GraphicsDevice::UnbindResource(Uint32 slot, Uint32 num, CommandList cmd)
{
    m_DeviceContexts[cmd]->PSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)g_NullResources);
    m_DeviceContexts[cmd]->VSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)g_NullResources);
    m_DeviceContexts[cmd]->GSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)g_NullResources);
    m_DeviceContexts[cmd]->HSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)g_NullResources);
    m_DeviceContexts[cmd]->DSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)g_NullResources);
    m_DeviceContexts[cmd]->CSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)g_NullResources);
}

void GraphicsDevice::UnbindUAV(Uint32 slot, Uint32 num, CommandList cmd)
{
    m_DeviceContexts[cmd]->CSSetUnorderedAccessViews(slot, num, (ID3D11UnorderedAccessView**)g_NullResources, 0);
}

void GraphicsDevice::BindDefaultViewPortAndScissor(CommandList cmd)
{
    //--Set Default Viewport and Rect for now--
    D3D11_VIEWPORT view;
    view.Width = (float)m_Parameters.Width;
    view.Height = (float)m_Parameters.Height;
    view.TopLeftX = 0;
    view.TopLeftY = 0;
    view.MinDepth = 0.0f;
    view.MaxDepth = 1.0f;

    D3D11_RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = (long)m_Parameters.Width;
    rect.bottom = (long)m_Parameters.Height;

    m_DeviceContexts[cmd]->RSSetViewports(1, &view);
    m_DeviceContexts[cmd]->RSSetScissorRects(1, &rect);
}

void GraphicsDevice::SetDefaultClearColor(Color color)
{
    m_ClearColor = color;
}

void GraphicsDevice::SetStencilRef(Uint32 ref, CommandList cmd)
{
    // Nothing yet -_-
}

void GraphicsDevice::Dispatch(Uint32 x, Uint32 y, Uint32 z, CommandList cmd)
{
    m_DeviceContexts[cmd]->Dispatch(x, y, z);
}

void GraphicsDevice::ClearRenderTarget(const RenderHandle renderTarget, Color color, CommandList cmd)
{
    ID3D11RenderTargetView* RTV = m_RenderTarget;
    if (renderTarget.IsValid())
    {
        GraphicsTexture* pTexture = m_Textures[renderTarget];
        assert(pTexture && "Render Target Null.");
        RTV = pTexture->m_RTV;
    }

    float RGBA[4] = { color.r, color.g, color.b, color.a };
    m_DeviceContexts[cmd]->ClearRenderTargetView(RTV, RGBA);
}

void GraphicsDevice::ClearDepthTarget(const DepthHandle depthTarget, float depth, Uint32 stencil, CommandList cmd)
{
    ID3D11DepthStencilView* DSV = m_DepthTarget;
    Uint32 flags = D3D11_CLEAR_DEPTH;
    if (depthTarget.IsValid())
    {
        GraphicsTexture* pTexture = m_Textures[depthTarget];
        assert(pTexture && "Depth Target Null.");
        DSV = pTexture->m_DSV;
        flags = IsDepthStencil(pTexture->m_Desc.Format) ? (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL) : D3D11_CLEAR_DEPTH;
    }

    m_DeviceContexts[cmd]->ClearDepthStencilView(DSV, flags, depth, (Uint8)stencil);
}

void GraphicsDevice::DestroyBuffer(BufferHandle handle)
{
    Buffer* pBuffer = m_Buffers[handle];
    if (pBuffer)
    {
        pBuffer->Release();
    }
    m_Buffers.Destroy(handle);
}

void GraphicsDevice::DestroyTexture(BufferHandle handle)
{
    GraphicsTexture* pTexture = m_Textures[handle];
    if (pTexture)
    {
        pTexture->Release();
        m_Textures.Destroy(handle);
    }
}

void GraphicsDevice::DestroyShader(ShaderHandle handle)
{
    GraphicsShader* pShader = m_Shaders[handle];
    if (pShader)
    {
        pShader->Release();
    }
    m_Shaders.Destroy(handle);
}

void GraphicsDevice::DestroyPipeline(PipelineHandle handle)
{
    PipelineState* pPipeline = m_Pipelines[handle];
    if (pPipeline)
    {
        pPipeline->Release();
    }
    m_Pipelines.Destroy(handle);
}


const GraphicsAdapter* GraphicsDevice::GetAdapter()const
{
    return m_Adapter;
}

const GraphicsParameters& GraphicsDevice::GetParameters()const
{
    return m_Parameters;
}

ID3D11Device* GraphicsDevice::GetDevice()const
{
    return m_Device;
}

ID3D11DeviceContext* GraphicsDevice::GetImmediateContext()const
{
    return m_DeviceContexts[0];
}

void GraphicsDevice::CompileShader(const std::string fileName, const char* entry, const char* shaderModel, Byte** byteCode, Uint32& byteLength)
{
    HRESULT hr = S_OK;
    ID3DBlob* pError = nullptr;
    ID3DBlob* blob = nullptr;

#ifdef DEBUG
    m_CompileFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    hr = D3DCompileFromFile(StringUtil::Widen(fileName.c_str()).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, shaderModel, m_CompileFlags, 0, &blob, &pError);

    if (FAILED(hr))
    {
        std::stringstream ss("");

        if (pError != nullptr)
        {
            ss << (char*)pError->GetBufferPointer();
        }
        else
        {
            ss << "Unkown shader error.";
        }

        LogError(ss.str());
        assert(0 && "shader failed to compile");
    }

    if (pError)
    {
        pError->Release();
    }

    //Copy and release the d3d object
    byteLength = (Uint32)blob->GetBufferSize();
    *byteCode = new Byte[byteLength];

    memcpy(*byteCode, blob->GetBufferPointer(), byteLength);
    blob->Release();
}

void GraphicsDevice::ReportLiveObjects()
{
    if (m_Debug)
    {
        m_Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    }
}

const Buffer* GraphicsDevice::GetBuffer(BufferHandle handle)
{
    return m_Buffers[handle];
}

const GraphicsTexture* GraphicsDevice::GetTexture(TextureHandle handle)
{
    return m_Textures[handle];
}

const PipelineState* GraphicsDevice::GetPiplineObject(PipelineHandle handle)
{
    return m_Pipelines[handle];
}

const GraphicsShader* GraphicsDevice::GetShader(ShaderHandle handle)
{
    return m_Shaders[handle];
}

void GraphicsDevice::BlitToBuffer(TextureHandle source, RenderHandle target, PipelineHandle pipelineHandle)
{
    PipelineHandle handle = pipelineHandle.IsValid() ? pipelineHandle : m_BlitPipeline;

    BindPipelineState(handle);
    UnbindResource(0, 1);
    ClearRenderTarget(target, Color::Black);
    BindRenderTarget(target);
    BindTexture(source, 0);
    Draw(3, 0, 0);
}

void GraphicsDevice::BlitToBound(TextureHandle source, PipelineHandle pipelineHandle)
{
    PipelineHandle handle = pipelineHandle.IsValid() ? pipelineHandle : m_BlitPipeline;
    UnbindResource(0, 1);
    BindPipelineState(handle);
    BindTexture(source, 0);
    Draw(3, 0, 0);
}


void GraphicsDevice::CreateDefaultDetphTarget()
{
    if (m_DepthTarget != nullptr)
    {
        m_DepthTarget->Release();
    }

    ID3D11Texture2D* depthTexture = NULL;
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width = (UINT)m_Parameters.Width;
    texDesc.Height = (UINT)m_Parameters.Height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = (DXGI_FORMAT)FormatToDXGIFormat((SurfaceFormat)m_Parameters.DepthStencilFormat);
    texDesc.SampleDesc.Count = m_Parameters.MultiSampleing.Count;
    texDesc.SampleDesc.Quality = m_Parameters.MultiSampleing.Quality;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    assert(SUCCEEDED(m_Device->CreateTexture2D(&texDesc, NULL, &depthTexture)));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = GetResourceViewFormat((SurfaceFormat)m_Parameters.DepthStencilFormat, true);

    if (texDesc.SampleDesc.Count > 1)
    {
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    }
    else
    {
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;
    }

    assert(SUCCEEDED(m_Device->CreateDepthStencilView(depthTexture, &dsvDesc, &m_DepthTarget)));
    depthTexture->Release();

}
ID3D11ShaderResourceView* GraphicsDevice::CreateShaderResourceView(const TextureDesc* pDesc, ID3D11Resource* resource)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = GetResourceViewFormat(pDesc->Format);

    if (pDesc->Type == TextureType::Texture1D)
    {
        if (pDesc->ArraySize > 1)
        {
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.FirstArraySlice = 0;
            desc.Texture1DArray.ArraySize = pDesc->ArraySize;
            desc.Texture1DArray.MostDetailedMip = 0;
            desc.Texture1DArray.MipLevels = pDesc->MipLevels;
        }
        else
        {
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            desc.Texture1D.MostDetailedMip = 0;
            desc.Texture1D.MipLevels = pDesc->MipLevels;
        }
    }
    else if (pDesc->Type == TextureType::Texture2D || pDesc->Type == TextureType::TextureCube)
    {
        if (pDesc->ArraySize > 1)
        {
            if ((pDesc->MiscFlags & (Uint32)ResourceMiscFlags::TexturedCube))
            {
                if (pDesc->ArraySize > 6)
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
                    desc.TextureCubeArray.First2DArrayFace = 0;
                    desc.TextureCubeArray.NumCubes = pDesc->ArraySize / 6;
                    desc.TextureCubeArray.MostDetailedMip = 0;
                    desc.TextureCubeArray.MipLevels = pDesc->MipLevels;
                }
                else
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                    desc.TextureCube.MostDetailedMip = 0;
                    desc.TextureCube.MipLevels = pDesc->MipLevels;
                }
            }
            else
            {
                if (pDesc->SampleDesc.Count > 1)
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
                    desc.Texture2DMSArray.FirstArraySlice = 0;
                    desc.Texture2DMSArray.ArraySize = pDesc->ArraySize;
                }
                else
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                    desc.Texture2DArray.FirstArraySlice = 0;
                    desc.Texture2DArray.ArraySize = pDesc->ArraySize;
                    desc.Texture2DArray.MostDetailedMip = 0;
                    desc.Texture2DArray.MipLevels = pDesc->MipLevels;
                }
            }
        }
        else
        {
            if (pDesc->SampleDesc.Count > 1)
            {
                desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                desc.Texture2D.MostDetailedMip = 0;
                desc.Texture2D.MipLevels = pDesc->MipLevels;
            }
        }
    }
    else if (pDesc->Type == TextureType::Texture3D)
    {
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        desc.Texture3D.MostDetailedMip = 0;
        desc.Texture3D.MipLevels = pDesc->MipLevels;
    }

    ID3D11ShaderResourceView* srv = nullptr;
    assert(SUCCEEDED(m_Device->CreateShaderResourceView(resource, &desc, &srv)));

    return srv;
}

ID3D11RenderTargetView* GraphicsDevice::CreateRenderTargetView(const TextureDesc* pDesc, ID3D11Resource* resource)
{
    D3D11_RENDER_TARGET_VIEW_DESC desc = {};
    desc.Format = GetResourceViewFormat(pDesc->Format);

    if (pDesc->Type == TextureType::Texture1D)
    {
        if (pDesc->ArraySize > 1)
        {
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.FirstArraySlice = 0;
            desc.Texture1DArray.ArraySize = pDesc->ArraySize;
            desc.Texture1DArray.MipSlice = 0;
        }
        else
        {
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
            desc.Texture1D.MipSlice = 0;
        }
    }
    else if (pDesc->Type == TextureType::Texture2D)
    {
        if (pDesc->ArraySize > 1)
        {
            if (pDesc->SampleDesc.Count > 1)
            {
                desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                desc.Texture2DMSArray.FirstArraySlice = 0;
                desc.Texture2DMSArray.ArraySize = pDesc->ArraySize;
            }
            else
            {
                desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                desc.Texture2DArray.FirstArraySlice = 0;
                desc.Texture2DArray.ArraySize = pDesc->ArraySize;
                desc.Texture2DArray.MipSlice = 0;
            }
        }
        else
        {
            if (pDesc->SampleDesc.Count > 1)
            {
                desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                desc.Texture2D.MipSlice = 0;
            }
        }
    }
    else if (pDesc->Type == TextureType::Texture3D)
    {
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
        desc.Texture3D.MipSlice = 0;
        desc.Texture3D.FirstWSlice = 0;
        desc.Texture3D.WSize = -1;
    }

    ID3D11RenderTargetView* rtv = nullptr;
    assert(SUCCEEDED(m_Device->CreateRenderTargetView(resource, &desc, &rtv)));

    return rtv;
}

ID3D11DepthStencilView* GraphicsDevice::CreateDepthStencilView(const TextureDesc* pDesc, ID3D11Resource* resource)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
    desc.Format = GetResourceViewFormat(pDesc->Format, true);


    if (pDesc->Type == TextureType::Texture1D)
    {
        if (pDesc->ArraySize > 1)
        {
            desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.FirstArraySlice = 0;
            desc.Texture1DArray.ArraySize = pDesc->ArraySize;
            desc.Texture1DArray.MipSlice = 0;
        }
        else
        {
            desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
            desc.Texture1D.MipSlice = 0;
        }
    }
    else if (pDesc->Type == TextureType::Texture2D)
    {
        if (pDesc->ArraySize > 1)
        {
            if (pDesc->SampleDesc.Count > 1)
            {
                desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
                desc.Texture2DMSArray.FirstArraySlice = 0;
                desc.Texture2DMSArray.ArraySize = pDesc->ArraySize;
            }
            else
            {
                desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                desc.Texture2DArray.FirstArraySlice = 0;
                desc.Texture2DArray.ArraySize = pDesc->ArraySize;
                desc.Texture2DArray.MipSlice = 0;
            }
        }
        else
        {
            if (pDesc->SampleDesc.Count > 1)
            {
                desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                desc.Texture2D.MipSlice = 0;
            }
        }
    }

    ID3D11DepthStencilView* dsv = nullptr;
    assert(SUCCEEDED(m_Device->CreateDepthStencilView(resource, &desc, &dsv)));

    return dsv;
}

ID3D11UnorderedAccessView* GraphicsDevice::CreateUnorderedAccessView(const TextureDesc* pDesc, ID3D11Resource* resource)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
    desc.Format = GetResourceViewFormat(pDesc->Format);


    if (pDesc->Type == TextureType::Texture1D)
    {
        if (pDesc->ArraySize > 1)
        {
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.FirstArraySlice = 0;
            desc.Texture1DArray.ArraySize = pDesc->ArraySize;
            desc.Texture1DArray.MipSlice = 0;
        }
        else
        {
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
            desc.Texture1D.MipSlice = 0;
        }
    }
    else if (pDesc->Type == TextureType::Texture2D)
    {
        if (pDesc->ArraySize > 1)
        {
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.ArraySize = pDesc->ArraySize;
            desc.Texture2DArray.MipSlice = 0;
        }
        else
        {
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipSlice = 0;
        }
    }
    else if (pDesc->Type == TextureType::Texture3D)
    {
        desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
        desc.Texture3D.MipSlice = 0;
        desc.Texture3D.FirstWSlice = 0;
        desc.Texture3D.WSize = -1;
    }

    ID3D11UnorderedAccessView* uav = nullptr;
    assert(SUCCEEDED(m_Device->CreateUnorderedAccessView(resource, &desc, &uav)));

    return uav;
}

DXGI_FORMAT GraphicsDevice::GetResourceViewFormat(SurfaceFormat format, bool isDepth)
{
    if (isDepth == false)
    {
        switch (format)
        {
        case SurfaceFormat::R16_Typeless:
            return DXGI_FORMAT_R16_UNORM;
            break;

        case SurfaceFormat::R32_Typeless:
            return DXGI_FORMAT_R32_FLOAT;
            break;

        case SurfaceFormat::R24G8_Typeless:
            return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            break;

        case SurfaceFormat::R32G8X24_Typeless:
            return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
            break;

        case SurfaceFormat::R8_Typeless:
            return DXGI_FORMAT_R8_UNORM;
            break;

        default:
            return (DXGI_FORMAT)FormatToDXGIFormat(format);
            break;
        }
    }
    else
    {
        switch (format)
        {
        case SurfaceFormat::R16_Typeless:
            return DXGI_FORMAT_D16_UNORM;
            break;

        case SurfaceFormat::R32_Typeless:
            return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
            break;

        case SurfaceFormat::R24G8_Typeless:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
            break;

        case SurfaceFormat::R32G8X24_Typeless:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            break;

        default:
            // Give up at this point...
            return (DXGI_FORMAT)FormatToDXGIFormat(format);
            break;
        }
    }
}

ID3D11BlendState* GraphicsDevice::GetBlendState(const BlendDesc& pDesc)
{
    Uint32 hash = Hash32::ComputeHash((Byte*)&pDesc, sizeof(BlendDesc));
    BlendState* pBlendState = nullptr;
    Uint16 handle = m_BlendMap.FindByHash(hash, &pBlendState);
    ID3D11BlendState* blendState = nullptr;

    if (pBlendState == nullptr)
    {
        D3D11_BLEND_DESC desc = {};
        desc.AlphaToCoverageEnable = pDesc.AlphaToCoverageEnabled;
        desc.IndependentBlendEnable = pDesc.IndependentBlendEnabled;

        for (size_t i = 0; i < 8; i++)
        {
            desc.RenderTarget[i].BlendEnable = pDesc.RenderTarget[i].BlendEnabled;
            desc.RenderTarget[i].SrcBlend = BlendTable[(Uint32)pDesc.RenderTarget[i].SrcColorBlend];
            desc.RenderTarget[i].DestBlend = BlendTable[(Uint32)pDesc.RenderTarget[i].DestColorBlend];
            desc.RenderTarget[i].BlendOp = BlendOpTable[(Uint32)pDesc.RenderTarget[i].ColorBlendOp];

            desc.RenderTarget[i].SrcBlendAlpha = BlendTable[(Uint32)pDesc.RenderTarget[i].SrcAlphaBlend];
            desc.RenderTarget[i].DestBlendAlpha = BlendTable[(Uint32)pDesc.RenderTarget[i].DestAlphaBlend];
            desc.RenderTarget[i].BlendOpAlpha = BlendOpTable[(Uint32)pDesc.RenderTarget[i].AlphaBlendOp];
            desc.RenderTarget[i].RenderTargetWriteMask = (Uint8)ConvertColorMask((Uint32)pDesc.RenderTarget[i].ColorWriteMask);
        }

        // Create and allocate
        assert(SUCCEEDED(m_Device->CreateBlendState(&desc, &blendState)));
        handle = m_BlendMap.Allocate(hash, &pBlendState);
        pBlendState->m_Blend = blendState;
    }
    else
    {
        blendState = pBlendState->m_Blend;
    }

    return blendState;
}

ID3D11RasterizerState* GraphicsDevice::GetRasterState(const RasterDesc& pDesc)
{
    //--Raster State--
    Uint32 hash = Hash32::ComputeHash((Byte*)&pDesc, sizeof(RasterDesc));
    RasterizerState* pRaster = nullptr;
    Uint16 handle = m_RasterMap.FindByHash(hash, &pRaster);
    ID3D11RasterizerState* rasterState = nullptr;

    if (pRaster == nullptr)
    {
        D3D11_RASTERIZER_DESC desc = {};
        desc.FillMode = (pDesc.Fill == FillMode::Solid) ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
        desc.CullMode = CullModeTable[(Uint32)pDesc.CullMode];
        desc.FrontCounterClockwise = pDesc.FrontClockWise;
        desc.DepthBias = pDesc.DepthBias;
        desc.DepthBiasClamp = pDesc.DepthBiasClamp;
        desc.SlopeScaledDepthBias = pDesc.SlopeDepthBias;
        desc.DepthClipEnable = pDesc.DepthClipEnable;
        desc.MultisampleEnable = pDesc.MultisampleEnable;
        desc.AntialiasedLineEnable = pDesc.AntialiasedLineEnable;
        desc.ScissorEnable = false;

        assert(SUCCEEDED(m_Device->CreateRasterizerState(&desc, &rasterState)));
        handle = m_RasterMap.Allocate(hash, &pRaster);
        pRaster->m_Raster = rasterState;
    }
    else
    {
        rasterState = pRaster->m_Raster;
    }
    return rasterState;
}


ID3D11DepthStencilState* GraphicsDevice::GetDepthState(const DepthDesc& pDesc)
{
    //--Depth State--
    Uint32 hash = Hash32::ComputeHash((Byte*)&pDesc, sizeof(DepthDesc));
    DepthState* pDepthState = nullptr;
    Uint16 handle = m_DepthMap.FindByHash(hash, &pDepthState);
    ID3D11DepthStencilState* depthState = nullptr;

    if (pDepthState == nullptr)
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        desc.DepthEnable = pDesc.DepthEnabled;
        desc.DepthWriteMask = DepthWriteTable[(Uint32)pDesc.DepthWriteMask];
        desc.DepthFunc = ComparisonTable[(Uint32)pDesc.DepthFunc];
        desc.StencilEnable = pDesc.StencilEnabled;
        desc.StencilReadMask = (Uint8)pDesc.StencilRead;
        desc.StencilWriteMask = (Uint8)pDesc.StencilWrite;
        desc.FrontFace.StencilPassOp = StencilOpTable[(Uint32)pDesc.FrontFace.StencilDepthFailOp];
        desc.FrontFace.StencilFailOp = StencilOpTable[(Uint32)pDesc.FrontFace.StencilFailOp];
        desc.FrontFace.StencilDepthFailOp = StencilOpTable[(Uint32)pDesc.FrontFace.StencilDepthFailOp];
        desc.FrontFace.StencilFunc = ComparisonTable[(Uint32)pDesc.FrontFace.StencilFunc];
        desc.BackFace.StencilPassOp = StencilOpTable[(Uint32)pDesc.BackFace.StencilDepthFailOp];
        desc.BackFace.StencilFailOp = StencilOpTable[(Uint32)pDesc.BackFace.StencilFailOp];
        desc.BackFace.StencilDepthFailOp = StencilOpTable[(Uint32)pDesc.BackFace.StencilDepthFailOp];
        desc.BackFace.StencilFunc = ComparisonTable[(Uint32)pDesc.BackFace.StencilFunc];

        assert(SUCCEEDED(m_Device->CreateDepthStencilState(&desc, &depthState)));
        handle = m_DepthMap.Allocate(hash, &pDepthState);
        pDepthState->m_Depth = depthState;
    }
    else
    {
        depthState = pDepthState->m_Depth;
    }

    return depthState;
}

ID3D11InputLayout* GraphicsDevice::GetInputLayout(const InputLayoutDesc& pDesc, const ShaderHandle shader)
{
    Uint32 hash = Hash32::ComputeHash((Byte*)&pDesc.m_LayoutElements[0], sizeof(InputElement) * (Uint32)pDesc.m_LayoutElements.size());
    InputLayout* pLayout = nullptr;
    Uint16 handle = m_InputMap.FindByHash(hash, &pLayout);
    ID3D11InputLayout* layout = nullptr;

    if (pLayout == nullptr)
    {
        Uint32 attribCount = pDesc.Size();
        D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC[attribCount];

        for (Uint32 i = 0; i < attribCount; i++)
        {
            const InputElement* attrib = &pDesc.m_LayoutElements[i];

            desc[i].SemanticName = attrib->SemanticName.c_str();
            desc[i].SemanticIndex = attrib->InputSlot;
            desc[i].Format = (DXGI_FORMAT)FormatToDXGIFormat(attrib->Format);
            desc[i].InputSlot = attrib->InputSlot;
            desc[i].AlignedByteOffset = attrib->AlignedByteOffset;
            desc[i].InputSlotClass = InputClassTable[(Uint32)attrib->InputSlotClass];
            desc[i].InstanceDataStepRate = attrib->InstanceDataStepRate;
        }

        GraphicsShader* pShader = m_Shaders[shader];
        assert(pShader && "Shader Not Found");

        assert(SUCCEEDED(m_Device->CreateInputLayout(desc, attribCount, pShader->m_ByteCode, pShader->m_Length, &layout)));
        handle = m_InputMap.Allocate(hash, &pLayout);
        pLayout->m_Input = layout;
    }
    else
    {
        layout = pLayout->m_Input;
    }

    return layout;
}

bool GraphicsDevice::InitializeBlitter()
{
    PipelineDesc desc;
    desc.BlendState  = CommonStates::Opaque;
    desc.DepthState  = CommonStates::DepthNone;
    desc.RasterState = CommonStates::CullNone;
    desc.Topology    = PrimitiveTopology::TriangleList;
    desc.InputLayout = InputLayoutDesc(VertexTexture::InputLayout, 2);

    Byte* bytecode; Uint32 length;
    CompileShader("Assets/Shaders/blit.hlsl", "vert", "vs_5_0", &bytecode, length);
    desc.VertexShader = CreateShader(ShaderType::VS, bytecode, length);
    delete[] bytecode;

    CompileShader("Assets/Shaders/blit.hlsl", "frag", "ps_5_0", &bytecode, length);
    desc.PixelShader = CreateShader(ShaderType::PS, bytecode, length);
    delete[] bytecode;

    m_BlitPipeline = CreatePipeline(&desc);

    return m_BlitPipeline.IsValid();
}
