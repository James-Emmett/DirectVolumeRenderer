//----------------------------------------------------------------------------------
// File:        NvFoveatedRenderingHelper.cpp
// SDK Version: 1.0
// Email:       vrsupport@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

#include "VR/FoveatedRenderHelper.h"
#include "Graphics/GraphicsDevice.h"
#include "Application/Application.h"
#include "Application/GameSettings.h"
#include "System/Logger.h"

std::string FoveatedRenderHelper::s_ShaderPerformanceTable[6] =
{
    "Highest_Performance",
    "High_Performance",
    "Balanced",
    "High_Quality",
    "Highest_Quality",
    "Custom"
};


std::string FoveatedRenderHelper::s_ShaderRateTable[12] =
{
    "Pixel_X0_Cull_Raster_Pixels",
    "Pixel_X16_Per_Raster_Pixel",
    "Pixel_X8_Per_Raster_Pixels",
    "Pixel_X4_Per_Raster_Pixels",
    "Pixel_X2_Per_Raster_Pixels",
    "Pixel_X1_Per_Raster_Pixels",
    "Pixel_X1_Per_2X1_Raster_Pixels",
    "Pixel_X1_Per_1X2_Raster_Pixels",
    "Pixel_X1_Per_2X2_Raster_Pixels",
    "Pixel_X1_Per_4X2_Raster_Pixels",
    "Pixel_X1_Per_2X4_Raster_Pixels",
    "Pixel_X1_Per_4X4_Raster_Pixels"
};


FoveatedRenderHelper::FoveatedRenderHelper()
{
    m_initialized = false;
    m_pVRSHelper = NULL;
    m_pGazeHandler = NULL;
    m_gazeTimestamp = 0;
    m_requestGetShadingRateResourceOnNextEnable = false;
    m_pShadingRateResource = NULL;
    m_pSRRShaderResourceView = NULL;
    m_eyeTrackingMode = FoveatedRenderEyeTracking::None;
    m_foveatedRenderingSupport = FoveatedRenderSupport::Unkown;

    ZeroMemory(&m_enableParams, sizeof(NV_VRS_HELPER_ENABLE_PARAMS));
    m_enableParams.version = NV_VRS_HELPER_ENABLE_PARAMS_VER;
    m_enableParams.ContentType = NV_VRS_CONTENT_TYPE_FOVEATED_RENDERING;
    m_enableParams.sFoveatedRenderingDesc.version = NV_FOVEATED_RENDERING_DESC_VER;
    m_enableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.version = NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_VER;
    m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.version = NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_VER;

    ZeroMemory(&m_gazeDataParams, sizeof(NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS));
    m_gazeDataParams.version = NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS_VER;
}

FoveatedRenderHelper::~FoveatedRenderHelper()
{
}

