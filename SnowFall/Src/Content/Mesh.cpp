#include "Content/Mesh.h"
#include "System/Logger.h"
#include "System/File.h"
#include "System/Hash32.h"
#include "Application/Application.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "Tinyobj/tiny_obj_loader.h"

Mesh::Mesh()
{
	m_GraphicsDevice = Application::graphicsDevice;
}

Mesh::Mesh(Mesh&& mesh) noexcept
{
	m_VertexCount	= mesh.m_VertexCount;
	m_IndexCount	= mesh.m_IndexCount;
	m_SubMeshCount	= mesh.m_SubMeshCount;
	m_IndexFormat	= mesh.m_IndexFormat;
	m_IsReadable	= mesh.m_IsReadable;
	m_IsPacked		= mesh.m_IsPacked;
	m_IsDataOld		= mesh.m_IsDataOld;

	m_Indices		= std::move(mesh.m_Indices);
	m_Vertices		= std::move(mesh.m_Vertices);
	m_Normals		= std::move(mesh.m_Normals);
	m_Tangents		= std::move(mesh.m_Tangents);
	m_uv			= std::move(mesh.m_uv);
	m_Colors		= std::move(mesh.m_Colors);

	m_SubMesh	 = std::move(mesh.m_SubMesh);
	m_VertexMesh = std::move(mesh.m_VertexMesh);

	m_GraphicsDevice = mesh.m_GraphicsDevice;
	m_VertexBuffer	 = mesh.m_VertexBuffer;
	m_IndexBuffer	 = mesh.m_IndexBuffer;

	mesh.m_VertexBuffer = BufferHandle();
	mesh.m_IndexBuffer = BufferHandle();
	mesh.m_GraphicsDevice = nullptr;
}

Mesh::~Mesh()
{
	Release();
}

void Mesh::operator=(Mesh&& mesh) noexcept
{
	m_VertexCount = mesh.m_VertexCount;
	m_IndexCount = mesh.m_IndexCount;
	m_SubMeshCount = mesh.m_SubMeshCount;
	m_IndexFormat = mesh.m_IndexFormat;
	m_IsReadable = mesh.m_IsReadable;
	m_IsPacked = mesh.m_IsPacked;
	m_IsDataOld = mesh.m_IsDataOld;


	m_Indices = std::move(mesh.m_Indices);
	m_Vertices = std::move(mesh.m_Vertices);
	m_Normals = std::move(mesh.m_Normals);
	m_Tangents = std::move(mesh.m_Tangents);
	m_uv = std::move(mesh.m_uv);
	m_Colors = std::move(mesh.m_Colors);

	m_SubMesh = std::move(mesh.m_SubMesh);
	m_VertexMesh = std::move(mesh.m_VertexMesh);

	m_GraphicsDevice = mesh.m_GraphicsDevice;
	m_VertexBuffer = mesh.m_VertexBuffer;
	m_IndexBuffer = mesh.m_IndexBuffer;

	mesh.m_VertexBuffer = BufferHandle();
	mesh.m_IndexBuffer = BufferHandle();
	mesh.m_GraphicsDevice = nullptr;
}

void Mesh::Create(GraphicsDevice* device)
{
	m_GraphicsDevice = device;
}

void Mesh::Create(GraphicsDevice* device, VertexMesh* vertexData, Uint32 count)
{
	m_GraphicsDevice = device;
	SetVertexData(vertexData, count);
}

Uint32 Mesh::GetVertexCount() const
{
	return m_VertexCount;
}

Uint32 Mesh::GetIndexCount() const
{
	return m_IndexCount;
}

Uint32 Mesh::GetSubMeshCount() const
{
	return m_SubMeshCount;
}

bool Mesh::GetVertices(std::vector<Vector3>& vertices) const
{
	if (!m_IsReadable || m_Vertices.empty())
	{
		LogError("Vertex data missing or unaccessible");
		return false;
	}

	vertices = m_Vertices;
	return true;
}

bool Mesh::GetNormals(std::vector<Vector3>& normals) const
{
	if (!m_IsReadable || m_Normals.empty())
	{
		LogError("Normal data missing or unaccessible");
		return false;
	}

	normals = m_Normals;
	return true;
}

