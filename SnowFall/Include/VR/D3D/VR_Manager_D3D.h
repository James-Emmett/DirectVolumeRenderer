// Note:
/*
	OpenXR is initial a decent API however the handling of view aquisition or
	sensor positioning is very tied to rendering. This is required due to the predictive
	timing but makes implementing OpenXR into a traditional pipline a bit annoying.
*/

#pragma once
#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11
#include <d3d11.h>

#include "OpenXR/openxr.h"
#include "OpenXR/openxr_platform.h"
#include "Math/Matrix4.h"
#include <vector>

struct XrSurfaceData
{
	ID3D11DepthStencilView* m_DepthTarget = nullptr;
	ID3D11RenderTargetView* m_RenderTarget = nullptr;

	void Release()
	{
		if (m_DepthTarget != nullptr)
		{
			m_DepthTarget->Release();
			m_DepthTarget = nullptr;
		}

		if (m_RenderTarget != nullptr)
		{
			m_RenderTarget->Release();
			m_RenderTarget = nullptr;
		}
	}
};

struct XrSwapchainData
{
	XrSwapchain m_Handle;
	int32_t     m_Width;
	int32_t     m_Height;
	std::vector<XrSwapchainImageD3D11KHR> m_SurfaceImages;
	std::vector<XrSurfaceData> m_SurfaceViews;

	void Release()
	{
		for (uint32_t i = 0; i < m_SurfaceImages.size(); i++)
		{
			// I have to do this?
			m_SurfaceImages[i].texture->Release();
		}

		for (uint32_t i = 0; i < m_SurfaceViews.size(); i++)
		{
			m_SurfaceViews[i].Release();
		}
	}
};


enum class Eye
{
	Left,
	Right
};


class GraphicsDevice;
class GraphicsAdapter;
class VR_Manager
{
private:
	GraphicsDevice*			m_GraphicsDevice = nullptr;
	const GraphicsAdapter*	m_Adapter = nullptr;

	XrViewConfigurationType m_ConfigView = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	XrInstance				m_Instance = nullptr;
	XrSession				m_Session = XrSession();
	XrSystemId				m_SystemID = XR_NULL_SYSTEM_ID;
	XrEnvironmentBlendMode	m_BlendMode = {};
	XrPosef					m_PoseIdentity = { {0,0,0,1}, {0,0,0} };
	XrSpace					m_PlaySpace = {}; // Local or Stage usally
	XrFrameState			m_FrameState = XrFrameState();
	Vector2					m_Resolution;

	// Viws for eyes
	std::vector<XrView>	m_Views; // Contains view pose and projection state for creating projection 
	std::vector<XrViewConfigurationView> m_ViewConfig; // Recommended settings for each view
	std::vector<XrCompositionLayerProjectionView> m_ProjectionViews;
	std::vector<Matrix4> m_ProjectionMatrix;
	std::vector<Matrix4> m_ViewMatrix;

	// Contains the surface info from the views on the HMD
	std::vector<XrSwapchainData> m_SwapChains;
	std::vector<uint32_t> m_CurrentImageID; // Each view has multiple buffers which one do we need now?

	// OpenXR Function Pointers.
	PFN_xrGetD3D11GraphicsRequirementsKHR ext_xrGetD3D11GraphicsRequirementsKHR = nullptr;
	PFN_xrCreateDebugUtilsMessengerEXT    ext_xrCreateDebugUtilsMessengerEXT = nullptr;
	PFN_xrDestroyDebugUtilsMessengerEXT   ext_xrDestroyDebugUtilsMessengerEXT = nullptr;

public:
	VR_Manager();
	~VR_Manager();

public:
	// Initialize OpenXR, loads runtime, checks head set and extensions
	bool Initialize();
	// Creates the session, hooks up graphics device to OpenXR creats swap chains for views
	bool CreateSession(GraphicsDevice* device);
	// Returns the desired adapter, i.e dedicated GPU not Intergrated on Laptop.
	const GraphicsAdapter* GetAdapterForHMD();
	// Updates tracking information and waits for last frame to render
	void BeginFrame();
	// Bind a specific eyes render target then do your render stuff
	void BindTarget(Eye eye);
	// Submit rendererd frames to HMD
	void EndFrame();
	// Return the view for eye relative to a parent (virtual camera, for snap or teleporting etc...)
	Matrix4 GetView(Eye eye, Matrix4 parent);
	// Return the projection matrix for this eye
	Matrix4 GetProjection(Eye eye);
	// Gets the resolution of the vr
	Vector2 GetResolution();
	// Shut down all resource OpenXR is using.
	void ShutDown();


private:
	// Create d3d view objects for binding (Render and Depth)
	XrSurfaceData CreateSurfaceViews(XrBaseInStructure& swapImage);
};