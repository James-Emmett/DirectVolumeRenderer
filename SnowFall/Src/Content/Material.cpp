#include "Content/Material.h"
#include "UI/ImGui_Interface.h"
#include "Tinyxml/tinyxml2.h"
#include "System/Hash32.h"
#include "Application/Application.h"
#include "System/Logger.h"
#include "System/File.h"
#include "Content/ContentManager.h"

const Uint32 propertyByteSize[4] = {};

Material::Material()
{
	// Change too dependency Injection
	m_GraphicsDevice = Application::graphicsDevice;
}

Material::Material(std::shared_ptr<Shader> shader)
{
	m_Shader = shader;
	m_GraphicsDevice = Application::graphicsDevice;

	// Create Buffer from Properties
	const std::vector<Property>& properties = m_Shader->PropertyList();
	for (size_t i = 0; i < properties.size(); i++)
	{
		if (properties[i].m_Type == PropertyType::Texture)
		{
			AddTextureProperty(properties[i].m_Name.c_str(), nullptr);
		}
		else
		{
			// Add an empty proeprty to reserve!
			AddValueProperty(properties[i].m_Name.c_str(), properties[i].m_Type, 0);
		}
	}

	// Create Constant Buffer
	Uint32 bufferSize = (Uint32)m_Buffer.size();
	if (bufferSize > 0)
	{
		// Pad buffer to 16 byte boundary, i.e fill empty spaces.
		Uint32 remainingBytes = (Uint32)(16 - ((m_Buffer.size()) % 16));
		bufferSize += remainingBytes;
		m_Buffer.resize(m_Buffer.size() + remainingBytes);
		m_ConstantBuffer = m_GraphicsDevice->CreateConstantBuffer(bufferSize, (Byte*)&m_Buffer[0]);
		assert(m_ConstantBuffer.IsValid() && "Failed to create Constant Buffer.");
	}
}

Material::Material(Material&& mat) noexcept : Resource(std::move(mat))
{
	m_ConstantBuffer = mat.m_ConstantBuffer;
	m_Shader	= mat.m_Shader;
	m_Textures	= std::move(mat.m_Textures);
	m_Buffer	= std::move(mat.m_Buffer);
	m_GraphicsDevice = mat.m_GraphicsDevice;
	mat.m_ConstantBuffer = BufferHandle();
}

Material::~Material()
{
	Release();
}

void Material::operator=(Material&& mat) noexcept
{
	m_ConstantBuffer = mat.m_ConstantBuffer;
	m_Shader	= mat.m_Shader;
	m_Textures	= std::move(mat.m_Textures);
	m_Buffer	= std::move(mat.m_Buffer);

	mat.m_ConstantBuffer = BufferHandle();
	mat.m_Shader.reset();
}

float Material::GetFloat(std::string name)const
{
	return GetFloat(m_Shader->PropertyToId(name));
}

float Material::GetFloat(int id)const
{
	float value = 0.0f;

	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Float)
	{
		memcpy(&value, &m_Buffer[m_Offsets[id]], sizeof(float));
	}

	return value;
}

void Material::SetFloat(std::string name, float value)
{
	return SetFloat(m_Shader->PropertyToId(name), value);
}

void Material::SetFloat(int id, float value)
{
	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Float)
	{
		memcpy(&m_Buffer[m_Offsets[id]], &value, sizeof(float));
		m_Dirty = true;
	}
}

int Material::GetInt(std::string name)const
{
	return GetInt(m_Shader->PropertyToId(name));
}

int Material::GetInt(int id)const
{
	int value = 0;
	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Int)
	{
		memcpy(&value, &m_Buffer[m_Offsets[id]], sizeof(int));
	}

	return value;
}

void Material::SetInt(std::string name, int value)
{
	SetInt(m_Shader->PropertyToId(name), value);
}

void Material::SetInt(int id, int value)
{
	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Int)
	{
		memcpy(&m_Buffer[m_Offsets[id]], &value, sizeof(int));
		m_Dirty = true;
	}
}

Vector2 Material::GetVector2(std::string name)const
{
	return GetVector2(m_Shader->PropertyToId(name));
}

Vector2 Material::GetVector2(int id)const
{
	Vector2 value = Vector2::Zero;

	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Vector2)
	{
		memcpy(&value, &m_Buffer[m_Offsets[id]], sizeof(Vector2));
	}

	return value;
}

void Material::SetVector2(std::string name, Vector2 value)
{
	SetVector2(m_Shader->PropertyToId(name), value);
}