bool Mesh::GetTangent(std::vector<Vector4>& tangents) const
{
	if (!m_IsReadable || m_Tangents.empty())
	{
		LogError("Tangent data missing or unaccessible");
		return false;
	}

	tangents = m_Tangents;
	return true;
}

bool Mesh::GetColors(std::vector<Color>& colors) const
{
	if (!m_IsReadable || m_Colors.empty())
	{
		LogError("Color data missing or unaccessible");
		return false;
	}

	colors = m_Colors;
	return true;
}

bool Mesh::GetUV(std::vector<Vector2>& uv) const
{
	if (!m_IsReadable || m_uv.empty())
	{
		LogError("Uv data missing or unaccessible");
		return false;
	}

	uv = m_uv;
	return true;
}

bool Mesh::GetIndices(std::vector<Uint32>& indicies, Uint32 submesh) const
{
	if (!m_IsReadable || m_Indices.empty())
	{
		LogError("Index data missing or unaccessible");
		return false;
	}

	const SubMesh mesh = m_SubMesh[submesh];
	indicies.resize(mesh.IndexCount);
	memcpy(&indicies[0], &m_Indices[mesh.IndexOffset], sizeof(Uint32) * mesh.IndexCount);
	return true;;
}

const SubMesh* Mesh::GetSubMesh(Uint32 subMesh) const
{
	if (subMesh > m_SubMeshCount) { LogError("Index greater than sub mesh count."); return nullptr; }
	return &m_SubMesh[subMesh];
}

void Mesh::SetVertices(Vector3* vertices, Uint32 count)
{
	if (vertices == nullptr) { LogError("Vertices was nullptr"); return; }
	m_VertexCount = count;
	m_Vertices.assign(&vertices[0], &vertices[count]);
	m_IsDataOld = true;
}

void Mesh::SetNormals(Vector3* normals, Uint32 count)
{
	if (normals == nullptr) { LogError("Normals was nullptr"); return; }
	if (count < m_VertexCount) { LogWarning("Normal count differs from vertex count? May result in crash!"); }

	m_Normals.assign(&normals[0], &normals[count]);
	m_IsDataOld = true;
}

void Mesh::SetTangent(Vector4* tangents, Uint32 count)
{
	if (tangents == nullptr) { LogError("Tangents was nullptr"); return; }
	if (count < m_VertexCount) { LogWarning("Tangent count differs from vertex count? May result in crash!"); }

	m_Tangents.assign(&tangents[0], &tangents[count]);
	m_IsDataOld = true;
}

void Mesh::SetColors(Color* colors, Uint32 count)
{
	if (colors == nullptr) { LogError("Colors was nullptr"); return; }
	if (count < m_VertexCount) { LogWarning("Colors count differs from vertex count? May result in crash!"); }

	m_Colors.assign(&colors[0], &colors[count]);
	m_IsDataOld = true;
}

void Mesh::SetUV(Vector2* uv, Uint32 count)
{
	if (uv == nullptr) { LogError("Uvs was nullptr"); return; }
	if (count < m_VertexCount) { LogWarning("Uvs count differs from vertex count? May result in crash!"); }

	m_uv.assign(&uv[0], &uv[count]);
	m_IsDataOld = true;
}

void Mesh::SetVertexData(VertexMesh* vertexData, Uint32 count)
{
	if (vertexData == nullptr) { LogError("VertexData was nullptr"); return; }

	m_VertexCount = count;
	m_VertexMesh.assign(&vertexData[0], &vertexData[count]);
	m_IsDataOld = true;
	m_IsPacked = true;
}

void Mesh::SetIndices(Uint32* indicies, Uint32 count)
{
	if (indicies == nullptr) { LogError("Index Data was nullptr");  return; }
	m_Indices.assign(&indicies[0], &indicies[count]);
	m_IndexCount = count;
	m_IsDataOld = true;
}

void Mesh::SetSubMeshCount(Uint32 count)
{
	m_SubMeshCount = count;
	m_SubMesh.resize(count);
	m_IsDataOld = true;
}

void Mesh::SetSubMesh(Uint32 subMesh, Uint32 indexCount, Uint32 indexStart)
{
	if (m_SubMesh.empty() || subMesh >= m_SubMeshCount)
	{
		m_SubMesh.resize((Uint64)subMesh + 1);
		m_SubMeshCount = subMesh + 1;
	}

	m_SubMesh[subMesh].IndexCount = indexCount;
	m_SubMesh[subMesh].IndexOffset = indexStart;
	m_IsDataOld = true; // ??
}

