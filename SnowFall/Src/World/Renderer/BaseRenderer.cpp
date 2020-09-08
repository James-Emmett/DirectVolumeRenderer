#include "World/Renderer/BaseRenderer.h"
#include "Application/Application.h"
#include "World/Component/Camera.h"
#include "Application/Game.h"
#include "World/Scene.h"
#include "Math/Mathf.h"

void BaseRenderer::Initialize(GraphicsDevice* device)
{
	m_Blitter.Initialize(device, Application::contentManager);
	m_ConstantBuffers[(Uint32)UniformTypes::Frame]  = m_GraphicsDevice->CreateConstantBuffer(sizeof(FrameConstBuffer), nullptr);
	m_ConstantBuffers[(Uint32)UniformTypes::Camera] = m_GraphicsDevice->CreateConstantBuffer(sizeof(CameraConstBuffer), nullptr);
	m_ConstantBuffers[(Uint32)UniformTypes::Object] = m_GraphicsDevice->CreateConstantBuffer(sizeof(ObjectConstBuffer), nullptr);
}

void BaseRenderer::SetConstantBuffers()
{
	m_GraphicsDevice->BindConstantBuffer(m_ConstantBuffers[(Uint32)UniformTypes::Frame],  (Uint32)UniformTypes::Frame);
	m_GraphicsDevice->BindConstantBuffer(m_ConstantBuffers[(Uint32)UniformTypes::Camera], (Uint32)UniformTypes::Camera);
	m_GraphicsDevice->BindConstantBuffer(m_ConstantBuffers[(Uint32)UniformTypes::Object], (Uint32)UniformTypes::Object);
}

void BaseRenderer::SetUpFrame()
{
	GameSettings* settings = Application::game->GetGameSettings();
	FrameConstBuffer frameCB;
	frameCB.m_Width		= (float)settings->GetWidth();
	frameCB.m_Height	= (float)settings->GetHeight();
	frameCB.m_Time		= Application::time->TimeSinceStart();
	frameCB.m_SinTime	= Mathf::Sin(frameCB.m_Time);
	frameCB.m_CostTime	= Mathf::Cos(frameCB.m_Time);
	frameCB.m_DeltaTime = Application::time->DeltaTime();

	m_GraphicsDevice->UpdateBuffer(m_ConstantBuffers[(Uint32)UniformTypes::Frame], (Byte*)&frameCB, sizeof(frameCB));
}

void BaseRenderer::SetCameraProperties(std::shared_ptr<Camera> camera)
{
	CameraConstBuffer cameraCB;
	cameraCB.m_View					= camera->GetView();
	cameraCB.m_Projection			= camera->GetProjection();
	cameraCB.m_ViewProjection		= camera->GetViewProjection();
	cameraCB.m_InvView				= Matrix4::Inverse(cameraCB.m_View);
	cameraCB.m_InvProjection		= Matrix4::Inverse(cameraCB.m_Projection);
	cameraCB.m_InvViewProjection	= Matrix4::Inverse(cameraCB.m_View);
	cameraCB.m_CameraPosition		= camera->GetPosition();

	m_GraphicsDevice->UpdateBuffer(m_ConstantBuffers[(Uint32)UniformTypes::Camera], (Byte*)&cameraCB, sizeof(CameraConstBuffer));
}

void BaseRenderer::SetEnviromentTextures(Scene* scene)
{
	scene->m_RenderSettings.m_SkyBox.BindEnviromentMaps();
}

void BaseRenderer::DrawSkybox(Scene* scene, std::shared_ptr<Camera> camera)
{
	scene->m_RenderSettings.m_SkyBox.Draw(camera->GetView(), camera->GetProjection());
}
