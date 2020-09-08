#pragma once
#include "System/ConfigFile.h"

class Game;
struct GraphicsParameters;
class GameSettings
{
protected:
	ConfigFile	m_ConfigFile;
	std::string m_AssetPath = "Assets/";
	Uint32		m_Width = 1280;
	Uint32		m_Height = 720;
	float		m_FixedTimeStep = 0.013333f;
	float		m_TimeScale = 1.00000f;
	bool		m_IsVsync = true;
	bool		m_IsFullScreen = false;
	bool		m_IsStero = false;
	bool		m_RequiresSave = true;
	bool		m_RequiresRestart = false;

public:
	std::string GetAssetPath()const;
	void		SetAssetPath(std::string path);
	Uint32		GetWidth()const;
	void		SetWidth(Uint32 width);
	Uint32		GetHeight()const;
	void		SetHeight(Uint32 height);
	float		GetFixedTimeStep()const;
	void		SetFixedTimeStep(float timeStep);
	float		GetTimeScale()const;
	void		SetTimeScale(float timeScale);
	bool		GetIsVsync()const;
	void		SetVsync(bool vsync);
	bool		GetIsFullScreen()const;
	void		SetFullScreen(bool fullscreen);
	bool		GetIsStero()const;
	void		SetStero(bool stero);

	void Apply();
	void SaveSettings();
	void LoadSettings();
	GraphicsParameters PrepareParameters()const;
};