#pragma once
#include "Component.h"
#include "Content/Material.h"

//https://docs.unity3d.com/ScriptReference/Renderer.html
class RenderComponent : public Component<RenderComponent>
{
public:
	std::shared_ptr<Material> m_Material; // expand to multiple in future?

public:
	bool m_CastShadows = true;
	bool m_ReceiveShadows = true;

public:
	RenderType GetRenderQueue()const;
	virtual void Draw(GraphicsDevice* device) = 0;
};