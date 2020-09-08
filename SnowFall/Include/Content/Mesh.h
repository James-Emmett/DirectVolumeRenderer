//Note:
/*
	Resource representing vertex and index data for a object
	contains helper functions for generating normals and tangents.

	Follws Unity API as i like theres, has a simpel and complex function set
	allowing you to setup triangles and there data. Or push all data in a blob and hope for the best.
*/

#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/VertexTypes.h"
#include "Resource.h"

#define MESH_MAGIC 0x57

struct SubMesh
{
	Uint32 IndexOffset = 0;
	Uint32 IndexCount = 0;

	SubMesh() {}
	SubMesh(Uint32 start, Uint32 count)
	{
		IndexOffset = start;
		IndexCount = count;
	}
};

class Mesh : public Resource
{
private:
	GraphicsDevice* m_GraphicsDevice = nullptr;
	BufferHandle	m_VertexBuffer	 = BufferHandle();
	BufferHandle	m_IndexBuffer	 = BufferHandle();
	Uint32			m_VertexCount	 = 0;
	Uint32			m_IndexCount	 = 0;
	Uint32			m_SubMeshCount	 = 0;
	IndexFormat		m_IndexFormat	 = IndexFormat::I32;
	bool			m_IsReadable	 = 0;
	bool			m_IsDataOld		 = false;
	bool			m_IsPacked		 = false;

	std::vector<Vector3> m_Vertices;
	std::vector<Vector3> m_Normals;
	std::vector<Vector4> m_Tangents;
	std::vector<Color>	 m_Colors;
	std::vector<Vector2> m_uv;
	std::vector<Uint32>	 m_Indices;

	std::vector<SubMesh>	m_SubMesh;
	std::vector<VertexMesh> m_VertexMesh;

public:
	Mesh();
	Mesh(const Mesh& mesh) = delete;
	Mesh(Mesh&& mesh) noexcept;
	~Mesh();
	void operator=(const Mesh& mesh) = delete;
	void operator=(Mesh&& mesh) noexcept;

public:
	void Create(GraphicsDevice* device);
	void Create(GraphicsDevice* device, VertexMesh* vertexData, Uint32 count);
	Uint32 GetVertexCount()const;
	Uint32 GetIndexCount()const;
	Uint32 GetSubMeshCount()const;
	bool GetVertices(std::vector<Vector3>& vertices)const;
	bool GetNormals(std::vector<Vector3>& normals)const;
	bool GetTangent(std::vector<Vector4>& tangents)const;
	bool GetColors(std::vector<Color>& colors)const;
	bool GetUV(std::vector<Vector2>& uv)const;
	bool GetIndices(std::vector<Uint32>& indicies, Uint32 submesh)const;
	const SubMesh* GetSubMesh(Uint32 subMesh)const;

	void SetVertices(Vector3* vertices, Uint32 count);
	void SetNormals(Vector3* normals, Uint32 count);
	void SetTangent(Vector4* tangents, Uint32 count);
	void SetColors(Color* colors, Uint32 count);
	void SetUV(Vector2* uv, Uint32 count);
	void SetVertexData(VertexMesh* vertexData, Uint32 count);
	void SetIndices(Uint32* indicies, Uint32 count);
	void SetSubMeshCount(Uint32 count);
	void SetSubMesh(Uint32 subMesh, Uint32 indexCount, Uint32 indexStart);

	// Very expensive please do NOT use at runtime!!!
	void PackMesh();
	void RecalculateNormals();
	void RecalculateTangents();

	BufferHandle GetVertexHandle()const;
	BufferHandle GetIndexHandle()const;
	const Buffer* GetVertexResource()const;
	const Buffer* GetIndexResource()const;

	bool Apply(bool readable = false);
	void Clear();
	void Draw(); // For testing only
	void LoadFromFile(const std::string& fileName);
	bool SaveToFile(std::string fileName);
	void Reload();
	void Release();
	void OnGui();
};