void Mesh::PackMesh()
{
	if (!m_IsPacked && m_VertexCount > 0 && m_IndexCount > 0)
	{
		if (m_Colors.size() < m_VertexCount)
		{
			m_Colors.resize(m_VertexCount, Color::White);
		}

		if (m_uv.size() < m_VertexCount)
		{
			m_uv.resize(m_VertexCount, Vector2());
		}

		// Calculate normals if none exist or too little.
		if (m_Normals.size() < m_VertexCount)
		{
			RecalculateNormals();
		}

		// Calculate tangent if non exists.
		if (m_Tangents.size() < m_VertexCount)
		{
			RecalculateTangents();
		}

		m_VertexMesh.clear();
		m_VertexMesh.resize(m_VertexCount);
		for (Uint32 i = 0; i < m_VertexCount; i++)
		{
			VertexMesh vertex;
			vertex.m_Position = m_Vertices[i];
			vertex.m_Normal = m_Normals[i];
			vertex.m_Tangent = m_Tangents[i];
			vertex.m_Color = m_Colors[i];
			vertex.m_Texture = m_uv[i];
			m_VertexMesh[i] = vertex;
		}
		m_IsPacked = true;
	}
}

void Mesh::RecalculateNormals()
{
	if (m_Vertices.empty())
	{
		LogError("Cannot Generate Normals when vertices are null.");
		return;
	}

	std::vector<Vector3> normals(m_VertexCount, Vector3(0, 0, 0));
	for (SubMesh mesh : m_SubMesh)
	{
		for (Uint32 i = 0; i < mesh.IndexCount; i += 3)
		{
			int index = mesh.IndexOffset + i;

			//--Indices--
			Uint32 id0 = m_Indices[index];
			Uint32 id1 = m_Indices[(size_t)index + 1];
			Uint32 id2 = m_Indices[(size_t)index + 2];

			// Triangle
			Vector3 p0 = m_Vertices[id0];
			Vector3 p1 = m_Vertices[id1];
			Vector3 p2 = m_Vertices[id2];

			Vector3 E1 = p1 - p0;
			Vector3 E2 = p2 - p0;

			// Calculate face normal
			Vector3 n = Vector3::Cross(E1, E2);

			// Get angles between edges
			float A = Vector3::Angle(p1 - p0, p2 - p0);
			float B = Vector3::Angle(p2 - p1, p0 - p1);
			float C = Vector3::Angle(p0 - p2, p1 - p2);

			// Accumulate normals with weights.
			normals[id0] += n * A;
			normals[id1] += n * B;
			normals[id2] += n * C;
		}
	}


	m_Normals.clear();
	m_Normals.resize(m_VertexCount, Vector3());

	for (Uint32 i = 0; i < m_VertexCount; i++)
	{
		m_Normals[i] = Vector3::Normalize(normals[i]);
	}

	m_IsDataOld = true;
}

void Mesh::RecalculateTangents()
{
	if (m_Vertices.empty())
	{
		LogError("Cannot Generate Tangents when vertices are null.");
		return;
	}

	std::vector<Vector3> tangents(m_VertexCount, Vector3());
	std::vector<Vector3> bitangents(m_VertexCount, Vector3());

	// Loop each submesh, and each submehses triangles!
	for (SubMesh mesh : m_SubMesh)
	{
		for (Uint32 i = 0; i < mesh.IndexCount; i += 3)
		{
			int index = mesh.IndexOffset + i;

			//--Indices--
			Uint32 i0 = m_Indices[index];
			Uint32 i1 = m_Indices[(size_t)index + 1];
			Uint32 i2 = m_Indices[(size_t)index + 2];

			// Triangle
			Vector3& p0 = m_Vertices[i0];
			Vector3& p1 = m_Vertices[i1];
			Vector3& p2 = m_Vertices[i2];

			//--Uv Edges--
			Vector2 uv1 = m_uv[i1] - m_uv[i0];
			Vector2 uv2 = m_uv[i2] - m_uv[i0];

			//--Triangle Edges--
			Vector3 E1 = p1 - p0;
			Vector3 E2 = p2 - p0;

			//--Formula--
			float r = 1.0f / (uv1.x * uv2.y - uv2.x * uv1.y);
			Vector3 t = (E1 * uv2.y - E2 * uv1.y) * r;
			Vector3 b = (E2 * uv1.x - E1 * uv2.x) * r;

			// Accumulate for smoothing between shared vertexs
			tangents[i0] += t;
			tangents[i1] += t;
			tangents[i2] += t;

			bitangents[i0] += b;
			bitangents[i1] += b;
			bitangents[i2] += b;
		}
	}

	m_Tangents.clear();
	m_Tangents.resize(m_VertexCount, Vector4());
	for (Uint32 i = 0; i < m_VertexCount; i++)
	{
		Vector3 t = Vector3::Normalize(tangents[i]);
		Vector3 b = Vector3::Normalize(bitangents[i]);
		Vector3 n = Vector3::Normalize(m_Normals[i]);

		// Ortho-normalize tangent
		m_Tangents[i] = Vector3::Normalize(t - (n * Vector3::Dot(t, n)));

		// Orthogonal bitangent checked agaisnt bitangent for direction.
		m_Tangents[i].w = (Vector3::Dot(Vector3::Cross(t, b), n) > 0.0f) ? 1.0f : -1.0f;
	}

	m_IsDataOld = true;
}

