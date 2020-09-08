#include "World/Renderer/SkyBox.h"
#include "World/Component/Camera.h"
#include "World/Component/Transform.h"
#include "Graphics/GraphicsDevice.h"
#include "Content/ContentManager.h"
#include "Content/Shader.h"
#include "World/Renderer/RenderCommon.h"
#include "System/Assert.h"

SkyBox::SkyBox()
{
}

SkyBox::~SkyBox()
{
}

void SkyBox::Initialize(GraphicsDevice* device, ContentManager* contentManager)
{
    assert(device && "Null device passed to skybox");
    m_GraphicsDevice = device;
    m_ContentManager = contentManager;

    // Create cube in vert buffer
    if (m_Vertexbuffer.IsValid() == false)
    {
        float skyboxVertices[] = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        m_Vertexbuffer = m_GraphicsDevice->CreateVertexBuffer(36, 12, BufferUsage::Immutable, (Byte*)skyboxVertices);
    }

    if (m_ConstantBuffer.IsValid() == false)
    {
        m_ConstantBuffer = m_GraphicsDevice->CreateConstantBuffer(sizeof(Matrix4), (Byte*)&m_WorldVieProj);
    }

    if (m_Shader == nullptr)
    {
        m_Shader = m_ContentManager->Load<Shader>("Assets/Shaders/Skybox/SkyBox.shader");
    }
}

void SkyBox::BindEnviromentMaps()
{
    // Sky Box
    m_GraphicsDevice->BindTexture(m_SkyBox->GetTextureHandle(), (Uint32)TextureDefaults::Skybox);
    m_GraphicsDevice->BindSampler(m_SkyBox->GetSampleHandle(), (Uint32)TextureDefaults::Skybox);
    // Diffuse Irradiance
    m_GraphicsDevice->BindTexture(m_DiffuseEnvHDR->GetTextureHandle(), (Uint32)TextureDefaults::Irradiance);
    m_GraphicsDevice->BindSampler(m_DiffuseEnvHDR->GetSampleHandle(), (Uint32)TextureDefaults::Irradiance);
    // Specular Radiance
    m_GraphicsDevice->BindTexture(m_SpecularEnvHDR->GetTextureHandle(), (Uint32)TextureDefaults::SpecularHDR);
    m_GraphicsDevice->BindSampler(m_SpecularEnvHDR->GetSampleHandle(), (Uint32)TextureDefaults::SpecularHDR);
    // Specular LUT
    m_GraphicsDevice->BindTexture(m_SpecularLUT->GetTextureHandle(), (Uint32)TextureDefaults::SpecularLUT);
    m_GraphicsDevice->BindSampler(m_SpecularLUT->GetSampleHandle(), (Uint32)TextureDefaults::SpecularLUT);
}

void SkyBox::Draw(Matrix4 view, Matrix4 projection)
{
    // Keep Box around camera
    Matrix4 world = Matrix4::Translate(Matrix4::Inverse(view).GetColumn(3));
    m_WorldVieProj = projection * view * world;
    m_GraphicsDevice->UpdateBuffer(m_ConstantBuffer, (Byte*)&m_WorldVieProj, sizeof(Matrix4));

    // Bind shader and mesh / textures
    m_Shader->Bind();
    m_GraphicsDevice->BindVertexBuffer(m_Vertexbuffer, 0);
    m_GraphicsDevice->BindConstantBuffer(m_ConstantBuffer, (Uint32)UniformTypes::Count);
    m_GraphicsDevice->Draw(36, 0, 0);
}