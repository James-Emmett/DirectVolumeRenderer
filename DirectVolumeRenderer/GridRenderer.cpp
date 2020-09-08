#include "GridRenderer.h"
#include "Application/Application.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/CommonStates.h"
#include "Content/ContentManager.h"
#include "System/Logger.h"

void GridRenderer::Initialize(int divisions)
{
	m_Device = Application::graphicsDevice;
	m_Divisions = divisions;

	Vector3 xAxis = Vector3(-m_Divisions * 0.5f, 0, 0);
	Vector3 zAxis = Vector3(0, 0, -m_Divisions * 0.5f);

	std::vector<Vertex> vertexData;

	for (size_t i = 0; i <= m_Divisions; i++)
	{
		// Map from 0 to 1 into -1 to 1 range.
		Vector3 xPos = xAxis * ((((float)i / m_Divisions) * 2.0f) - 1);
		Vector3 zPos = zAxis * ((((float)i / m_Divisions) * 2.0f) - 1);

		// Vertical Line
		vertexData.push_back(xPos - zAxis);
		vertexData.push_back(xPos + zAxis);

		// Horizontal Line
		vertexData.push_back(zPos - xAxis);
		vertexData.push_back(zPos + xAxis);
	}
	m_VertexCount = (Uint32)vertexData.size();

	m_VertexBuffer = m_Device->CreateVertexBuffer(m_VertexCount, (Uint32)VertexType::Vertex, BufferUsage::Immutable, (Byte*)&vertexData[0]);

	m_Material = Application::contentManager->Load<Material>("Assets/Shaders/Grid/grid.material");

}

void GridRenderer::Release()
{
	if (m_Device)
	{
		if (m_VertexBuffer.IsValid())
		{
			m_Device->DestroyBuffer(m_VertexBuffer);
		}
	}

	LogError("No device to release buffer.");
}

Uint32 GridRenderer::Divisions() const
{
	return m_Divisions;
}

Uint32 GridRenderer::VertexCount() const
{
	return m_VertexCount;
}

void GridRenderer::Draw(GraphicsDevice* device)
{
	if (m_VertexBuffer.IsValid() == false)
	{
		Initialize();
	}

	m_Material->Bind();
	device->BindVertexBuffer(m_VertexBuffer, 0);
	device->Draw(m_VertexCount, 0, 0);
}