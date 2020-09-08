//Note:
/*
	Like Unity, modify settings here to change how a scene is rendered!
*/

#pragma once
#include "World/Renderer/SkyBox.h"

enum class FogMode { Linear, Exponential, ExponentialSquared };

struct RenderSettings
{
	SkyBox	m_SkyBox;
	Color	m_AmbientLight = Color::White;
	bool	m_FogEnabled = false;
	FogMode m_FogMode = FogMode::Linear;
	Color	m_FogColor = Color::White;
	float	m_FogDensity = 0.05f;
	float	m_FogStart = 10;
	float	m_FogEnd = 150;

	RenderSettings() {}
	RenderSettings(const RenderSettings& settings)
	{
		m_SkyBox = settings.m_SkyBox;
		m_AmbientLight = settings.m_AmbientLight;
		m_FogEnabled = settings.m_FogEnabled;
		m_FogMode = settings.m_FogMode;
		m_FogColor = settings.m_FogColor;
		m_FogDensity = settings.m_FogDensity;
		m_FogStart = settings.m_FogStart;
		m_FogEnd = settings.m_FogEnd;
	}
};