BufferHandle Mesh::GetVertexHandle() const
{
	return m_VertexBuffer;
}

BufferHandle Mesh::GetIndexHandle() const
{
	return m_IndexBuffer;
}

const Buffer* Mesh::GetVertexResource() const
{
	assert(m_GraphicsDevice);
	return m_GraphicsDevice->GetBuffer(m_VertexBuffer);
}

const Buffer* Mesh::GetIndexResource() const
{
	assert(m_GraphicsDevice);
	return m_GraphicsDevice->GetBuffer(m_IndexBuffer);
}

bool Mesh::Apply(bool readable)
{
	if (m_GraphicsDevice == nullptr) { return false; LogError("Mesh doesnt have a graphcis device?"); };
	if (m_VertexCount == 0) { LogError("No vertices to set."); return false; }
	if (m_Indices.empty()) { LogError("No Indices to set."); return false; }
	if (m_IsReadable == false && !m_VertexBuffer.IsValid()) { LogError("Cannot update."); return false; }

	//--Create upload verts--
	if (m_IsDataOld == false)
	{
		return true; // True as its not really an error, just a redundent call!
	}

	if (m_SubMeshCount == 0)
	{
		SubMesh mesh;
		mesh.IndexCount = m_IndexCount;
		mesh.IndexOffset = 0;
		m_SubMesh.push_back(mesh);
	}

	// Slow, mostly used by people at runtime in unity, avoid if possible !!!
	PackMesh();

	// We have to re-create the mesh GPU buffers, because they want it readable now -_-
	if (readable && m_IsReadable == true)
	{
		m_IsReadable = false;

		// Destory current buffers as they may be dynamic!
		m_GraphicsDevice->DestroyBuffer(m_VertexBuffer);
		m_GraphicsDevice->DestroyBuffer(m_IndexBuffer);
		m_VertexBuffer = BufferHandle();
		m_IndexBuffer = BufferHandle();
	}

	//--Create the buffers if they dont exist already--
	if (m_VertexBuffer.IsValid() == false)
	{
		m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(m_VertexCount, (Uint32)VertexType::VertexMesh, readable ? BufferUsage::Immutable : BufferUsage::Dynamic, (Byte*)&m_VertexMesh[0]);
	}
	else // Update instead
	{
		m_GraphicsDevice->UpdateBuffer(m_VertexBuffer, (Byte*)&m_VertexMesh[0], m_VertexCount);
	}

	if (m_IndexBuffer.IsValid() == false)
	{
		m_IndexBuffer = m_GraphicsDevice->CreateIndexBuffer((Uint32)m_Indices.size(), IndexFormat::I32, readable ? BufferUsage::Immutable : BufferUsage::Dynamic, (Byte*)&m_Indices[0]);
	}
	else // Update
	{
		m_GraphicsDevice->UpdateBuffer(m_IndexBuffer, (Byte*)&m_Indices[0], (Uint32)m_Indices.size());
	}

	if (readable)
	{
		// Clear CPU data.
		Clear();
	}

	return true;
}