void Material::SetVector2(int id, Vector2 value)
{
	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Vector2)
	{
		memcpy(&m_Buffer[m_Offsets[id]], &value, sizeof(Vector2));
		m_Dirty = true;
	}
}

Vector3 Material::GetVector3(std::string name)const
{
	return GetVector3(m_Shader->PropertyToId(name));
}

Vector3 Material::GetVector3(int id)const
{
	Vector3 value = Vector3::Zero;
	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Vector3)
	{
		memcpy(&value, &m_Buffer[m_Offsets[id]], sizeof(Vector3));
	}

	return value;
}

void Material::SetVector3(std::string name, Vector3 value)
{
	SetVector3(m_Shader->PropertyToId(name), value);
}

void Material::SetVector3(int id, Vector3 value)
{
	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Vector3)
	{
		memcpy(&m_Buffer[m_Offsets[id]], &value, sizeof(Vector3));
		m_Dirty = true;
	}
}

Vector4 Material::GetVector4(std::string name)const
{
	return GetVector4(m_Shader->PropertyToId(name));
}

Vector4 Material::GetVector4(int id)const
{
	Vector4 value = Vector3::Zero;

	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Vector4)
	{
		memcpy(&value, &m_Buffer[m_Offsets[id]], sizeof(Vector4));
	}

	return value;
}

void Material::SetVector4(std::string name, Vector4 value)
{
	SetVector4(m_Shader->PropertyToId(name), value);
}

void Material::SetVector4(int id, Vector4 value)
{
	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Vector4)
	{
		memcpy(&m_Buffer[m_Offsets[id]], &value, sizeof(Vector4));
		m_Dirty = true;
	}
}

Color Material::GetColor(std::string name)const
{
	return GetColor(m_Shader->PropertyToId(name));
}

Color Material::GetColor(int id)const
{
	Color value = Color::White;

	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Color)
	{
		memcpy(&value, &m_Buffer[m_Offsets[id]], sizeof(Color));
	}

	return value;
}

void Material::SetColor(std::string name, Color value)
{
	SetColor(m_Shader->PropertyToId(name), value);
}

void Material::SetColor(int id, Color value)
{
	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Buffer.size() && property.m_Type == PropertyType::Color)
	{
		memcpy(&m_Buffer[m_Offsets[id]], &value, sizeof(Color));
		m_Dirty = true;
	}
}

const std::shared_ptr<Texture> Material::GetTexture(std::string name)const
{
	return GetTexture(m_Shader->PropertyToId(name));
}

const std::shared_ptr<Texture> Material::GetTexture(int id)const
{
	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Textures.size() && property.m_Type == PropertyType::Texture)
	{
		return  m_Textures[m_Offsets[id]];
	}

	return nullptr;
}

void Material::SetTexture(std::string name, std::shared_ptr<Texture> value)
{
	SetTexture(m_Shader->PropertyToId(name), value);
}

void Material::SetTexture(int id, std::shared_ptr<Texture> value)
{
	if (id >= (int)m_Shader->GetPropertyCount()) { return; }
	if (value == nullptr) { return; }

	Property& property = m_Shader->GetProperty(id);
	if (m_Offsets[id] < m_Textures.size() && property.m_Type == PropertyType::Texture)
	{
		m_Textures[m_Offsets[id]] = value;
	}
}

bool Material::HasProperty(std::string name)
{
	for (size_t i = 0; i < m_Shader->GetPropertyCount(); i++)
	{
		const std::vector<Property>& properties = m_Shader->PropertyList();
		if (properties[i].m_Name == name)
		{
			return true;
		}
	}
	return false;
}

const std::vector<std::shared_ptr<Texture>>& Material::GetTextureArray() const
{
	return m_Textures;
}

std::shared_ptr<Shader> Material::GetShader() const
{
	return m_Shader;
}

BufferHandle Material::GetConstantBuffer() const
{
	return m_ConstantBuffer;
}

void Material::LoadFromFile(const std::string& filePath)
{
	//--Get Extension--
	assert(filePath.empty() == false);

	//--Get Extension--
	std::string path = filePath.c_str();
	size_t extPos = path.find_last_of(".");
	size_t endDir = path.find_last_of("//");
	std::string ext = path.substr(extPos + 1);

	m_FilePath = filePath;
	m_Name = filePath.substr((int)(endDir + 1), (int)((extPos - endDir) - 1));

	bool result;
	if (ext == "material")
	{
		result = LoadFromXML(filePath);
	}
	else if (ext == "materialbinary")
	{
		result = LoadFromBinary(filePath);
	}
}

