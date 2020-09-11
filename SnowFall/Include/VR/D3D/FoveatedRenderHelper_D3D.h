#pragma once
//----------------------------------------------------------------------------------
// File:        NvFoveatedRenderingHelper.h
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

#include "d3d11.h"
#include "Nvapi/nvapi.h"

#ifndef NV_SHADER_EXTN_SLOT_NUMBER
#define NV_SHADER_EXTN_SLOT_NUMBER 7
#endif

enum class FoveatedRenderEyeTracking
{
    None = 0,
    Mouse_Cursor = 1,
    Max = Mouse_Cursor
};

enum class FoveatedRenderSupport
{
    Unkown = 0,
    Not_Supported = 1,
    Supported = 2,
};

class GraphicsDevice;
class FoveatedRenderHelper
{
private:
    GraphicsDevice* m_GraphicsDevice = nullptr;

public:
    bool                    m_initialized;
    bool                    m_requestGetShadingRateResourceOnNextEnable;
    unsigned long long      m_gazeTimestamp;

    ID3DNvVRSHelper*                                m_pVRSHelper;
    ID3DNvGazeHandler*                              m_pGazeHandler;
    NV_VRS_HELPER_ENABLE_PARAMS                     m_enableParams;
    NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS   m_gazeDataParams;
    ID3D11Texture2D*                                m_pShadingRateResource;
    ID3D11ShaderResourceView*                       m_pSRRShaderResourceView;
    D3D11_TEXTURE2D_DESC                            m_srrTexDesc;

    FoveatedRenderEyeTracking                       m_eyeTrackingMode;
    FoveatedRenderSupport                           m_foveatedRenderingSupport;

    FoveatedRenderHelper();
    ~FoveatedRenderHelper();

    bool Initialize(GraphicsDevice* device);
    void LatchGazeData();
    void EnableFoveatedRendering(NV_VRS_RENDER_MODE renderMode);
    void DisableFoveatedRendering();
    void UpdateGazeData(float GazeNormX, float GazeNormY, bool bStereoSplit);
    void PurgeInternalVRSResources();
    void GetShadingRateResource();
    void ShutDown();
};