void Mesh::Clear()
{
	m_Vertices.clear();
	m_Normals.clear();
	m_Tangents.clear();
	m_Colors.clear();
	m_uv.clear();
	m_Indices.clear();
	m_IsReadable = false;
	m_IsDataOld = false;
}

void Mesh::Draw()
{
	assert(m_GraphicsDevice != nullptr && "Mesh is Uninitialized");
	if (m_VertexBuffer.IsValid() == false || m_IndexBuffer.IsValid() == false) { return; }

	//--Bind Buffers--
	m_GraphicsDevice->BindVertexBuffer(m_VertexBuffer, 0);
	m_GraphicsDevice->BindIndexBuffer(m_IndexBuffer, 0);

	for (SubMesh mesh : m_SubMesh)
	{
		m_GraphicsDevice->DrawIndexed(mesh.IndexOffset, mesh.IndexCount, 0);
	}
}

void Mesh::LoadFromFile(const std::string& fileName)
{
	//--Get Extension--
	std::string ext = fileName.c_str();
	ext = ext.substr(ext.find_last_of(".") + 1);
	for (int i = 0; i < 3; i++)
	{
		ext[i] = tolower(ext[i]);
	}

	if (ext == "obj")
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warning;
		std::string error;

		bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, fileName.c_str());

		if (!warning.empty())
		{
			LogWarning(warning);
		}

		if (!error.empty())
		{
			LogWarning(error);
		}

		if (!result)
		{
			assert(0 && "Failed To Load Mesh.");
		}

		if (attrib.texcoords.empty())
		{
			attrib.texcoords.resize(attrib.vertices.size());
		}

		if (attrib.colors.empty())
		{
			attrib.colors.resize(attrib.vertices.size());
		}

		if (attrib.normals.empty())
		{
			attrib.normals.resize(attrib.vertices.size());
		}

		Uint32 index_offset = 0;
		// Could hash instead?
		std::unordered_map<Uint32, Uint32> indexMap = {};

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++)
		{
			// Set submesh
			SetSubMesh((Uint32)s, (Uint32)shapes[s].mesh.indices.size(), index_offset);
			for (Uint32 i = 0; i < (Uint32)shapes[s].mesh.indices.size(); i++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[i];

				if (idx.texcoord_index < 0)
				{
					idx.texcoord_index = 0;
				}

				if (idx.normal_index < 0)
				{
					idx.normal_index = 0;
				}

				// We could just store packed, but we would end up repacking again anyways?
				VertexMesh vertex;
				vertex.m_Position = Vector3(attrib.vertices[(Uint64)3 * idx.vertex_index + 0], attrib.vertices[(Uint64)3 * idx.vertex_index + 1], attrib.vertices[(Uint64)3 * idx.vertex_index + 2]);
				vertex.m_Normal = Vector3(attrib.normals[(Uint64)3 * idx.normal_index + 0], attrib.normals[(Uint64)3 * idx.normal_index + 1], attrib.normals[(Uint64)3 * idx.normal_index + 2]);
				vertex.m_Texture = Vector2(attrib.texcoords[(Uint64)2 * idx.texcoord_index + 0], 1.0f - attrib.texcoords[(Uint64)2 * idx.texcoord_index + 1]);
				vertex.m_Color = Color(attrib.colors[(Uint64)3 * idx.vertex_index + 0], attrib.colors[(Uint64)3 * idx.vertex_index + 1], attrib.colors[(Uint64)3 * idx.vertex_index + 2]);

				// Tangent is 0,0,0 but should still be okay
				Uint32 hash = Hash32::ComputeHash((Byte*)&vertex, (Uint32)VertexType::VertexMesh);

				// Check if this vertex is unique
				std::unordered_map<Uint32, Uint32>::iterator itr = indexMap.find(hash);
				int index = (int)m_Vertices.size();
				if (itr == indexMap.end())
				{
					// Push back the data and assign the index
					m_Vertices.push_back(vertex.m_Position);
					m_Normals.push_back(vertex.m_Normal);
					m_uv.push_back(vertex.m_Texture);
					m_Colors.push_back(vertex.m_Color);
					indexMap[hash] = index;
				}
				else
				{
					index = itr->second;
				}

				m_Indices.push_back(index);
			}

			index_offset += (Uint32)shapes[s].mesh.indices.size();
		}

		//--Set counts--
		m_VertexCount = (Uint32)m_Vertices.size();
		m_IndexCount = index_offset;
		m_IsDataOld = true;
		m_IsReadable = true;
		m_IsPacked = false; //Urgg had to change this
		m_FilePath = fileName.c_str();
		m_Name = fileName.c_str();
		RecalculateTangents();
		Apply(); // Submit to GPU
		return;
	}
	else if (ext == "mesh")
	{
		BinaryFile file;
		if (file.Open(fileName.c_str(), FileMode::Read))
		{
			assert(file.ReadDword() == MESH_MAGIC && "Failed To Read Mesh.");
			Uint32 vertexCount = file.ReadDword();
			Uint32 indexCount = file.ReadDword();
			VertexMesh* vertexData = new VertexMesh[vertexCount];
			Uint32* indexData = new Uint32[indexCount];

			if (!file.ReadBuffer((Byte*)vertexData, (Uint32)VertexType::VertexMesh * vertexCount))
			{
				assert(0 && "Failed To Load Mesh.");
			}

			if (!file.ReadBuffer((Byte*)indexData, sizeof(Uint32) * indexCount))
			{
				assert(0 && "Failed To Load Mesh.");
			}

			//--Read Submeshes--
			Uint32 subMeshCount = file.ReadDword();
			for (Uint32 i = 0; i < subMeshCount; ++i)
			{
				Uint32 indexCount = file.ReadDword();
				Uint32 indexOffset = file.ReadDword();
				SetSubMesh((Uint32)i, indexCount, indexOffset);
			}

			SetVertexData(vertexData, vertexCount);
			SetIndices(indexData, indexCount);
			m_IsDataOld = true;
			m_IsReadable = true;
			m_IsPacked = true;
			m_FilePath = fileName.c_str();
			m_Name = fileName.c_str();
			Apply(); // Submit to GPU

			//--Clean data--
			delete[] vertexData;
			delete[] indexData;
			return;
		}
	}

	assert(0 && "Failed To Load Mesh.");
}

