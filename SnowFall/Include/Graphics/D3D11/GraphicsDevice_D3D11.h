//Note:
/*
    Note you do NOT need to destory:
    -Blend
    -Depth
    -Raster
    -InputLayout
*/

#pragma once
#include "Graphics/Graphics.h"
#include "Graphics/GraphicsAdapter.h"
#include "System/Types.h"
#include "Math/Rectangle.h"
#include "System/FreeList.h"

#include <d3d11.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <unordered_map>

// Link libs
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "dxgi.lib")

struct GraphcisObject
{
    ID3D11Resource* m_Resource = nullptr;
    ID3D11ShaderResourceView* m_SRV = nullptr;
    ID3D11UnorderedAccessView* m_UAV = nullptr;

    bool IsValid()
    {
        return m_Resource != nullptr;
    }

    virtual void Release()
    {
        if (m_Resource) { m_Resource->Release(); }
        if (m_SRV) { m_SRV->Release(); }
        if (m_UAV) { m_UAV->Release(); }

        m_Resource = nullptr;
        m_SRV = nullptr;
        m_UAV = nullptr;
    }
};

struct Buffer : public GraphcisObject
{
    BufferDesc m_Desc;

    ~Buffer()
    {
        Release();
    }
};

struct GraphicsTexture : public GraphcisObject
{
    ID3D11RenderTargetView* m_RTV = nullptr;
    ID3D11DepthStencilView* m_DSV = nullptr;
    TextureDesc m_Desc;

    ~GraphicsTexture()
    {
        Release();
    }

    void Release()
    {
        if (m_RTV) { m_RTV->Release(); }
        if (m_DSV) { m_DSV->Release(); }
        m_RTV = nullptr;
        m_DSV = nullptr;
        GraphcisObject::Release();
    }
};

struct SamplerState
{
    ID3D11SamplerState* m_Sampler = nullptr;

    ~SamplerState()
    {
        Release();
    }

    void Release()
    {
        if (m_Sampler)
        {
            m_Sampler->Release();
            m_Sampler = nullptr;
        }
    }

    bool IsValid()
    {
        return m_Sampler != nullptr;
    }
};

struct BlendState
{
    ID3D11BlendState* m_Blend = nullptr;

    ~BlendState()
    {
        Release();
    }

    void Release()
    {
        if (m_Blend)
        {
            m_Blend->Release();
            m_Blend = nullptr;
        }
    }

    bool IsValid()
    {
        return m_Blend != nullptr;
    }
};

struct DepthState
{
    ID3D11DepthStencilState* m_Depth = nullptr;

    ~DepthState()
    {
        Release();
    }

    void Release()
    {
        if (m_Depth)
        {
            m_Depth->Release();
            m_Depth = nullptr;
        }
    }

    bool IsValid()
    {
        return m_Depth != nullptr;
    }
};

struct RasterizerState
{
    ID3D11RasterizerState* m_Raster = nullptr;

    ~RasterizerState()
    {
        Release();
    }

    void Release()
    {
        if (m_Raster)
        {
            m_Raster->Release();
            m_Raster = nullptr;
        }
    }

    bool IsValid()
    {
        return m_Raster != nullptr;
    }
};

struct InputLayout
{
    ID3D11InputLayout* m_Input = nullptr;

    ~InputLayout()
    {
        Release();
    }

    void Release()
    {
        if (m_Input)
        {
            m_Input->Release();
            m_Input = nullptr;
        }
    }

    bool IsValid()
    {
        return m_Input != nullptr;
    }
};

struct GraphicsShader
{
    ID3D11Resource* m_Shader = nullptr;
    Byte* m_ByteCode = nullptr;
    Uint32 m_Length = 0;

    GraphicsShader()
    {
        m_Shader = nullptr;
        m_ByteCode = nullptr;
        m_Length = 0;
    }

    ~GraphicsShader()
    {
        Release();
    }

    void Release()
    {
        if (m_Shader)
        {
            m_Shader->Release();
            m_Shader = nullptr;
        }

        if (m_ByteCode)
        {
            delete[] m_ByteCode;
            m_Length = 0;
        }
    }

    bool IsValid()
    {
        return m_Shader != nullptr;
    }
};

