#include "Application/GameSettings.h"
#include "Application/Application.h"
#include "Graphics/GraphicsDevice.h"
#include "System/ConfigFile.h"
#include "Application/Game.h"

std::string GameSettings::GetAssetPath()const
{
	return m_AssetPath;
}

void GameSettings::SetAssetPath(std::string path)
{
	m_AssetPath = path;
}

Uint32 GameSettings::GetWidth()const
{
	return m_Width;
}

void GameSettings::SetWidth(Uint32 width)
{
	if (width != m_Width)
	{
		m_Width = width;
		m_RequiresSave = true;
	}
}

Uint32 GameSettings::GetHeight()const
{
	return m_Height;
}

void GameSettings::SetHeight(Uint32 height)
{
	if (height != m_Height)
	{
		m_Height = height;
		m_RequiresSave = true;
	}
}

float GameSettings::GetFixedTimeStep()const
{
	return m_FixedTimeStep;
}

void GameSettings::SetFixedTimeStep(float timeStep)
{
	if (m_FixedTimeStep != timeStep)
	{
		m_FixedTimeStep = timeStep;
		m_RequiresSave = true;
	}
}

float GameSettings::GetTimeScale()const
{
	return m_TimeScale;
}

void GameSettings::SetTimeScale(float timeScale)
{
	if (m_TimeScale != timeScale)
	{
		m_TimeScale = timeScale;
		m_RequiresSave = true;
	}
}

bool GameSettings::GetIsVsync()const
{
	return m_IsVsync;
}

void GameSettings::SetVsync(bool vsync)
{
	if (m_IsVsync != vsync)
	{
		m_IsVsync = vsync;
		m_RequiresSave = true;
	}
}

bool GameSettings::GetIsFullScreen()const
{
	return m_IsFullScreen;
}

void GameSettings::SetFullScreen(bool fullscreen)
{
	if (m_IsFullScreen != fullscreen)
	{
		m_IsFullScreen = fullscreen;
		Application::game->m_Window.SetFullScreen(fullscreen); // easy to do it ehre
		m_RequiresSave = true;
	}
}

bool GameSettings::GetIsStero() const
{
	return m_IsStero;
}

void GameSettings::SetStero(bool stero)
{
	if (m_IsStero != stero)
	{
		m_IsStero = stero;
		m_RequiresSave = true;
		m_RequiresRestart = true;
	}
}

bool GameSettings::IsVRS() const
{
	return m_IsVairbleRateShading;
}

void GameSettings::Apply()
{
	if (m_RequiresSave)
	{
		Game* game = Application::game;
		game->m_Window.SetSize(Vector2((float)m_Width, (float)m_Height));
		game->m_GraphicsDevice->Reset(PrepareParameters());
		game->m_Time.SetFixedTimeStep(m_FixedTimeStep);
		game->m_Time.SetTimeScale(m_TimeScale);
		game->m_Window.InvalidateWindow();
		SaveSettings();
	}

	if (m_RequiresRestart)
	{
		// Boot the Reboot.exe;
		// close this
	}
}

void GameSettings::SaveSettings()
{
	// Update all the settings.
	m_ConfigFile.SetString("Common", "AssetPath", m_AssetPath);
	m_ConfigFile.SetInt("Graphics", "Width", m_Width);
	m_ConfigFile.SetInt("Graphics", "Height", m_Height);
	m_ConfigFile.SetBool("Graphics", "FullScreen", m_IsFullScreen);
	m_ConfigFile.SetBool("Graphics", "Vsync", m_IsVsync);
	m_ConfigFile.SetBool("Graphics", "Stero", m_IsStero);
	m_ConfigFile.SetBool("Graphics", "VRS", m_IsVairbleRateShading);
	m_ConfigFile.SetFloat("Time", "FixedTimeStep", 0.01333333333f);
	m_ConfigFile.SetFloat("Time", "TimeScale", 1);
	m_ConfigFile.Save();
	m_RequiresSave = false;
}

void GameSettings::LoadSettings()
{
	m_ConfigFile.Open("Assets/settings.cfg");
	m_AssetPath = m_ConfigFile.GetString("Common", "AssetPath", "Assets/");
	m_Width = m_ConfigFile.GetInt("Graphics", "Width", 1280);
	m_Height = m_ConfigFile.GetInt("Graphics", "Height", 720);
	m_IsFullScreen = m_ConfigFile.GetBool("Graphics", "FullScreen", false);
	m_IsVsync = m_ConfigFile.GetBool("Graphics", "Vsync", true);
	m_IsStero = m_ConfigFile.GetBool("Graphics", "Stero", false);
	m_IsVairbleRateShading = m_ConfigFile.GetBool("Graphics", "VRS", false);
	m_FixedTimeStep = m_ConfigFile.GetFloat("Time", "FixedTimeStep", 0.01333333333f);
	m_TimeScale = m_ConfigFile.GetFloat("Time", "TimeScale", 1);
	m_RequiresSave = true;
}

GraphicsParameters GameSettings::PrepareParameters() const
{
	GraphicsParameters parameters;
	parameters.Width = m_Width;
	parameters.Height = m_Height;
	parameters.Fullscreen = m_IsFullScreen;
	parameters.Stero = m_IsStero;
	parameters.Vsync.m_Enabled = m_IsVsync;
	parameters.Handle = Application::game->m_Window.GetHandle();

	return parameters;
}