void Material::Reload()
{
	if (!m_FilePath.empty())
	{
		// cache file std::string because it might be blanked!
		std::string filePath = m_FilePath;

		Release();
		LoadFromFile(filePath.c_str());
		return;
	}

	LogWarning("Attempting to reload user generated Material!");
}


void Material::OnGui()
{
	if (ImGui::CollapsingHeader(m_Name.c_str()))
	{
		// Dispaly texture inputs
		for (size_t i = 0; i < m_Textures.size(); i++)
		{
			// Need to figure this out :/, Textures are live, would have to release re-Request
			// From asset manager?
		}

		// Only bother if we have something
		if (!m_Buffer.empty() && m_Shader)
		{
			std::vector<Property>& properties = m_Shader->PropertyList();
			for (size_t i = 0; i <  properties.size(); ++i)
			{
				// Putting trust in imgui not to overrun memory! Risky o.0
				Property& property = properties[i];
				switch (properties[i].m_Type)
				{
				case PropertyType::Int:
				{
					if (property.m_IsRanged)
					{
						if (ImGui::SliderInt(property.m_Name.c_str(), (int*)&m_Buffer[m_Offsets[i]], (int)property.m_Range.x, (int)property.m_Range.y))
						{
							m_Dirty = true;
						}
					}
					else
					{
						if (ImGui::InputInt(property.m_Name.c_str(), (int*)&m_Buffer[m_Offsets[i]]))
						{
							m_Dirty = true;
						}
					}
					break;
				}

				case PropertyType::Float:
				{
					if (property.m_IsRanged)
					{
						if (ImGui::SliderFloat(property.m_Name.c_str(), (float*)&m_Buffer[m_Offsets[i]], property.m_Range.x, property.m_Range.y))
						{
							m_Dirty = true;
						}
					}
					else
					{
						if (ImGui::InputFloat(property.m_Name.c_str(), (float*)&m_Buffer[m_Offsets[i]]))
						{
							m_Dirty = true;
						}
					}
					break;
				}

				case PropertyType::Vector2:
				{
					if (ImGui::InputFloat2(property.m_Name.c_str(), (float*)&m_Buffer[m_Offsets[i]]))
					{
						m_Dirty = true;
					}
					break;
				}

				case PropertyType::Vector3:
				{
					if (ImGui::InputFloat3(property.m_Name.c_str(), (float*)&m_Buffer[m_Offsets[i]]))
					{
						m_Dirty = true;
					}
					break;
				}

				case PropertyType::Vector4:
				{
					if (ImGui::InputFloat4(property.m_Name.c_str(), (float*)&m_Buffer[m_Offsets[i]]))
					{
						m_Dirty = true;
					}
					break;
				}

				case PropertyType::Color:
				{
					if (ImGui::ColorEdit4(property.m_Name.c_str(), (float*)&m_Buffer[m_Offsets[i]]))
					{
						m_Dirty = true;
					}
					break;
				}
				}
			}
		}
	}
}

void Material::Bind(Uint32 startTextureSlot, Uint32 startBuffer)
{
	for (int i = 0; i < (int)m_Textures.size(); i++)
	{
		// Material textures always start after Renderers defaults.
		m_GraphicsDevice->BindTexture(m_Textures[i]->GetTextureHandle(), startTextureSlot + i);
		m_GraphicsDevice->BindSampler(m_Textures[i]->GetSampleHandle(), startTextureSlot + i);
	}
	if (m_ConstantBuffer.IsValid())
	{
		// Update the constant buffer!
		if (m_Dirty)
		{
			m_GraphicsDevice->UpdateBuffer(m_ConstantBuffer, (Byte*)&m_Buffer[0], (Uint32)m_Buffer.size());
			m_Dirty = false;
		}

		// Material constant is always added after ALL the default render onedds.
		m_GraphicsDevice->BindConstantBuffer(m_ConstantBuffer, startBuffer);
	}

	if (m_Shader)
	{
		m_Shader->Bind();
	}
}

bool Material::SaveToFile(std::string fileName)
{
	std::string filePath = fileName;
	filePath += ".MaterialBinary";

	BinaryFile file;
	if (file.Open(filePath.c_str(), FileMode::Write))
	{
		// Write out shader path
		file.WriteString(m_Shader->ResourcePath().c_str());

		file.WriteDword((Dword)m_Offsets.size());
		for (Uint32 i = 0; i < m_Offsets.size(); ++i)
		{
			m_Offsets.push_back(file.ReadDword());
		}

		// Write texture Paths out, this could be a GUID in the future.
		Uint32 textureCount = (Uint32)m_Textures.size();
		file.WriteDword(textureCount);

		for (size_t i = 0; i < textureCount; i++)
		{
			file.WriteString(m_Textures[i]->ResourcePath().c_str());
		}

		// Write size, Required so we know not to load if its 0
		file.WriteDword((Uint32)m_Buffer.size());

		// Write buffer if we need too
		if (m_Buffer.size() > 0)
		{
			// Write the Constant buffer block
			file.WriteBuffer((Byte*)&m_Buffer[0], (Uint32)m_Buffer.size());
		}

		file.Close();
		return true;
	}

	return false;
}

