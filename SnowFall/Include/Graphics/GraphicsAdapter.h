#pragma once
#include "System/Types.h"
#include "Graphics/Graphics.h"

enum class GpuVender
{
	Unkown = 0,
	Nvidia = 0x10DE,
	AMD = 0x1002 | 0x1022
};

struct DisplayMode
{
	Uint32			m_Width = 0;
	Uint32			m_Height = 0;
	Uint32			m_Denominator = 0;
	Uint32			m_Numerator = 0;
	float			m_AspectRatio = 0;
	SurfaceFormat	m_Format = SurfaceFormat::R8G8B8A8_Unorm;
	bool			m_Stero = false; // HMD

	bool operator==(const DisplayMode& mode)const;
	bool operator!=(const DisplayMode& mode)const;
	bool operator<(const DisplayMode& mode)const;
	bool operator>(const DisplayMode& mode)const;
	bool operator<=(const DisplayMode& mode)const;
	bool operator>=(const DisplayMode& mode)const;
};

class GraphicsAdapter
{
private:
	static std::vector<GraphicsAdapter> g_AdapterList;

private:
	std::string	m_DeviceName;
	GpuVender	m_Vender = GpuVender::Unkown;
	Uint64		m_Ram = 0;
	Uint64		m_VRam = 0;
	AdapterID   m_AdapterID;	// For HMD
	std::vector<DisplayMode> m_DisplayModes;

public:
	std::string	DeviceName()const;
	GpuVender	Vender()const;
	Uint64		Ram()const;
	Uint64		VRam()const;
	AdapterID	ID()const;
	DisplayMode GetDefaultMode()const;
	const std::vector<DisplayMode>& DisplayModes()const;
	bool IsModeValid(const DisplayMode& mode)const;

	static const GraphicsAdapter* GetAdapterFromLUID(AdapterID& id);
	static const GraphicsAdapter* GetDefaultAdapter();
	static const std::vector<GraphicsAdapter>* GetAdapterList();
};