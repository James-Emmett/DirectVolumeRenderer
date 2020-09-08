#include "World/Component/MeshRenderer.h"

void MeshRenderer::Draw(GraphicsDevice* device)
{
	assert(device != nullptr && "MesRender Device is nullptr");
	assert(m_Material != nullptr && "MeshRenderer has no materials");
	assert(m_Mesh != nullptr && "MeshRenderer has no mesh");

	// just bind it for now
	m_Material->Bind();

	//--Bind Buffers--
	device->BindVertexBuffer(m_Mesh->GetVertexHandle(), 0);
	device->BindIndexBuffer(m_Mesh->GetIndexHandle(), 0);

	for (size_t i = 0; i < m_Mesh->GetSubMeshCount(); i++)
	{
		const SubMesh* mesh = m_Mesh->GetSubMesh((Uint32)i);
		device->DrawIndexed(mesh->IndexOffset, mesh->IndexCount, 0);
	}
}
