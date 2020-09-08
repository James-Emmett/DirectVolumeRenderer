#include "VR/D3D/VR_Manager_D3D.h"
#include "System/Assert.h"
#include "Graphics/GraphicsDevice.h"
#include "Application/Application.h"
#include "Application/GameSettings.h"
#include <vector>

VR_Manager::VR_Manager()
{
}

VR_Manager::~VR_Manager()
{
	ShutDown();
}

bool VR_Manager::Initialize()
{
	XrResult result = XrResult::XR_SUCCESS;

	std::vector<const char*> active_exts;
	const char* request_exts[] = 
	{
		XR_KHR_D3D11_ENABLE_EXTENSION_NAME, 
		XR_EXT_DEBUG_UTILS_EXTENSION_NAME,
	};

	uint32_t ext_count = 0;
	xrEnumerateInstanceExtensionProperties(nullptr, 0, &ext_count, nullptr);
	std::vector<XrExtensionProperties> xr_exts(ext_count, { XR_TYPE_EXTENSION_PROPERTIES });
	xrEnumerateInstanceExtensionProperties(nullptr, ext_count, &ext_count, xr_exts.data());

	// Fill the active extensions up
	for (size_t i = 0; i < xr_exts.size(); i++)
	{
		for (int32_t j = 0; j < 2; j++) 
		{
			if (strcmp(request_exts[j], xr_exts[i].extensionName) == 0)
			{
				active_exts.push_back(request_exts[j]);
				break;
			}
		}
	}
	
	// Make sure allt he requests were satisfied
	if (active_exts.size() != 2)
	{
		assert(0 && "OpenXR Extension Requests were not satisfied");
	}

	// Pass active extensions to OpenXR
	XrInstanceCreateInfo createInfo				= { XR_TYPE_INSTANCE_CREATE_INFO };
	createInfo.enabledExtensionCount			= (uint32_t)active_exts.size();
	createInfo.enabledExtensionNames			= active_exts.data();
	createInfo.applicationInfo.apiVersion		= XR_CURRENT_API_VERSION;
	strcpy_s(createInfo.applicationInfo.applicationName, "Rune");

	// Create are OpenXR instance, gives acess to various OpenXR stuff.
	result = xrCreateInstance(&createInfo, &m_Instance);

	if (m_Instance == nullptr)
	{
		assert(0 && "Open VR Failed to Initialize");
		return false;
	}

	// Grab function pointers to extension pointers.
	xrGetInstanceProcAddr(m_Instance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)(&ext_xrCreateDebugUtilsMessengerEXT));
	xrGetInstanceProcAddr(m_Instance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)(&ext_xrDestroyDebugUtilsMessengerEXT));
	xrGetInstanceProcAddr(m_Instance, "xrGetD3D11GraphicsRequirementsKHR", (PFN_xrVoidFunction*)(&ext_xrGetD3D11GraphicsRequirementsKHR));

	// Request a form factor from the device (HMD, Handheld, etc.)
	XrSystemGetInfo systemInfo = { XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	result = xrGetSystem(m_Instance, &systemInfo, &m_SystemID);

	if (result != XrResult::XR_SUCCESS)
	{
		Application::Abort("Invalid System ID, likly headset not detected");
		return false;
	}

	// Get the device blend mode, mixed reality is semi transparent vs occulus opaque display etc?
	// im only using opaque atm!
	uint32_t blend_count = 0;
	xrEnumerateEnvironmentBlendModes(m_Instance, m_SystemID, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 1, &blend_count, &m_BlendMode);

	// Just like Occulus and OpenVr we need to use LUID to init graphics device, this is annoying but nessasery
	// means i must initialize graphics device here if using VR rather than in Game1.cpp :/
	XrGraphicsRequirementsD3D11KHR requirement = { XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR };
	result = ext_xrGetD3D11GraphicsRequirementsKHR(m_Instance, m_SystemID, &requirement);

	if (result != XrResult::XR_SUCCESS)
	{
		Application::Abort("Failed to get Adapter Luid");
		return false;
	}

	AdapterID adapterID(requirement.adapterLuid.LowPart, requirement.adapterLuid.HighPart);
	// Is this legal? assign to a const pointer outside of constructor?
	m_Adapter = GraphicsAdapter::GetAdapterFromLUID(adapterID);


	return true;
}