void Material::Release()
{
	if (m_GraphicsDevice)
	{
		// Destory constant buffer
		m_GraphicsDevice->DestroyBuffer(m_ConstantBuffer);
	}

	// Clear memory
	m_Buffer.clear();

	// Release are references to the textures!
	m_Textures.clear();

	// Reslease shader
	m_Shader.reset();
}

bool Material::LoadFromBinary(const std::string& filePath)
{
	BinaryFile file;
	if (file.Open(filePath.c_str(), FileMode::Read))
	{
		// Write out shader path
		m_Shader = Application::contentManager->Load<Shader>(file.ReadString().c_str());
		if (m_Shader == nullptr)
		{
			LogError("Failed to create shader.");
			file.Close();
			return false;
		}

		Uint32 offsetCount = file.ReadDword();
		for (Uint32 i = 0; i < offsetCount; ++i)
		{
			m_Offsets.push_back(file.ReadDword());
		}

		// Read and load each texture
		Uint32 textureCount = file.ReadDword();
		for (size_t i = 0; i < textureCount; i++)
		{
			// Acttually load the texture
			std::shared_ptr<Texture> texture = Application::contentManager->Load<Texture>(file.ReadString().c_str());
			if (texture)
			{
				texture->Apply();
				m_Textures.push_back(texture);
			}
			else
			{
				file.Close();
				return false;
			}
		}

		// Write the Constant buffer block
		Uint32 bufferSize = file.ReadDword();
		if (bufferSize > 0)
		{
			m_Buffer.resize(bufferSize);
			file.ReadBuffer((Byte*)&m_Buffer[0], bufferSize);
			m_ConstantBuffer = m_GraphicsDevice->CreateConstantBuffer(bufferSize, (Byte*)&m_Buffer[0]);

			if (!m_ConstantBuffer.IsValid())
			{
				LogError("Failed to create Constant Buffer.");
				return false;
			}
		}

		file.Close();
		return true;
	}

	return false;
}

bool Material::LoadFromXML(const std::string& filePath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filePath.c_str()) == tinyxml2::XMLError::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pRootElement = doc.FirstChildElement("Material");
		if (pRootElement != nullptr)
		{
			//--Load Properties--
			tinyxml2::XMLElement* pPropertyList = pRootElement->FirstChildElement("Properties");
			if (pPropertyList != nullptr)
			{
				tinyxml2::XMLElement* pPropertyElement = pPropertyList->FirstChildElement("Property");
				while (pPropertyElement != nullptr)
				{
					std::string type = pPropertyElement->Attribute("type");
					PropertyType propertyType = (PropertyType)Hash32::ComputeHash((Byte*)type.c_str(), (Uint32)type.size());

					switch (propertyType)
					{
					case PropertyType::Color:
					case PropertyType::Vector4:
						AddValueProperty(pPropertyElement->Attribute("name"), propertyType,
										std::stof(pPropertyElement->Attribute("X")),
										std::stof(pPropertyElement->Attribute("Y")),
										std::stof(pPropertyElement->Attribute("Z")),
										std::stof(pPropertyElement->Attribute("W")));
						break;
					case PropertyType::Vector3:
						AddValueProperty(pPropertyElement->Attribute("name"), propertyType,
										std::stof(pPropertyElement->Attribute("X")),
										std::stof(pPropertyElement->Attribute("Y")),
										std::stof(pPropertyElement->Attribute("Z")));
						break;
					case PropertyType::Vector2:
						AddValueProperty(pPropertyElement->Attribute("name"), propertyType,
										std::stof(pPropertyElement->Attribute("X")),
										std::stof(pPropertyElement->Attribute("Y")));
						break;
					case PropertyType::Float:
					case PropertyType::Int:
					{
						if (pPropertyElement->Attribute("min") && pPropertyElement->Attribute("max"))
						{
							AddValueProperty(pPropertyElement->Attribute("name"), propertyType, std::stof(pPropertyElement->Attribute("X")));
						}
						else
						{
							AddValueProperty(pPropertyElement->Attribute("name"), propertyType, std::stof(pPropertyElement->Attribute("X")));
						}
					}
					break;
					case PropertyType::Texture:
						AddTextureProperty(pPropertyElement->Attribute("name"), pPropertyElement->Attribute("source"));
						break;
					case PropertyType::Undefined:
					default:
						LogError("Unrecognised Property Type.");
						return false;
						break;
					}
					pPropertyElement = pPropertyElement->NextSiblingElement("Property");
				}
			}

			//--Load Shader--
			tinyxml2::XMLElement* pShaderElement = pRootElement->FirstChildElement("Shader");
			if (pShaderElement != nullptr)
			{
				m_Shader = Application::contentManager->Load<Shader>(pShaderElement->Attribute("source"));

				if (!m_Shader)
				{
					LogError("No Shaders Found.");
					return false;
				}
			}

			// Create constant buffer as everything else is good!
			if (m_Buffer.size() > 0)
			{
				// Pad buffer to 16 byte boundary, i.e fill empty spaces.
				Uint32 remainingBytes = (Uint32)(16 - ((m_Buffer.size()) % 16));
				m_Buffer.resize(m_Buffer.size() + remainingBytes);

				m_ConstantBuffer = m_GraphicsDevice->CreateConstantBuffer((Uint32)m_Buffer.size(), (Byte*)&m_Buffer[0]);

				if (!m_ConstantBuffer.IsValid())
				{
					LogError("Failed to create Constant Buffer.");
					return false;
				}
			}

			return true;
		}
		else
		{
			LogError("Material Not Found.");
			return false;
		}
	}
	else
	{
	LogError("Material Not Found.");
		return false;
	}

	LogError("Poorly Formed XML for Material");
	return false;
}

