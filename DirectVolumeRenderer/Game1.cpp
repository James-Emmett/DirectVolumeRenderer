#include "Game1.h"
#include "Application/Application.h"

void Game1::Initialize()
{
	// Set clear color
	m_GraphicsDevice->SetDefaultClearColor(Color(0.01f, 0.01f, 0.01f, 1.0f));

	// Init scene and set skybox
	m_Scene.Initialize(m_GraphicsDevice);
	m_Forwardrenderer.Initialize(m_GraphicsDevice);
	
	SkyBox* skyBox = &m_Scene.m_RenderSettings.m_SkyBox;
	skyBox->m_SkyBox		 = m_ContentManager->Load<Texture>("Assets/Textures/CubeMap/Palermo/PalermoSkyBox.dds");
	skyBox->m_DiffuseEnvHDR  = m_ContentManager->Load<Texture>("Assets/Textures/CubeMap/Palermo/PalermoDiffuseHDR.dds");
	skyBox->m_SpecularEnvHDR = m_ContentManager->Load<Texture>("Assets/Textures/CubeMap/Palermo/PalermoSpecularHDR.dds");
	skyBox->m_SpecularLUT	 = m_ContentManager->Load<Texture>("Assets/Textures/CubeMap/Palermo/PalermoBrdf.dds");
	
	// Create Camera
	Entity* camera = m_Scene.CreateEntity(Application::NextEntityID());
	m_Camera = camera->AddComponent<FlyCamera>();
	
	if (m_GameSettings.GetIsStero() == false)
	{
		m_Camera->SetCamera((Uint32)m_GameSettings.GetWidth(), (Uint32)m_GameSettings.GetHeight(), 75, 0.01f, 1000, Projection::Perspective);
	}
	else
	{
		m_Camera->SetCamera((Uint32)m_VRManager->GetResolution().x, (Uint32)m_VRManager->GetResolution().y, 75, 0.01f, 1000, Projection::Perspective);
	}
	
	m_Camera->m_Transform->SetPosition(Vector3(0, 0, -3));
	m_Camera->m_Background = Color(0.3f, 0.3f, 0.3f, 1.0f);
	
	//Entity* cabinet = m_Scene.CreateEntity(Application::NextEntityID());
	//cabinet->m_Transform->SetPosition(Vector3(2, 0, 0));
	//m_GothicCabinet = cabinet->AddComponent<MeshRenderer>();
	//m_GothicCabinet->m_Mesh = m_ContentManager->Load<Mesh>("Assets/Models/GothicCabinet_01/GothicCabinet_01.mesh");
	//m_GothicCabinet->m_Material = m_ContentManager->Load<Material>("Assets/Models/GothicCabinet_01/GothicCabinet.material");
	
	Entity* grid = m_Scene.CreateEntity(Application::NextEntityID());
	m_Grid = grid->AddComponent<GridRenderer>();
	m_Grid->Initialize(20);
	
	// Create volume
	Entity* volume = m_Scene.CreateEntity(Application::NextEntityID());
	volume->m_Transform->SetPosition(Vector3(0, 1, 0));
	volume->m_Transform->Rotate(Vector3(-90, 0, 0));
	m_VolumeComponent = volume->AddComponent<VolumeComponent>();
	m_VolumeComponent->Initialize(m_GraphicsDevice, m_ContentManager);
	// Load a default raw file i.e Manix
	m_VolumeComponent->LoadVolume("Assets/Models/Male/male.raw");
}

void Game1::LoadContent()
{

}

void Game1::FixedUpdate()
{

}

void Game1::Update(float deltaTime)
{
	m_Camera->Update(deltaTime);
}

void Game1::Draw()
{
	m_Forwardrenderer.Render(&m_Scene);
}

void Game1::OnGui()
{
	m_VolumeComponent->OnGui();
}

void Game1::OnResize()
{
	m_Forwardrenderer.Resize();

	// Only resize if its not VR?
	if (m_GameSettings.GetIsStero() == false)
	{
		m_Camera->SetCamera(m_GameSettings.GetWidth(), m_GameSettings.GetHeight(), 75, 0.01f, 1000, Projection::Perspective);
	}
}

void Game1::ShutDown()
{
	m_Forwardrenderer.ShutDown();
	m_VolumeComponent->Shutdown();
}