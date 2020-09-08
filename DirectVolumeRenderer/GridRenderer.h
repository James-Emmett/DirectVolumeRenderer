#pragma once
#include "Graphics/GraphicsDevice.h"
#include "World/Component/RenderComponent.h"

class GridRenderer : public RenderComponent
{
private:
	int				m_Divisions = 0;
	Uint32			m_VertexCount = 0;
	BufferHandle	m_VertexBuffer;
	GraphicsDevice* m_Device = nullptr;

public:
	void Initialize(int divisions = 10);
	void Release();

	Uint32 Divisions()const;
	Uint32 VertexCount()const;
	void Draw(GraphicsDevice* device);
};