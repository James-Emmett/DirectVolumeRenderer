#pragma once
#include "GameSettings.h"
#include "Graphics/GraphicsDevice.h"
#include "System/Logger.h"
#include "System/Window.h"
#include "System/Time.h"
#include "VR/VR_Manager.h"
#include "UI/ImGui_Interface.h"
#include "Input/Input.h"
#include "Content/ContentManager.h"

class Game
{
public:
	Window			m_Window;
	GraphicsDevice* m_GraphicsDevice = nullptr;
	ContentManager* m_ContentManager;
	GameSettings    m_GameSettings;
	Time			m_Time;

private:
	LogFile			m_LogFile;
	LogMessageBox	m_LogMessageBox;
	VR_Manager*		m_VRManager = nullptr;
	ImGui_Interface	m_ImGui;
	double			m_Accumulator = 0.0f;
	bool m_PauseRender = false;

public:
	void Run();
	void Exit();
	GameSettings* GetGameSettings();
	VR_Manager*	  GetVRManager();

public:
	virtual void Initialize() = 0;
	virtual void LoadContent() = 0;
	virtual void FixedUpdate() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void OnGui() = 0;
	virtual void OnResize() = 0;
	virtual void ShutDown() = 0;
};