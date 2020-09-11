#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Blitter.h"
#include "VR/VR_Manager.h"
#include "RenderCommon.h"
#include <memory>

class PostProcessor;
class Scene;
class Camera;
class Shader;
class BaseRenderer
{
protected:
	GraphicsDevice* m_GraphicsDevice = nullptr;
	VR_Manager*		m_VRManager = nullptr;
	BufferHandle	m_ConstantBuffers[(Uint32)UniformTypes::Count];
	Blitter			m_Blitter;

public:
	virtual void Initialize(GraphicsDevice* device);
	virtual void Render(Scene* scene) = 0;
	virtual void Resize()=0;
	virtual void ShutDown()=0;

protected:
	void SetConstantBuffers();
	void SetUpFrame();
	void SetCameraProperties(std::shared_ptr<Camera> camera);
	void SetEnviromentTextures(Scene* scene);
	void DrawSkybox(Scene* scene, std::shared_ptr<Camera> camera);
	virtual void RenderCamera(Scene* scene, std::shared_ptr<Camera> camera, RenderHandle renderTarget, DepthHandle depthTarget) = 0;
};