bool FoveatedRenderHelper::Initialize(GraphicsDevice* device)
{
    if (m_initialized) { return true; }

    m_GraphicsDevice = device;

    // Pattern preset
    m_enableParams.sFoveatedRenderingDesc.FoveationPatternPreset = NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_BALANCED;

    // Set defaults for the custom preset
    m_enableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fInnermostRadii[0] = 0.15f;
    m_enableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fInnermostRadii[1] = 0.1f;
    m_enableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fMiddleRadii[0] = 0.25f;
    m_enableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fMiddleRadii[1] = 0.2f;
    m_enableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fPeripheralRadii[0] = 0.35f;
    m_enableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fPeripheralRadii[1] = 0.3f;

    // Defaults for shading presets
    m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.InnerMostRegionShadingRate  = (NV_PIXEL_SHADING_RATE)Application::gameSettings->GetInnerRegionRate();
    m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.MiddleRegionShadingRate     = (NV_PIXEL_SHADING_RATE)Application::gameSettings->GetMiddleRegionRate();
    m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.PeripheralRegionShadingRate = (NV_PIXEL_SHADING_RATE)Application::gameSettings->GetOuterRegionRate();

    SetShadingRatePerformance(Application::gameSettings->GetShadingRatePerformance());

    static NvAPI_Status NvStatus = NVAPI_OK;
    static bool bNvFoveatedRenderingUnsupported = false;
    bool bVRSHelperInitialized = false;
    bool bGazeHandlerInitialized = false;

    if (m_foveatedRenderingSupport != FoveatedRenderSupport::Unkown)
    {
        return false;
    }

    NV_VRS_HELPER_INIT_PARAMS vrsHelperInitParams;
    ZeroMemory(&vrsHelperInitParams, sizeof(NV_VRS_HELPER_INIT_PARAMS));
    vrsHelperInitParams.version = NV_VRS_HELPER_INIT_PARAMS_VER;
    vrsHelperInitParams.ppVRSHelper = &m_pVRSHelper;
    NvStatus = NvAPI_D3D_InitializeVRSHelper(m_GraphicsDevice->GetDevice(), &vrsHelperInitParams);
    if (NvStatus != NVAPI_OK)
    {
        LogError("VRS Helper is not supported on this setup.");
        bVRSHelperInitialized = false;
    }
    else
    {
        bVRSHelperInitialized = true;
    }

    NV_GAZE_HANDLER_INIT_PARAMS gazeHandlerInitParams;
    ZeroMemory(&gazeHandlerInitParams, sizeof(NV_GAZE_HANDLER_INIT_PARAMS));
    gazeHandlerInitParams.version = NV_GAZE_HANDLER_INIT_PARAMS_VER;

    gazeHandlerInitParams.GazeDataDeviceId = 0;
    gazeHandlerInitParams.GazeDataType = NV_GAZE_DATA_STEREO;
    gazeHandlerInitParams.fHorizontalFOV = 110.0;
    gazeHandlerInitParams.fVericalFOV = 110.0;

    gazeHandlerInitParams.ppNvGazeHandler = &m_pGazeHandler;

    NvStatus = NvAPI_D3D_InitializeNvGazeHandler(m_GraphicsDevice->GetDevice(), &gazeHandlerInitParams);
    if (NvStatus != NVAPI_OK)
    {
        LogError("Foveated Rendering Gaze Data Handler is not supported on this setup.");
        bGazeHandlerInitialized = false;
    }
    else
    {
        bGazeHandlerInitialized = true;
    }

    NvAPI_D3D11_SetNvShaderExtnSlot(m_GraphicsDevice->GetDevice(), NV_SHADER_EXTN_SLOT_NUMBER);

    if (bVRSHelperInitialized && bGazeHandlerInitialized)
    {
        m_initialized = true;
        m_foveatedRenderingSupport = FoveatedRenderSupport::Supported;
    }
    else
    {
        // Foveated Rendering Initialization failed
        m_initialized = false;
        m_foveatedRenderingSupport = FoveatedRenderSupport::Not_Supported;
    }

    return m_initialized;
}

void FoveatedRenderHelper::LatchGazeData()
{
    NvAPI_Status NvStatus = NVAPI_OK;

    if (m_initialized && m_pVRSHelper)
    {
        NV_VRS_HELPER_LATCH_GAZE_PARAMS latchGazeParams;
        ZeroMemory(&latchGazeParams, sizeof(NV_VRS_HELPER_LATCH_GAZE_PARAMS));
        latchGazeParams.version = NV_VRS_HELPER_LATCH_GAZE_PARAMS_VER;
        NvStatus = m_pVRSHelper->LatchGaze(m_GraphicsDevice->GetImmediateContext(), &latchGazeParams);
    }
}

void FoveatedRenderHelper::EnableFoveatedRendering(NV_VRS_RENDER_MODE renderMode)
{

    NvAPI_Status NvStatus = NVAPI_OK;

    if (m_initialized && m_pVRSHelper)
    {
        // Update with settings
        m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.InnerMostRegionShadingRate = (NV_PIXEL_SHADING_RATE)Application::gameSettings->GetInnerRegionRate();
        m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.MiddleRegionShadingRate = (NV_PIXEL_SHADING_RATE)Application::gameSettings->GetMiddleRegionRate();
        m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.PeripheralRegionShadingRate = (NV_PIXEL_SHADING_RATE)Application::gameSettings->GetOuterRegionRate();

        SetShadingRatePerformance(Application::gameSettings->GetShadingRatePerformance());

        m_enableParams.RenderMode = renderMode;
        NvStatus = m_pVRSHelper->Enable(m_GraphicsDevice->GetImmediateContext(), &m_enableParams);
    }
}

void FoveatedRenderHelper::DisableFoveatedRendering()
{
    NvAPI_Status NvStatus = NVAPI_OK;

    if (m_initialized && m_pVRSHelper)
    {
        NV_VRS_HELPER_DISABLE_PARAMS disableParams;
        ZeroMemory(&disableParams, sizeof(NV_VRS_HELPER_DISABLE_PARAMS));
        disableParams.version = NV_VRS_HELPER_DISABLE_PARAMS_VER;
        NvStatus = m_pVRSHelper->Disable(m_GraphicsDevice->GetImmediateContext(), &disableParams);
    }
}