struct PipelineState
{
    ShaderHandle				m_VertexShader = ShaderHandle();
    ShaderHandle				m_PixelShader = ShaderHandle();
    ShaderHandle				m_DomainShader = ShaderHandle();
    ShaderHandle				m_HullShader = ShaderHandle();
    ShaderHandle				m_GeometaryShader = ShaderHandle();
    ID3D11BlendState* m_BlendState = nullptr;
    ID3D11RasterizerState* m_RasterState = nullptr;
    ID3D11DepthStencilState* m_DepthStencilState = nullptr;
    ID3D11InputLayout* m_InputLayout = nullptr;
    PrimitiveTopology			m_TopologyType = PrimitiveTopology::TriangleList;
    Uint32						m_NumRenderTargets = 1;
    DXGI_FORMAT					m_RTVFormats[8] = { DXGI_FORMAT_UNKNOWN };
    DXGI_FORMAT					m_DSVFormat = DXGI_FORMAT_UNKNOWN;
    float						m_BlendFactor[4] = { 1,1,1,1 };
    Uint32						m_SampleMask = 0xffffffff;

    ~PipelineState()
    {
        Release();
    }

    void Release()
    {
        // States release by engine
        // Shaders released seperatly by user?
    }
};

namespace Internal
{
    // This is very specific to DX11 So dont need it for DX12 etc.
    template <class T>
    class FixedCache
    {
    private:
        std::vector<T> m_Items;
        typedef std::unordered_map<Uint32, Uint16> HashMap;
        HashMap m_HashMap;

        Uint32 m_Count = 0;
        Uint32 m_MaxCount = 0;

    public:
        void Initialize(Uint32 maxSize)
        {
            m_Count = 0;
            m_MaxCount = maxSize;
            m_Items.resize(m_MaxCount);
        }

        Uint16 Allocate(Uint32 key, T** outValue)
        {
            // If already contaiend just return index
            typename HashMap::iterator it = m_HashMap.find(key);
            if (it != m_HashMap.end())
            {
                m_HashMap.erase(key);
            }

            Uint16 index = (Uint16)m_Count;
            *outValue = &m_Items[index];
            m_HashMap.insert(std::make_pair(key, index));
            m_Count++;
            return index;
        }

        Uint16 FindByHash(Uint32 key, T** outValue)
        {
            typename HashMap::iterator it = m_HashMap.find(key);
            if (it != m_HashMap.end())
            {
                *outValue = &m_Items[it->second];
                return it->second;
            }

            outValue = nullptr;
            return UINT16_MAX;
        }

        T* FindByIndex(Uint32 index)
        {
            return &m_Items[index];
        }

        // Call when shutting down the Engine.
        void Clear()
        {
            for (size_t i = 0; i < m_Items.size(); i++)
            {
                if (m_Items[i].IsValid())
                {
                    m_Items[i].Release();
                }
            }
            m_Items.clear();
            m_HashMap.clear();
        }

        Uint32 Count()
        {
            return m_Count;
        }
    };
}

class GraphicsDevice
{
private:
    GraphicsParameters	   m_Parameters;		// Describes swapchain etc.
    GPUMemory			   m_MemoryLimits;		//Describes Allocations allowed.
    const GraphicsAdapter* m_Adapter = nullptr; // Adapter this device is running on.

    //--Core D3D Objects--
    ID3D11Device*           m_Device = nullptr;
    IDXGISwapChain3*        m_SwapChain = nullptr;
    IDXGIFactory6*          m_Factory = nullptr;
    ID3D11Debug*            m_Debug = nullptr;
    ID3D11RenderTargetView* m_RenderTarget = nullptr;
    ID3D11DepthStencilView* m_DepthTarget = nullptr;
    ID3D11Texture2D*        m_BackBuffer = nullptr;
    D3D_FEATURE_LEVEL		m_FeatureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
    ID3D11DeviceContext*    m_DeviceContexts[COMMANDLIST_COUNT];
    ID3D11CommandList*      m_CommandLists[COMMANDLIST_COUNT];

    //--Settings/Data--
    Uint32	m_FrameCount = 0;
    UINT  	m_CompileFlags = 0;
    Color 	m_ClearColor = Color::CornflowerBlue;
    float 	m_ClearDepth = 1.0f;
    Uint32	m_ClearStencil = 0;
    Uint32  m_CommandListCount = 1;

