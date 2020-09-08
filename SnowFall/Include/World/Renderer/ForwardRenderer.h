#pragma once
#include "BaseRenderer.h"

class ForwardRenderer : public BaseRenderer
{
public:
	RenderHandle m_RenderTargets[2];
	DepthHandle  m_DepthTargets[2];

public:
	void Initialize(GraphicsDevice* manager);
	void Render(Scene* scene);
	void Resize();

protected:
	void RenderShadows(Scene* scene);
	void RenderCamera(Scene* scene, std::shared_ptr<Camera> camera, RenderHandle renderTarget, DepthHandle depthTarget);
	void DrawRenderQueue(const RenderQueue& renderQueue);
};