void FoveatedRenderHelper::UpdateGazeData(float GazeNormX, float GazeNormY, bool bStereoSplit)
{
    NvAPI_Status NvStatus = NVAPI_OK;

    if (m_initialized && m_pGazeHandler)
    {

        static bool bUpdatedInitialGazeOnce = false;

        switch (m_eyeTrackingMode)
        {
        case FoveatedRenderEyeTracking::None:
            if (!bUpdatedInitialGazeOnce)
            {
                m_gazeTimestamp++;
                m_gazeDataParams.Timestamp = m_gazeTimestamp;         // Timestamp at which the gaze data has been captured
                                                                      // Needs to be greater than previous timestamp so as to update the shading rate pattern.

                m_gazeDataParams.sStereoData.sLeftEye.version = NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER;
                m_gazeDataParams.sStereoData.sLeftEye.fGazeNormalizedLocation[0] = 0.0f;
                m_gazeDataParams.sStereoData.sLeftEye.fGazeNormalizedLocation[1] = 0.0f;
                m_gazeDataParams.sStereoData.sLeftEye.GazeDataValidityFlags = NV_GAZE_LOCATION_VALID;

                m_gazeDataParams.sStereoData.sRightEye.version = NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER;
                m_gazeDataParams.sStereoData.sRightEye.fGazeNormalizedLocation[0] = 0.0f;
                m_gazeDataParams.sStereoData.sRightEye.fGazeNormalizedLocation[1] = 0.0f;
                m_gazeDataParams.sStereoData.sRightEye.GazeDataValidityFlags = NV_GAZE_LOCATION_VALID;

                NvStatus = m_pGazeHandler->UpdateGazeData(m_GraphicsDevice->GetImmediateContext(), &m_gazeDataParams);

                bUpdatedInitialGazeOnce = true;
            }
            break;

        case FoveatedRenderEyeTracking::Mouse_Cursor:

            m_gazeTimestamp++;
            m_gazeDataParams.Timestamp = m_gazeTimestamp;             // Timestamp at which the gaze data has been captured
                                                                      // Needs to be greater than previous timestamp so as to update the shading rate pattern.

            if (bStereoSplit)
            {
                m_gazeDataParams.sStereoData.sLeftEye.version = NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER;
                m_gazeDataParams.sStereoData.sLeftEye.fGazeNormalizedLocation[0] = (float)(((GazeNormX < 0) ? GazeNormX + 0.5 : GazeNormX - 0.5));
                m_gazeDataParams.sStereoData.sLeftEye.fGazeNormalizedLocation[1] = (float)(GazeNormY / 2.0);
                m_gazeDataParams.sStereoData.sLeftEye.GazeDataValidityFlags = NV_GAZE_LOCATION_VALID;

                m_gazeDataParams.sStereoData.sRightEye.version = NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER;
                m_gazeDataParams.sStereoData.sRightEye.fGazeNormalizedLocation[0] = (float)(((GazeNormX < 0) ? GazeNormX + 0.5 : GazeNormX - 0.5));
                m_gazeDataParams.sStereoData.sRightEye.fGazeNormalizedLocation[1] = (float)(GazeNormY / 2.0);
                m_gazeDataParams.sStereoData.sRightEye.GazeDataValidityFlags = NV_GAZE_LOCATION_VALID;
            }
            else
            {
                m_gazeDataParams.sMonoData.version = NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER;
                m_gazeDataParams.sMonoData.fGazeNormalizedLocation[0] = (float)(GazeNormX / 2.0);
                m_gazeDataParams.sMonoData.fGazeNormalizedLocation[1] = (float)(GazeNormY / 2.0);
                m_gazeDataParams.sMonoData.GazeDataValidityFlags = NV_GAZE_LOCATION_VALID;
            }

            NvStatus = m_pGazeHandler->UpdateGazeData(m_GraphicsDevice->GetImmediateContext(), &m_gazeDataParams);

            break;

        default:
            break;
        }
    }
}

void FoveatedRenderHelper::PurgeInternalVRSResources()
{
    if (m_initialized && m_pVRSHelper)
    {
        NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS purgeParams;
        ZeroMemory(&purgeParams, sizeof(NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS));
        purgeParams.version = NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS_VER;

        m_pVRSHelper->PurgeInternalShadingRateResources(m_GraphicsDevice->GetImmediateContext(), &purgeParams);
    }
}

