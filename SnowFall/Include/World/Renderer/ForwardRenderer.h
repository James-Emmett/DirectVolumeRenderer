#pragma once
#include "BaseRenderer.h"
#include "VR/FoveatedRenderHelper.h"
#include "PostProcess/ToneMapping.h"

class ForwardRenderer : public BaseRenderer
{
public:
	RenderHandle m_RenderTargets[2];
	DepthHandle  m_DepthTargets[2];
	FoveatedRenderHelper* m_FoveatedRendering;
	ToneMapping m_ToneMapper;

public:
	void Initialize(GraphicsDevice* manager);
	void Render(Scene* scene);
	void Resize();
	void ShutDown();

protected:
	void RenderShadows(Scene* scene);
	void RenderCamera(Scene* scene, std::shared_ptr<Camera> camera, RenderHandle renderTarget, DepthHandle depthTarget);
	void DrawRenderQueue(const RenderQueue& renderQueue);
};