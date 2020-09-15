#include "World/Renderer/ForwardRenderer.h"
#include "World/Component/Transform.h"
#include "Application/Application.h"
#include "Application/GameSettings.h"
#include "World/Component/MeshRenderer.h"
#include "Application/Game.h"
#include "World/Scene.h"

#include "UI/ImGui_Interface.h"

void ForwardRenderer::Initialize(GraphicsDevice* device)
{
	m_GraphicsDevice = device;
	m_VRManager = Application::game->GetVRManager();
	m_FoveatedRendering = Application::game->GetFoveatedHelper();

	BaseRenderer::Initialize(device);
	GameSettings* gameSettings = Application::gameSettings;

	Vector2 resolution = Vector2((float)gameSettings->GetWidth(), (float)gameSettings->GetHeight());

	if (gameSettings->GetIsStero())
	{
		resolution = m_VRManager->GetResolution();
		m_RenderTargets[0] = m_GraphicsDevice->CreateRenderTarget((Uint32)resolution.x, (Uint32)resolution.y, RenderFormat::R8G8B8A8_Unorm);
		m_RenderTargets[1] = m_GraphicsDevice->CreateRenderTarget((Uint32)resolution.x, (Uint32)resolution.y, RenderFormat::R8G8B8A8_Unorm);
		m_DepthTargets[0]  = m_GraphicsDevice->CreateDepthTarget((Uint32)resolution.x, (Uint32)resolution.y, DepthFormat::Depth32);
		m_DepthTargets[1]  = m_GraphicsDevice->CreateDepthTarget((Uint32)resolution.x, (Uint32)resolution.y, DepthFormat::Depth32);
	}
	else
	{
		m_RenderTargets[0] = m_GraphicsDevice->CreateRenderTarget((Uint32)resolution.x, (Uint32)resolution.y, RenderFormat::R16G16B16A16_Float);
		m_DepthTargets[0]  = m_GraphicsDevice->CreateDepthTarget((Uint32)resolution.x, (Uint32)resolution.y, DepthFormat::Depth32);
	}

	m_ToneMapper.Initialize(m_GraphicsDevice);
}

void ForwardRenderer::Render(Scene* scene)
{
	if (scene == nullptr || scene->m_CameraList.size() == 0) { return; }

	GameSettings* settings = Application::gameSettings;

	// Bind Enviroment Maps: sky, irradiance etc.
	SetEnviromentTextures(scene);
	SetConstantBuffers();
	SetUpFrame();

	// Render shadows there same for all cameras and eyes(vr)

	// Render scene for both eyes
	if (settings->GetIsStero())
	{
		Vector2 res = m_VRManager->GetResolution();
		ViewPort viewPort = ViewPort(res.x, res.y);
		m_GraphicsDevice->BindScissor(0, 0, (Uint32)res.x, (Uint32)res.y);
		m_GraphicsDevice->BindViewPort(viewPort);
		m_VRManager->BeginFrame();

		if (settings->IsVRS())
		{
			m_FoveatedRendering->EnableFoveatedRendering(NV_VRS_RENDER_MODE_STEREO);
			m_FoveatedRendering->UpdateGazeData(0.5, 0.5, true);
			m_FoveatedRendering->LatchGazeData();
		}

		for (size_t i = 0; i < 2; i++)
		{
			std::shared_ptr<Camera> camera = scene->m_CameraList[0];
			camera->SetView(m_VRManager->GetView((Eye)i, camera->m_Transform->World()));
			camera->SetProjection(m_VRManager->GetProjection((Eye)i));
			RenderCamera(scene, scene->m_CameraList[0], m_RenderTargets[i], m_DepthTargets[i]);
		}

		if (settings->IsVRS())
		{
			m_FoveatedRendering->DisableFoveatedRendering();
		}

		m_VRManager->BindTarget(Eye::Left);
		m_ToneMapper.Apply(m_RenderTargets[0]);

		m_VRManager->BindTarget(Eye::Right);
		m_ToneMapper.Apply(m_RenderTargets[1]);

		m_VRManager->EndFrame();
	}
	else
	{
		if (settings->IsVRS())
		{
			m_FoveatedRendering->EnableFoveatedRendering(NV_VRS_RENDER_MODE_MONO);
			m_FoveatedRendering->UpdateGazeData(0.0f, 0.0f, false);
			m_FoveatedRendering->LatchGazeData();
		}

		m_GraphicsDevice->BindDefaultViewPortAndScissor();
		// just render first camera for testing!
		RenderCamera(scene, scene->m_CameraList[0], m_RenderTargets[0], m_DepthTargets[0]);
	
		if (settings->IsVRS())
		{
			m_FoveatedRendering->DisableFoveatedRendering();
		}
	}

	ImGui::Begin("debug");
	m_ToneMapper.OnGui();
	ImGui::End();

	//--ToneMap Too BackBuffer LDR--
	m_GraphicsDevice->BindRenderTarget();
	m_ToneMapper.Apply(m_RenderTargets[0]);
}