    // Previous Bound Resources Per Frame.
    ID3D11VertexShader*      m_PrevVertexShader[COMMANDLIST_COUNT] = { nullptr };
    ID3D11PixelShader*       m_PrevPixelShader[COMMANDLIST_COUNT] = { nullptr };
    ID3D11HullShader*        m_PrevHullShader[COMMANDLIST_COUNT] = { nullptr };
    ID3D11DomainShader*      m_PrevDomainShader[COMMANDLIST_COUNT] = { nullptr };
    ID3D11GeometryShader*    m_PrevGeometryShader[COMMANDLIST_COUNT] = { nullptr };
    ID3D11ComputeShader*     m_PrevComputeShader[COMMANDLIST_COUNT] = { nullptr };
    ID3D11BlendState*        m_PrevBlendState[COMMANDLIST_COUNT] = { nullptr };
    ID3D11RasterizerState*   m_PrevRasterState[COMMANDLIST_COUNT] = { nullptr };
    ID3D11DepthStencilState* m_PrevDepthState[COMMANDLIST_COUNT] = { nullptr };
    ID3D11InputLayout*       m_PrevInputLayout[COMMANDLIST_COUNT] = { nullptr };
    D3D11_PRIMITIVE_TOPOLOGY m_PrevTopology[COMMANDLIST_COUNT] = { D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED };
    // Can have (16 samplers dx11) so we need Command list * Slot count amount
    ID3D11SamplerState*      m_PrevSamplerState[COMMANDLIST_COUNT * 16] = { nullptr };

    //--Fixed Cached States--
    Internal::FixedCache<RasterizerState>	m_RasterMap;
    Internal::FixedCache<DepthState>	    m_DepthMap;
    Internal::FixedCache<BlendState>		m_BlendMap;
    Internal::FixedCache<InputLayout>		m_InputMap;
    Internal::FixedCache<SamplerState>      m_SamplerMap;

    //--Other--
    FreeList<Buffer>            m_Buffers;
    FreeList<GraphicsTexture>   m_Textures;
    FreeList<PipelineState>     m_Pipelines;
    FreeList<GraphicsShader>    m_Shaders;

    //--Blit Stuff--
    PipelineHandle m_BlitPipeline;

public:
    bool Initialize(const GraphicsParameters& info, const GPUMemory& memoryLimits = GPUMemory(), const GraphicsAdapter* adapter = nullptr);
    void Reset(GraphicsParameters info);
    void FlushGPU();
    void ShutDown();

    //--Low_Level_Methods--
    BufferHandle        CreateBuffer(const BufferDesc* pDesc, const Byte* data);
    TextureHandle       CreateTexture(const TextureDesc* pDesc, const Byte* data);
    ShaderHandle        CreateShader(ShaderType stage, Byte* pByteCode, Uint32 length);
    SamplerHandle       CreateSamplerState(const SamplerDesc* pDesc);
    PipelineHandle      CreatePipeline(const PipelineDesc* pDesc);

    //--Helper_Methods--
    BufferHandle        CreateVertexBuffer(Uint32 vertexCount, Uint32 stride, BufferUsage usage, const Byte* data);
    BufferHandle        CreateIndexBuffer(Uint32 indexCount, IndexFormat format, BufferUsage usage, const Byte* data);
    BufferHandle        CreateConstantBuffer(Uint32 byteWidth, const Byte* data);
    TextureHandle	    CreateRenderTarget(Uint32 width, Uint32 height, RenderFormat renderFormat, Uint32 sampleCount = 1, Uint32 sampleQuality = 0);
    TextureHandle	    CreateDepthTarget(Uint32 width, Uint32 height, DepthFormat format, Uint32 sampleCount = 1, Uint32 sampleQuality = 0);

    //--Update Resources--
    void UpdateBuffer(const BufferHandle buffer, const Byte* data, Uint32 byteCount, CommandList cmd = 0);
    void UpdateTexture(const TextureHandle texture, const Byte* data, Uint32 byteCount, CommandList cmd = 0);
    void CopyTextureResource(const TextureHandle dest, const TextureHandle src, CommandList cmd = 0);
    void CopyBufferResource(const BufferHandle dest, const BufferHandle src, CommandList cmd = 0);
    void GetTextureData(const TextureHandle texture, Byte* data, Uint32 byteCount, CommandList cmd = 0);