void Material::AddTextureProperty(const char* name, const char* source)
{
	if (source != nullptr)
	{
		// Acttually load the texture
		std::shared_ptr<Texture> texture = Application::contentManager->Load<Texture>(source);
		if (texture)
		{
			m_Offsets.push_back((Uint32)m_Textures.size());
			texture->Apply();
			m_Textures.push_back(texture);
		}
		else
		{
			assert("Failed to load texture");
		}
	}
	else
	{
		// Just add a blank texture slot ready for use
		m_Offsets.push_back((Uint32)m_Textures.size());
		m_Textures.push_back(std::shared_ptr<Texture>());
	}

}

void Material::AddValueProperty(const char* name, PropertyType type, float X, float Y, float Z, float W)
{
	Uint32 offset = 0;

	switch (type)
	{
	case PropertyType::Color:
	case PropertyType::Vector4:
		offset = ResizeAlignBuffer(16);
		memcpy(&m_Buffer[offset], &X, 4);
		memcpy(&m_Buffer[offset + 4], &Y, 4);
		memcpy(&m_Buffer[offset + 8], &Z, 4);
		memcpy(&m_Buffer[offset + 12], &W, 4);
		break;
	case PropertyType::Vector3:
		offset = ResizeAlignBuffer(12);
		memcpy(&m_Buffer[offset], &X, 4);
		memcpy(&m_Buffer[offset + 4], &Y, 4);
		memcpy(&m_Buffer[offset + 8], &Z, 4);
		break;
	case PropertyType::Vector2:
		offset = ResizeAlignBuffer(8);
		memcpy(&m_Buffer[offset], &X, 4);
		memcpy(&m_Buffer[offset + 4], &Y, 4);
		break;
	case PropertyType::Float:
		offset = ResizeAlignBuffer(4);
		memcpy(&m_Buffer[offset], &X, 4);
		break;
	case PropertyType::Int:
	{
		offset = ResizeAlignBuffer(4);
		int value = (int)X; // get float as an int.
		memcpy(&m_Buffer[offset], &value, 4);
	}
	break;
	default:
		assert(0 && "Unknown property");
		break;
	}

	// Add the offset for this property index.
	m_Offsets.push_back(offset);
}


Uint32 Material::ResizeAlignBuffer(Uint32 byteCount)
{
	Uint32 offset = (Uint32)m_Buffer.size();

	// Bytes remaing before boundary is crossed!
	Uint32 remainingBytes = (Uint32)(16 - ((m_Buffer.size()) % 16));
	// We would cross boundary account for it
	if (byteCount > remainingBytes)
	{
		offset += remainingBytes;
		m_Buffer.resize(m_Buffer.size() + remainingBytes + byteCount);
	}
	else
	{
		// Standard resize i.e we dont cross the boundary
		m_Buffer.resize(m_Buffer.size() + byteCount);
	}

	return offset;
}