void Mesh::Reload()
{
	if (!m_FilePath.empty())
	{
		// cache file string because it might be blanked!
		std::string filePath = m_FilePath;
		bool wasOnGpu = m_VertexBuffer.IsValid() == false ? false : true;
		bool readable = m_IsReadable;

		Release();
		LoadFromFile(filePath.c_str());
		if (wasOnGpu) { Apply(readable); } // Resubmit resource to gpu
	}

	assert(0 && "Attempting to reload user generated mesh!");
}

bool Mesh::SaveToFile(std::string fileName)
{
	if (!m_IsReadable) { LogWarning("No Mesh data to write");  return false; }
	if (m_IndexCount == 0 || m_VertexCount == 0) { LogWarning("No data to save.");   return false; }

	std::string filePath = fileName;
	filePath += ".mesh";

	BinaryFile file;
	if (file.Open(filePath.c_str(), FileMode::Write))
	{
		// Pack mesh for save
		PackMesh();

		if (m_VertexMesh.empty()) { LogWarning("No vertices to save.");  return false; }

		// Write File Type
		file.WriteDword((Dword)MESH_MAGIC);

		// Write Buffers to file
		file.WriteDword(m_VertexCount);
		file.WriteDword(m_IndexCount);
		file.WriteBuffer((Byte*)&m_VertexMesh[0], (Uint32)VertexType::VertexMesh * m_VertexCount);
		file.WriteBuffer((Byte*)&m_Indices[0], sizeof(Uint32) * m_IndexCount);

		//--Write Submeshes--
		file.WriteDword(m_SubMeshCount);
		for (Uint32 i = 0; i < m_SubMeshCount; ++i)
		{
			file.WriteDword(m_SubMesh[i].IndexCount);
			file.WriteDword(m_SubMesh[i].IndexOffset);
		}
		file.Close();

		return true;
	}

	return false;
}

void Mesh::Release()
{
	if (m_GraphicsDevice)
	{
		m_GraphicsDevice->DestroyBuffer(m_VertexBuffer);
		m_GraphicsDevice->DestroyBuffer(m_IndexBuffer);
	}

	m_VertexBuffer = BufferHandle();
	m_IndexBuffer = BufferHandle();
	m_VertexMesh.clear();
	m_SubMesh.clear();
	m_VertexCount = 0;
	m_IndexCount = 0;
	m_SubMeshCount = 0;
	Clear();
}

void Mesh::OnGui()
{
}