bool VR_Manager::CreateSession(GraphicsDevice* device)
{
	m_GraphicsDevice = device;
	XrResult result = XrResult::XR_SUCCESS;

	// Set D3D11 device for bindings
	XrGraphicsBindingD3D11KHR binding = { XR_TYPE_GRAPHICS_BINDING_D3D11_KHR };
	binding.device = m_GraphicsDevice->GetDevice();

	// Create session so we can submit frames?
	XrSessionCreateInfo sessionInfo = { XR_TYPE_SESSION_CREATE_INFO };
	sessionInfo.next = &binding;
	sessionInfo.systemId = m_SystemID;

	result = xrCreateSession(m_Instance, &sessionInfo, &m_Session);

	if (result != XrResult::XR_SUCCESS)
	{
		Application::Abort("Failed to create session");
		return false;
	}

	// Create Reference Frame Relative to the Guardian Boundary!
	XrReferenceSpaceCreateInfo ref_space = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
	ref_space.poseInReferenceSpace = m_PoseIdentity;
	ref_space.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
	result = xrCreateReferenceSpace(m_Session, &ref_space, &m_PlaySpace);

	if (result != XrResult::XR_SUCCESS)
	{
		Application::Abort("Failed to get reference space");
		return false;
	}

	// --Begin session--
	XrSessionBeginInfo sessionBeginInfo;
	sessionBeginInfo.type = XR_TYPE_SESSION_BEGIN_INFO;
	sessionBeginInfo.next = NULL;
	sessionBeginInfo.primaryViewConfigurationType = m_ConfigView;
	result = xrBeginSession(m_Session, &sessionBeginInfo);

	if (result != XrResult::XR_SUCCESS)
	{
		Application::Abort("Failed to get reference space");
		return false;
	}


	//--Create the swapChain--

	// Enumerate the views for rendering, gets how many views we need, 1 for AR 2 for stero (vr) etc.
	uint32_t viewCount = 0;
	xrEnumerateViewConfigurationViews(m_Instance, m_SystemID, m_ConfigView, 0, &viewCount, nullptr);
	m_ViewConfig.resize(viewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
	xrEnumerateViewConfigurationViews(m_Instance, m_SystemID, m_ConfigView, viewCount, &viewCount, m_ViewConfig.data());

	// Resize XrView as it holds pose info/FOV per view (eye) etc.
	m_Views.resize(viewCount, { XR_TYPE_VIEW });

	// Create a Swapchain (double buffer) for each view
	for (uint32_t i = 0; i < viewCount; i++)
	{
		XrSwapchain handle;
		XrViewConfigurationView& view = m_ViewConfig[i];
		XrSwapchainCreateInfo swapInfo = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
		swapInfo.arraySize		= 1;
		swapInfo.mipCount		= 1;
		swapInfo.faceCount		= 1;
		swapInfo.format			= DXGI_FORMAT_R8G8B8A8_UNORM; // Make this dynamic in the future.
		swapInfo.width			= view.recommendedImageRectWidth;
		swapInfo.height			= view.recommendedImageRectHeight;
		swapInfo.sampleCount	= view.recommendedSwapchainSampleCount;
		swapInfo.usageFlags		= XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
		
		result = xrCreateSwapchain(m_Session, &swapInfo, &handle);

		if (result != XrResult::XR_SUCCESS)
		{
			Application::Abort("Failed to Create swap chain for View.");
			return false;
		}

		m_Resolution.x = (float)view.recommendedImageRectWidth;
		m_Resolution.y = (float)view.recommendedImageRectHeight;

		// Get how many textures we need per view (eye)
		uint32_t surfaceCount = 0;
		xrEnumerateSwapchainImages(handle, 0, &surfaceCount, nullptr);

		// keep track of swapchain data
		XrSwapchainData swapChain = {};
		swapChain.m_Width	= swapInfo.width;
		swapChain.m_Height	= swapInfo.height;
		swapChain.m_Handle	= handle;
		swapChain.m_SurfaceImages.resize(surfaceCount, { XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR });
		swapChain.m_SurfaceViews.resize(surfaceCount);

		// get access to swapchain images
		result = xrEnumerateSwapchainImages(swapChain.m_Handle, surfaceCount, &surfaceCount, (XrSwapchainImageBaseHeader*)swapChain.m_SurfaceImages.data());

		if (result != XrResult::XR_SUCCESS)
		{
			Application::Abort("Failed to Enumerate swap chain Images.");
			return false;
		}

		// Create Render Target and Depth from Surface Images.
		for (uint32_t j = 0; j < surfaceCount; j++)
		{
			swapChain.m_SurfaceViews[j] = CreateSurfaceViews((XrBaseInStructure&)swapChain.m_SurfaceImages[j]);
		}

		m_SwapChains.push_back(swapChain);
	}

	return true;
}

const GraphicsAdapter* VR_Manager::GetAdapterForHMD()
{
	return m_Adapter;
}

void VR_Manager::BeginFrame()
{
	XrResult result = XR_SUCCESS;

	// wait untill frame is finished + grab the predicted time
	m_FrameState = { XR_TYPE_FRAME_STATE };
	xrWaitFrame(m_Session, nullptr, &m_FrameState);

	// start next session?
	xrBeginFrame(m_Session, nullptr);

	uint32_t viewProjCount = 0;
	// Is view State useful in any way?
	XrViewState viewState = { XR_TYPE_VIEW_STATE };
	// Describes the time and space info for aquiring vies/projections
	XrViewLocateInfo locateInfo = { XR_TYPE_VIEW_LOCATE_INFO };
	locateInfo.viewConfigurationType = m_ConfigView;
	locateInfo.displayTime = m_FrameState.predictedDisplayTime;
	locateInfo.space = m_PlaySpace;

	// Returns the view and projection info for a particular display time
	result = xrLocateViews(m_Session, &locateInfo, &viewState, (uint32_t)m_Views.size(), &viewProjCount, m_Views.data());

	// Set up the projection view struct for each eye using optianed data
	m_ProjectionViews.resize(viewProjCount);
	m_ProjectionMatrix.resize(viewProjCount);
	m_ViewMatrix.resize(viewProjCount);
	m_CurrentImageID.resize(viewProjCount);

	for (size_t i = 0; i < viewProjCount; i++)
	{
		// Get the image index for this views swapchain, can have double or tripple buffering
		// so we must ask which one the HMD expects next.
		XrSwapchainImageAcquireInfo acquire_info = { XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
		result = xrAcquireSwapchainImage(m_SwapChains[i].m_Handle, &acquire_info, &m_CurrentImageID[i]);

		// we must wait encase the buffer is being read from before we can utilise it for this frame.
		XrSwapchainImageWaitInfo wait_info = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
		wait_info.timeout = XR_INFINITE_DURATION;
		result = xrWaitSwapchainImage(m_SwapChains[i].m_Handle, &wait_info);

		// Set up our rendering information for the viewpoint we're using right now!
		m_ProjectionViews[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
		m_ProjectionViews[i].pose = m_Views[i].pose;
		m_ProjectionViews[i].fov = m_Views[i].fov;
		m_ProjectionViews[i].subImage.swapchain = m_SwapChains[i].m_Handle;
		m_ProjectionViews[i].subImage.imageRect.offset = { 0, 0 };
		m_ProjectionViews[i].subImage.imageRect.extent = { m_SwapChains[i].m_Width, m_SwapChains[i].m_Height };

		//--Create Projection--
		// Opposite = tan(angle) * adjacent
		float left	= 0.01f * tanf(m_Views[i].fov.angleLeft);
		float right = 0.01f * tanf(m_Views[i].fov.angleRight);
		float down	= 0.01f * tanf(m_Views[i].fov.angleDown);
		float up	= 0.01f * tanf(m_Views[i].fov.angleUp);
		m_ProjectionMatrix[i] = Matrix4::PerspectiveOffCenter(left, right, up, down, 0.01f, 1000.0f);//Matrix4::PerspectiveFov(m_Views[i].fov.angleUp * 2, m_Resolution.x / m_Resolution.y, 0.01f, 100.0f);

		//--Create View--
		XrView& view = m_Views[i];
		XrVector3f& xrPos = view.pose.position;
		XrQuaternionf& xrQuat = view.pose.orientation;

		Quaternion quat = Quaternion(xrQuat.x, xrQuat.y, -xrQuat.z, xrQuat.w);
		Vector3 pos = Vector3(xrPos.x, xrPos.y, -xrPos.z);
		m_ViewMatrix[i] = Matrix4::Inverse(Matrix4::Rotate(quat) * Matrix4::Translate(pos));
	}

	// Get Hand Locations
}

void VR_Manager::BindTarget(Eye eye)
{
	// Get correct surface for this eye.
	XrSurfaceData* surface = &m_SwapChains[(Uint32)eye].m_SurfaceViews[m_CurrentImageID[(Uint32)eye]];
	ID3D11DeviceContext* context = m_GraphicsDevice->GetImmediateContext();

	// get the view rext from projection view structure
	XrRect2Di& rect = m_ProjectionViews[(Uint32)eye].subImage.imageRect;
	D3D11_VIEWPORT viewport = CD3D11_VIEWPORT((float)rect.offset.x, (float)rect.offset.y, (float)rect.extent.width, (float)rect.extent.height);
	context->RSSetViewports(1, &viewport);

	float clear[] = { 0, 0, 0, 1 };
	context->ClearRenderTargetView(surface->m_RenderTarget, clear);
	context->ClearDepthStencilView(surface->m_DepthTarget, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, &surface->m_RenderTarget, surface->m_DepthTarget);
}

void VR_Manager::EndFrame()
{
	for (size_t i = 0; i < m_ProjectionViews.size(); i++)
	{
		// And tell OpenXR we're done with rendering to this one!
		XrSwapchainImageReleaseInfo release_info = { XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
		xrReleaseSwapchainImage(m_SwapChains[i].m_Handle, &release_info);
	}

	// We need to compose the projections/views in this structure to submit for drawing?
	XrCompositionLayerProjection  layerProj = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
	XrCompositionLayerBaseHeader* layer = (XrCompositionLayerBaseHeader*)&layerProj;

	layerProj.space = m_PlaySpace;
	layerProj.viewCount = (uint32_t)m_ProjectionViews.size();
	layerProj.views = m_ProjectionViews.data();

	// We're finished with rendering our layer, so send it off for display!
	XrFrameEndInfo endInfo{ XR_TYPE_FRAME_END_INFO };
	endInfo.displayTime = m_FrameState.predictedDisplayTime;
	endInfo.environmentBlendMode = m_BlendMode;
	endInfo.layerCount = layer == nullptr ? 0 : 1;
	endInfo.layers = &layer;
	xrEndFrame(m_Session, &endInfo);
}

Matrix4 VR_Manager::GetView(Eye eye, Matrix4 parent)
{
	return m_ViewMatrix[(Uint32)eye] * Matrix4::Inverse(parent);
}

Matrix4 VR_Manager::GetProjection(Eye eye)
{
	return m_ProjectionMatrix[(Uint32)eye];
}

Vector2 VR_Manager::GetResolution()
{
	return m_Resolution;
}

void VR_Manager::ShutDown()
{
	for (size_t i = 0; i < m_SwapChains.size(); i++)
	{
		m_SwapChains[i].Release();
		xrDestroySwapchain(m_SwapChains[i].m_Handle);
	}

	m_SwapChains.clear();

	// Shut all aquired resources down
	if (m_PlaySpace != XR_NULL_HANDLE)
	{
		xrDestroySpace(m_PlaySpace);
	}

	if (m_Session != XR_NULL_HANDLE)
	{
		xrDestroySession(m_Session);
	}

	if (m_Instance != XR_NULL_HANDLE)
	{
		xrDestroyInstance(m_Instance);
	}
}

XrSurfaceData VR_Manager::CreateSurfaceViews(XrBaseInStructure& swapImage)
{
	ID3D11Device* device = m_GraphicsDevice->GetDevice();
	XrSurfaceData surfaceData = {};

	// Grab d3d info from the Xr swapchain image
	XrSwapchainImageD3D11KHR& d3dSwapChainImage = (XrSwapchainImageD3D11KHR&)swapImage;
	D3D11_TEXTURE2D_DESC texDesc;
	d3dSwapChainImage.texture->GetDesc(&texDesc);

	// Create render target view too surface image
	D3D11_RENDER_TARGET_VIEW_DESC targetDesc = {};
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	device->CreateRenderTargetView(d3dSwapChainImage.texture, &targetDesc, &surfaceData.m_RenderTarget);

	// create depth texture too match!
	ID3D11Texture2D* depthTexture;
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.SampleDesc.Count = 1;
	depthDesc.MipLevels	= 1;
	depthDesc.Width		= texDesc.Width;
	depthDesc.Height	= texDesc.Height;
	depthDesc.ArraySize	= texDesc.ArraySize;
	depthDesc.Format	= DXGI_FORMAT_R32_TYPELESS;
	depthDesc.BindFlags	= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	device->CreateTexture2D(&depthDesc, nullptr, &depthTexture);

	assert(depthTexture != nullptr);

	// create a view to the depth texture
	D3D11_DEPTH_STENCIL_VIEW_DESC stencilDesc = {};
	stencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	stencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	device->CreateDepthStencilView(depthTexture, &stencilDesc, &surfaceData.m_DepthTarget);

	// view holds ref to this so we dont need too!
	depthTexture->Release();

	return surfaceData;
}
