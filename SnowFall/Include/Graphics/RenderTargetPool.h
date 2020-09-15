#pragma once
#include "Application/Application.h"
#include "Graphics/GraphicsDevice.h"

// Convinet struct too store data about target
// mayeb i need a render target class by default in the engine?
struct RenderTargetTemp
{
	GraphicsDevice* m_GraphicsDevice = nullptr;
	RenderHandle m_RenderTarget = RenderHandle();
	Uint32 m_Width;
	Uint32 m_Height;
	RenderFormat m_Format;
	bool m_Active = false;

public:
	RenderTargetTemp(GraphicsDevice* device, Uint32 width, Uint32 height, RenderFormat format)
	{
		assert(device && "null device passed to RednerTargetTemp");

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_GraphicsDevice = device;
		m_RenderTarget = m_GraphicsDevice->CreateRenderTarget(width, height, format);
	}

	bool IsMatch(Uint32 width, Uint32 height, RenderFormat format)
	{
		if (m_Active) { return false; }
		return (m_Width == width) && (m_Height == height) && (m_Format == format);
	}

	void Release()
	{
		m_GraphicsDevice->DestroyTexture(m_RenderTarget);
	}
};

class RenderTargetPool
{
	static std::vector<RenderTargetTemp> s_TempoaryRenderTarget;
public:
	static void ShutDown()
	{
		for (size_t i = 0; i < s_TempoaryRenderTarget.size(); i++)
		{
			s_TempoaryRenderTarget[i].Release();
		}
		s_TempoaryRenderTarget.clear();
	}

	static RenderHandle GetTempoary(Uint32 width, Uint32 height, RenderFormat format = RenderFormat::R16G16B16A16_Float)
	{
		for (size_t i = 0; i < s_TempoaryRenderTarget.size(); ++i)
		{
			if (s_TempoaryRenderTarget[i].IsMatch(width, height, format))
			{
				s_TempoaryRenderTarget[i].m_Active = true;
				return s_TempoaryRenderTarget[i].m_RenderTarget;
			}
		}

		// Failed to find one if you get here, so create one
		RenderTargetTemp target(Application::graphicsDevice, width, height, format);
		target.m_Active = true;
		s_TempoaryRenderTarget.push_back(target);

		return target.m_RenderTarget;
	}

	static void ReleaseTempoary(RenderHandle handle)
	{
		for (size_t i = 0; i < s_TempoaryRenderTarget.size(); ++i)
		{
			if (s_TempoaryRenderTarget[i].m_RenderTarget == handle)
			{
				s_TempoaryRenderTarget[i].m_Active = false;
			}
		}
	}
};