void FoveatedRenderHelper::GetShadingRateResource()
{
    NvAPI_Status NvStatus = NVAPI_OK;
    HRESULT hr = S_OK;

    if (m_initialized && m_pVRSHelper)
    {
        if (m_pShadingRateResource)
        {
            m_pShadingRateResource->Release();
            m_pShadingRateResource = NULL;
        }

        ZeroMemory(&m_srrTexDesc, sizeof(D3D11_TEXTURE2D_DESC));

        NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS getSRRParams;
        ZeroMemory(&getSRRParams, sizeof(NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS));
        getSRRParams.version = NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS_VER;
        getSRRParams.ppShadingRateResource = (IUnknown**)&m_pShadingRateResource;

        NvStatus = m_pVRSHelper->GetShadingRateResource(m_GraphicsDevice->GetImmediateContext(), &getSRRParams);

        if (NvStatus == NVAPI_OK)
        {
            if (m_pSRRShaderResourceView)
            {
                m_pSRRShaderResourceView->Release();
                m_pSRRShaderResourceView = NULL;
            }

            m_pShadingRateResource->GetDesc(&m_srrTexDesc);
            hr = m_GraphicsDevice->GetDevice()->CreateShaderResourceView(m_pShadingRateResource, NULL, &m_pSRRShaderResourceView);
            if (hr == S_OK)
            {
                // Do further handling of the shading rate resource
            }
        }
    }
}

void FoveatedRenderHelper::SetShadingRatePerformance(FoveatedShaderPerformance quality)
{
    switch (quality)
    {
    case FoveatedShaderPerformance::Highest_Performance:
        m_enableParams.sFoveatedRenderingDesc.ShadingRatePreset = NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGHEST_PERFORMANCE;
    break;
    case FoveatedShaderPerformance::High_Performance:
        m_enableParams.sFoveatedRenderingDesc.ShadingRatePreset = NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGH_PERFORMANCE;
    break;
    case FoveatedShaderPerformance::Balanced:
        m_enableParams.sFoveatedRenderingDesc.ShadingRatePreset = NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_BALANCED;
    break;
    case FoveatedShaderPerformance::High_Quality:
        m_enableParams.sFoveatedRenderingDesc.ShadingRatePreset = NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGH_QUALITY;
    break;
    case FoveatedShaderPerformance::Highest_Quality:
        m_enableParams.sFoveatedRenderingDesc.ShadingRatePreset = NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGHEST_QUALITY;
    break;
    case FoveatedShaderPerformance::Custom:
        m_enableParams.sFoveatedRenderingDesc.ShadingRatePreset = NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_CUSTOM;
    break;
    default:
        m_enableParams.sFoveatedRenderingDesc.ShadingRatePreset = NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGHEST_PERFORMANCE;
        break;
    }
}

void FoveatedRenderHelper::SetInnerRegionShadingRate(FoveatedShadingRate shadingRate)
{
    m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.InnerMostRegionShadingRate = (NV_PIXEL_SHADING_RATE)shadingRate;
}

void FoveatedRenderHelper::SetMiddleRegionShadingRate(FoveatedShadingRate shadingRate)
{
    m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.MiddleRegionShadingRate = (NV_PIXEL_SHADING_RATE)shadingRate;
}

void FoveatedRenderHelper::SetOuterRegionShadingRate(FoveatedShadingRate shadingRate)
{
    m_enableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.PeripheralRegionShadingRate = (NV_PIXEL_SHADING_RATE)shadingRate;
}

void FoveatedRenderHelper::ShutDown()
{
    if (m_initialized)
    {
        if (m_pShadingRateResource)
        {
            m_pShadingRateResource->Release();
            m_pShadingRateResource = NULL;
        }

        if (m_pSRRShaderResourceView)
        {
            m_pSRRShaderResourceView->Release();
            m_pSRRShaderResourceView = NULL;
        }

        PurgeInternalVRSResources();
        m_pGazeHandler->Release();
        m_pVRSHelper->Release();
        m_GraphicsDevice->GetDevice()->Release();
        m_GraphicsDevice->GetDevice()->Release();
        m_GraphicsDevice->GetDevice()->Release();
    }
}

FoveatedShaderPerformance FoveatedRenderHelper::ShadingPerformanceFromString(std::string string)
{
    for (size_t i = 0; i < 5; i++)
    {
        if (string == s_ShaderPerformanceTable[i])
        {
            // Starts from 1
            return (FoveatedShaderPerformance)(i);
        }
    }

    return FoveatedShaderPerformance::Balanced;
}

std::string FoveatedRenderHelper::ShadingPerformanceTooString(FoveatedShaderPerformance shadingRate)
{
    return s_ShaderPerformanceTable[(int)shadingRate];
}

FoveatedShadingRate FoveatedRenderHelper::ShadingRateFromString(std::string string)
{
    for (size_t i = 0; i < 12; i++)
    {
        if (string == s_ShaderRateTable[i])
        {
            // Starts from 1
            return (FoveatedShadingRate)(i);
        }
    }

    // Native
    return FoveatedShadingRate::Pixel_X1_Per_Raster_Pixels;
}

std::string FoveatedRenderHelper::ShadingRateTooString(FoveatedShadingRate shadingRate)
{
    return s_ShaderRateTable[(int)shadingRate];
}