void ForwardRenderer::RenderShadows(Scene* scene)
{
	// nothign for now.
}

// Need a frame allocator?
void ForwardRenderer::RenderCamera(Scene* scene, std::shared_ptr<Camera> camera, RenderHandle renderTarget, DepthHandle depthTarget)
{
	m_GraphicsDevice->ClearRenderTarget(renderTarget, camera->m_Background.Linear());
	m_GraphicsDevice->ClearDepthTarget(depthTarget, 1.0f, 0);
	m_GraphicsDevice->BindRenderTarget(renderTarget, depthTarget);

	SetCameraProperties(camera);

	std::vector<std::shared_ptr<RenderComponent>>& renderList = scene->m_RenderList;
	RenderQueue geometryQueue;
	RenderQueue transQueue;

	geometryQueue.m_Camera = camera;
	transQueue.m_Camera = camera;

	for (size_t i = 0; i < renderList.size(); ++i)
	{
		if (renderList[i]->GetRenderQueue() == RenderType::Opaque)
		{
			RenderItem& item = geometryQueue.Alocate();
			item.m_Renderer = renderList[i];
		}
		else if (renderList[i]->GetRenderQueue() == RenderType::Transparent)
		{
			RenderItem& item = transQueue.Alocate();
			item.m_Renderer = renderList[i];
		}
	}

	// Render Opaque
	DrawRenderQueue(geometryQueue);

	// Draw skybox after opaque no overdraw
	if (camera->m_ClearFlags == ClearFlag::SkyBox)
	{
		DrawSkybox(scene, camera);
	}

	// Render Transparents
	DrawRenderQueue(transQueue);

	//Post Process here, need to fix
	RenderPostProcess(scene, renderTarget);
}

void ForwardRenderer::DrawRenderQueue(const RenderQueue& renderQueue)
{
	// do light stuff?
	for (Uint32 i = 0; i < renderQueue.Size(); i++)
	{
		// Renderer will be repalced by the RenderItem eventurally.
		const RenderItem& renderItem = renderQueue[i];
		std::shared_ptr<RenderComponent> renderer = renderItem.m_Renderer;

		// Set Per Draw Constant Buffer
		ObjectConstBuffer objectCB;
		objectCB.m_World = renderer->m_Transform->World();
		objectCB.m_InvWorld = Matrix4::Inverse(objectCB.m_World);
		objectCB.m_WorldViewProj = renderQueue.m_Camera->GetProjection() * renderQueue.m_Camera->GetView() * objectCB.m_World;
		objectCB.m_NormalMatrix = Matrix4::Transpose(objectCB.m_InvWorld);
		objectCB.m_CameraPosLocal = objectCB.m_InvWorld.TransformPoint(renderQueue.m_Camera->GetPosition());
		m_GraphicsDevice->UpdateBuffer(m_ConstantBuffers[(Uint32)UniformTypes::Object], (Byte*)&objectCB, sizeof(ObjectConstBuffer));

		renderer->Draw(m_GraphicsDevice);
	}
}

void ForwardRenderer::Resize()
{
	if (Application::gameSettings->GetIsStero())
	{
		return;
	}
	else
	{
		GameSettings* gameSettings = Application::gameSettings;
		Vector2 resolution = Vector2((float)gameSettings->GetWidth(), (float)gameSettings->GetHeight());

		if (m_RenderTargets[0].IsValid())
		{
			m_GraphicsDevice->DestroyTexture(m_RenderTargets[0]);
			m_RenderTargets[0] = m_GraphicsDevice->CreateRenderTarget((Uint32)resolution.x, (Uint32)resolution.y, RenderFormat::R8G8B8A8_Unorm);
		}

		if (m_DepthTargets[0].IsValid())
		{
			m_GraphicsDevice->DestroyTexture(m_DepthTargets[0]);
			m_DepthTargets[0] = m_GraphicsDevice->CreateDepthTarget((Uint32)resolution.x, (Uint32)resolution.y, DepthFormat::Depth32);
		}
	}
}

void ForwardRenderer::ShutDown()
{
	if (m_RenderTargets[0].IsValid()) { m_GraphicsDevice->DestroyTexture(m_RenderTargets[0]); }
	if (m_RenderTargets[1].IsValid()) { m_GraphicsDevice->DestroyTexture(m_RenderTargets[1]); }

	if (m_DepthTargets[0].IsValid()) { m_GraphicsDevice->DestroyTexture(m_DepthTargets[0]); }
	if (m_DepthTargets[1].IsValid()) { m_GraphicsDevice->DestroyTexture(m_DepthTargets[1]); }

	m_ToneMapper.Release();
}
