//Note;
/*
	In future all this should do is return a DrawItem to the render
	which can store it in the correct list (Opaque, Geometry, Transparent etc)
	and then render it in the pereffered way it wants tooe tc.
*/

#pragma once
#include "RenderComponent.h"
#include "Content/Mesh.h"

class MeshRenderer : public RenderComponent
{
public:
	std::shared_ptr<Mesh> m_Mesh;

public:
	void Draw(GraphicsDevice* device);
};