    //--Bind Resources--
    void BindRenderTarget(RenderHandle renderTarget = RenderHandle(), DepthHandle depthTarget = DepthHandle(), CommandList cmd = 0);
    void BindRenderTarget(const RenderTargetGroup* renderTargetGroup, CommandList cmd = 0);
    void BindVertexBuffer(BufferHandle buffer, Uint32 offset, CommandList cmd = 0);
    void BindVertexBuffers(BufferHandle* buffers, Uint32* offsets, Uint32 count, CommandList cmd = 0);
    void BindIndexBuffer(BufferHandle buffer, Uint32 offset, CommandList cmd = 0);
    void BindConstantBuffer(BufferHandle buffer, Uint32 slot, CommandList cmd = 0);
    void BindTexture(TextureHandle texture, Uint32 slot, CommandList cmd = 0);
    void BindBufferUAV(BufferHandle handle, ShaderType stage, Uint32 slot, CommandList cmd = 0);
    void BindTextureUAV(TextureHandle handle, ShaderType stage, Uint32 slot, CommandList cmd = 0);
    void BindSampler(SamplerHandle sampler, Uint32 slot, CommandList cmd = 0);
    void BindPipelineState(PipelineHandle pipeline, CommandList cmd = 0);
    void BindComputeShader(ShaderHandle shader, CommandList cmd = 0);
    void BindScissor(Uint32 x, Uint32 y, Uint32 width, Uint32 height, CommandList cmd = 0);
    void BindScissorRects(const Rect<float>* pRects, Uint32 numRects, CommandList cmd = 0);
    void BindViewPort(const ViewPort& viewport, CommandList cmd = 0);
    void BindViewports(const ViewPort* pViewports, Uint32 count, CommandList cmd = 0);
    void UnbindResource(Uint32 slot, Uint32 num, CommandList cmd = 0);
    void UnbindUAV(Uint32 slot, Uint32 num, CommandList cmd = 0);
    void BindDefaultViewPortAndScissor(CommandList cmd = 0);
    void SetDefaultClearColor(Color color);
    void SetStencilRef(Uint32 ref, CommandList cmd = 0);
    void Dispatch(Uint32 x, Uint32 y, Uint32 z, CommandList cmd = 0);
    void ClearRenderTarget(const RenderHandle renderTarget, Color color, CommandList cmd = 0);
    void ClearDepthTarget(const DepthHandle depthTarget, float depth, Uint32 stencil, CommandList cmd = 0);

    //--Destory Methods--
    void DestroyBuffer(BufferHandle handle);
    void DestroyTexture(BufferHandle handle);
    void DestroyShader(ShaderHandle handle);
    void DestroyPipeline(PipelineHandle handle);

    //--Draw Methods--
    void PresentBegin();
    void PresentEnd();
    void Draw(Uint32 vertexCount, Uint32 startVertex, CommandList cmd);
    void DrawIndexed(Uint32 startIndex, Uint32 indexCount, Uint32 startVertex, CommandList cmd = 0);

    //--Fetch Functions--
    const GraphicsAdapter* GetAdapter()const;
    const GraphicsParameters& GetParameters()const;
    ID3D11Device* GetDevice()const;
    ID3D11DeviceContext* GetImmediateContext()const;
    void				        CompileShader(const std::string fileName, const char* entry, const char* shaderModel, Byte** byteCode, Uint32& byteLength);
    void                        ReportLiveObjects();

    // Access Low-Level Objects, NOT RECOMMENDED
    const Buffer* GetBuffer(BufferHandle handle);
    const GraphicsTexture* GetTexture(TextureHandle handle); // Use this for Render target and depth
    const PipelineState* GetPiplineObject(PipelineHandle handle);
    const GraphicsShader* GetShader(ShaderHandle handle);

    //--Blit functions--;
    // blit source to target, source is always bound to slot 0
    void BlitToBuffer(TextureHandle source, RenderHandle target, PipelineHandle pipelineHandle = PipelineHandle());
    // blit the source into what ever target is currently bound
    void BlitToBound(TextureHandle source, PipelineHandle pipelineHandle = PipelineHandle());

private:
    void					    CreateDefaultDetphTarget();
    ID3D11ShaderResourceView*   CreateShaderResourceView(const TextureDesc* pDesc, ID3D11Resource* resource);
    ID3D11RenderTargetView*     CreateRenderTargetView(const TextureDesc* pDesc, ID3D11Resource* resource);
    ID3D11DepthStencilView*     CreateDepthStencilView(const TextureDesc* pDesc, ID3D11Resource* resource);
    ID3D11UnorderedAccessView*  CreateUnorderedAccessView(const TextureDesc* pDesc, ID3D11Resource* resource);
    DXGI_FORMAT					GetResourceViewFormat(SurfaceFormat format, bool isDepth = false);
    ID3D11BlendState*           GetBlendState(const BlendDesc& desc);
    ID3D11RasterizerState*      GetRasterState(const RasterDesc& pDesc);
    ID3D11DepthStencilState*    GetDepthState(const DepthDesc& pDesc);
    ID3D11InputLayout*          GetInputLayout(const InputLayoutDesc& pDesc, const ShaderHandle shader);
    bool                        InitializeBlitter();
};