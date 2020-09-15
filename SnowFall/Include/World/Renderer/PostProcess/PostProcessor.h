//Note:
/*
	Base Post process effect, inherit to make your
	own effects if required. Effects are added to a stack in the scene and get rendered
	by the renderer!

	Very Important, Set IsHDR otherwise it may get ran in a LDR pass and details
	will be lost!
*/

#pragma once
#include "Graphics/GraphicsDevice.h"
#include <memory>

class Scene;
class Shader;
class GraphicsDevice;
class PostProcessor
{
public:
	std::shared_ptr<Shader> m_Effect;
	GraphicsDevice*			m_GraphicsDevice = nullptr;
	bool					m_Enabled = true;
	int						m_Order = 0;

public:
	PostProcessor();
	~PostProcessor();

public:
	virtual void Initialize(GraphicsDevice* device, int executionOrder);
	virtual void Release();
	virtual void Apply(RenderHandle source, RenderHandle destination);
	virtual void OnGui();

	bool operator>(const PostProcessor& pass);
	bool operator<(const PostProcessor& pass);
	bool operator>=(const PostProcessor& pass);
	bool operator<=(const PostProcessor& pass);

protected:
	// Its actually a triangle, check Tonemap.hlsl for example of vert shader required!
	void DrawFullScreenQuad(TextureHandle texture, RenderHandle target, std::shared_ptr<Shader> shader = nullptr);
};