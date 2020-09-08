#include "Application/Game.h"
#include "Application/Application.h"

void Game::Run()
{
	m_GraphicsDevice = new GraphicsDevice();
	m_ContentManager = new ContentManager();

	Application::game = this;
	Application::time = &m_Time;
	Application::contentManager = m_ContentManager;
	Application::gameSettings = &m_GameSettings;
	Application::graphicsDevice = m_GraphicsDevice;
	m_GameSettings.LoadSettings();

	//--Set Logging channels Up--
	LogHandler::Subscribe(&m_LogFile);
	LogHandler::Subscribe(&m_LogMessageBox);
	LogInfo("Initializing");


	//--Create window and graphics--
	m_Window.Create(Vector2(1024, 720), false, "Rune");
	if (m_GameSettings.GetIsStero())
	{
		m_VRManager = new VR_Manager();
		m_VRManager->Initialize();
		m_GraphicsDevice->Initialize(m_GameSettings.PrepareParameters(), GPUMemory(), m_VRManager->GetAdapterForHMD());
		m_VRManager->CreateSession(m_GraphicsDevice);
	}
	else
	{
		m_GraphicsDevice->Initialize(m_GameSettings.PrepareParameters(), GPUMemory(), nullptr);
	}

	m_ImGui.Initialize(m_GraphicsDevice, m_GameSettings.GetAssetPath() + "/Font/fontawesome-webfont.ttf");
	m_Time.Start();
	m_Accumulator = 0.0;
	Initialize();
	LoadContent();

	while (m_Window.IsOpen())
	{
		WindowEvent event;
		while (m_Window.PollEvent(event))
		{
			if (event.type == WindowEventType::Closed)
			{
				m_Window.Close();
				Exit();
			}

			if (event.type == WindowEventType::Resize)
			{
				// Only bother if we need too, prevents looping
				if (event.resized.width != m_GameSettings.GetWidth() || event.resized.height != m_GameSettings.GetHeight())
				{
					m_GameSettings.SetWidth(event.resized.width);
					m_GameSettings.SetHeight(event.resized.height);
					m_GameSettings.Apply();
					OnResize();
				}
			}
		}

		Input::Update();
		m_Time.Update();
		m_Accumulator += (double)m_Time.DeltaTime();

		// Run Fixed Update
		while (m_Accumulator >= Time::FixedTimeStep())
		{
			m_Accumulator -= Time::FixedTimeStep();
			FixedUpdate();
		}

		Update(m_Time.DeltaTime());

		if (m_PauseRender == false)
		{
			m_GraphicsDevice->PresentBegin();
			m_ImGui.BeginDraw();
			Draw();
			OnGui();
			m_ImGui.OnGui();
			m_ImGui.EndDraw();
			m_GraphicsDevice->PresentEnd();
		}

		Input::Reset();
	}

	Exit();
	return;
}

void Game::Exit()
{
	ShutDown();
	if (m_VRManager)
	{
		m_VRManager->ShutDown();
	}
	m_ImGui.ShutDown();
	m_GraphicsDevice->ShutDown();
	m_GameSettings.SaveSettings();
	m_Window.Close();
	exit(0);
}

GameSettings* Game::GetGameSettings()
{
	return &m_GameSettings;
}

VR_Manager* Game::GetVRManager()
{
	return m_VRManager;
}
