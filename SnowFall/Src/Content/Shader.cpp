#include "Content/Shader.h"
#include "Application/Application.h"
#include "Graphics/CommonStates.h"
#include "Tinyxml/tinyxml2.h"
#include "System/Hash32.h"
#include "System/File.h"
#include "Application/GameSettings.h"

const BlendDesc blendTable[4] = { CommonStates::Opaque,  CommonStates::AlphaBlend, CommonStates::Additive,  CommonStates::NonPremultiplied };
const DepthDesc depthTable[3] = { CommonStates::DepthNone,  CommonStates::DepthDefault, CommonStates::DepthRead };
const RasterDesc rasterTable[4] = { CommonStates::CullNone,  CommonStates::CullCounterClockwise, CommonStates::CullClockwise, CommonStates::Wireframe };

//Maps attribute semantic to its format, cannot think of a better way atm
const std::unordered_map<std::string, SurfaceFormat> attributeMap =
{
	{ "POSITION" , SurfaceFormat::R32G32B32_Float},
	{ "NORMAL"   , SurfaceFormat::R32G32B32_Float},
	{ "TANGENT"  , SurfaceFormat::R32G32B32A32_Float},
	{ "COLOR"	 , SurfaceFormat::R32G32B32A32_Float},
	{ "TEXCOORD" , SurfaceFormat::R32G32_Float},
};

Shader::Shader()
{
	// Use dependency Injection in the future.
	m_GraphicsDevice = Application::graphicsDevice;
}

Shader::~Shader()
{
	Release();
}

Property& Shader::operator[](int id)
{
	assert(id < m_Properties.size() && "Invalid ID");
	return m_Properties[id];
}

int Shader::PropertyToId(std::string name)const
{
	for (size_t i = 0; i < m_Properties.size(); i++)
	{
		if (m_Properties[i].m_Name == name)
		{
			return (int)i;
		}
	}

	return -1;
}

int Shader::GetPropertyCount() const
{
	return (int)m_Properties.size();
}

std::string Shader::GetPropertyName(int id) const
{
	assert(id < m_Properties.size() && "Propety ID Out Of range");
	return m_Properties[id].m_Name;
}

Vector2 Shader::PropertyRange(int id) const
{
	assert(id < m_Properties.size() && "Property ID out Of Range");
	return m_Properties[id].m_Range;
}

PropertyType Shader::GetPropertyType(int id)const
{
	assert(id < m_Properties.size() && "Propety ID Out Of range");
	return m_Properties[id].m_Type;
}

const std::vector<Property>& Shader::PropertyList()const
{
	return m_Properties;
}

std::vector<Property>& Shader::PropertyList()
{
	return m_Properties;
}

const Property& Shader::GetProperty(int id) const
{
	assert(id < m_Properties.size() && "Propety ID Out Of range");
	return m_Properties[id];
}

Property& Shader::GetProperty(int id)
{
	assert(id < m_Properties.size() && "Propety ID Out Of range");
	return m_Properties[id];
}

RenderType Shader::GetRenderQueue() const
{
	return m_RenderQueue;
}

void Shader::LoadFromFile(const std::string& filePath)
{
	assert(filePath.empty() == false);

	//--Get Extension--
	std::string path = filePath.c_str();
	size_t extPos = path.find_last_of(".");
	size_t endDir = path.find_last_of("//");
	std::string ext = path.substr(extPos + 1);

	m_FilePath = filePath;
	m_Name = filePath.substr((int)(endDir + 1), (int)((extPos - endDir) - 1));

	if (ext == "shader")
	{
		LoadFromXML();
	}
	else if (ext == "ShaderBinary")
	{
		LoadFromBinary();
	}
}

void Shader::Reload()
{
	if (!m_FilePath.empty())
	{
		Release();
		LoadFromFile(m_FilePath.c_str());
		return;
	}
}

void Shader::OnGui()
{
	// Nothing
}

bool Shader::SaveToFile(std::string fileName)
{
	std::string filePath = fileName;
	filePath += ".MaterialBinary";

	BinaryFile file;
	if (file.Open(filePath.c_str(), FileMode::Write))
	{
		// Write Property count
		Uint32 propertyCount = (Uint32)m_Properties.size();
		file.WriteDword(propertyCount);

		// Save each property out.
		for (Uint32 i = 0; i < propertyCount; i++)
		{
			Property* property = &m_Properties[i];
			file.WriteString(property->m_Name.c_str());
			file.WriteDword((Dword)property->m_Type);
			file.WriteFloat(property->m_Range.x);
			file.WriteFloat(property->m_Range.y);
			file.WriteByte(property->m_IsRanged);
		}

		// Write States
		file.WriteDword((Dword)m_Blend);
		file.WriteDword((Dword)m_DepthMode);
		file.WriteDword((Dword)m_RasterMode);
		file.WriteDword((Dword)m_PipeDesc.Topology);

		// Write inputlayout
		file.WriteDword(m_PipeDesc.InputLayout.Size());
		for (size_t i = 0; i < m_PipeDesc.InputLayout.Size(); i++)
		{
			InputElement& element = m_PipeDesc.InputLayout.m_LayoutElements[i];
			file.WriteString(element.SemanticName);
			file.WriteDword(element.SemanticIndex);
			file.WriteDword((Dword)element.Format);
			file.WriteDword(element.InputSlot);
			file.WriteDword(element.AlignedByteOffset);
			file.WriteDword((Dword)element.InputSlotClass);
			file.WriteDword((Dword)element.InstanceDataStepRate);
		}

		// How many shaders there is to read.
		file.WriteDword(m_ShaderCount);

		// size of shader in bytes
		ShaderType type = (ShaderType)file.ReadDword();

		if (m_PipeDesc.VertexShader.IsValid())
		{
			const GraphicsShader* shader = m_GraphicsDevice->GetShader(m_PipeDesc.VertexShader);
			file.WriteDword(shader->m_Length);
			file.WriteBuffer(shader->m_ByteCode, shader->m_Length);
		}

		if (m_PipeDesc.PixelShader.IsValid())
		{
			const GraphicsShader* shader = m_GraphicsDevice->GetShader(m_PipeDesc.PixelShader);
			file.WriteDword(shader->m_Length);
			file.WriteBuffer(shader->m_ByteCode, shader->m_Length);
		}

		if (m_PipeDesc.HullShader.IsValid())
		{
			const GraphicsShader* shader = m_GraphicsDevice->GetShader(m_PipeDesc.HullShader);
			file.WriteDword(shader->m_Length);
			file.WriteBuffer(shader->m_ByteCode, shader->m_Length);
		}

		if (m_PipeDesc.DomainShader.IsValid())
		{
			const GraphicsShader* shader = m_GraphicsDevice->GetShader(m_PipeDesc.DomainShader);
			file.WriteDword(shader->m_Length);
			file.WriteBuffer(shader->m_ByteCode, shader->m_Length);
		}

		if (m_PipeDesc.GeometryShader.IsValid())
		{
			const GraphicsShader* shader = m_GraphicsDevice->GetShader(m_PipeDesc.GeometryShader);
			file.WriteDword(shader->m_Length);
			file.WriteBuffer(shader->m_ByteCode, shader->m_Length);
		}
	}

	return true;
}

void Shader::Release()
{
	m_GraphicsDevice->DestroyPipeline(m_Pipeline);
}

const PipelineHandle Shader::GetPipeline() const
{
	return m_Pipeline;
}

void Shader::Bind() const
{
	if (m_IsCompute == false)
	{
		m_GraphicsDevice->BindPipelineState(m_Pipeline);
	}
	else
	{
		m_GraphicsDevice->BindComputeShader(m_ComputeShader);
	}
}

void Shader::LoadFromBinary()
{
	// Some defaults.
	m_PipeDesc.DepthState = CommonStates::DepthDefault;
	m_PipeDesc.BlendState = CommonStates::Opaque;
	m_PipeDesc.RasterState = CommonStates::CullCounterClockwise;

	BinaryFile file;
	if (file.Open(m_FilePath.c_str(), FileMode::Read))
	{
		// Read all of the Properties
		Uint32 propertyCount = file.ReadDword();
		for (Uint32 i = 0; i < propertyCount; i++)
		{
			Property property;
			property.m_Name = file.ReadString().c_str();
			property.m_Type = (PropertyType)file.ReadDword();
			property.m_Range.x = file.ReadFloat();
			property.m_Range.y = file.ReadFloat();
			property.m_IsRanged = file.ReadByte();
			m_Properties.push_back(property);
		}

		// Read States
		m_PipeDesc.BlendState  = blendTable[file.ReadDword()];
		m_PipeDesc.DepthState  = depthTable[file.ReadDword()];
		m_PipeDesc.RasterState = rasterTable[file.ReadDword()];
		m_PipeDesc.Topology = (PrimitiveTopology)file.ReadDword();

		// Write inputlayout
		int count = file.ReadDword();
		for (int i = 0; i < count; ++i)
		{
			InputElement element;
			element.SemanticName = file.ReadString();
			element.SemanticIndex = file.ReadDword();
			element.Format = (SurfaceFormat)file.ReadDword();
			element.InputSlot = file.ReadDword();
			element.AlignedByteOffset = file.ReadDword();
			element.InputSlotClass = (InputType)file.ReadDword();
			element.InstanceDataStepRate = file.ReadDword();
			m_PipeDesc.InputLayout.Add(element);
		}

		// How many shaders there is to read.
		Uint32 shaderCount = file.ReadDword();
		for (Uint32 i = 0; i < shaderCount; i++)
		{
			// size of shader in bytes
			ShaderType type = (ShaderType)file.ReadDword();
			Uint32 shaderSize = file.ReadDword();
			Byte* bytes = new Byte[shaderSize];
			file.ReadBuffer(bytes, shaderSize);

			switch (type)
			{
			case ShaderType::VS:
				m_PipeDesc.VertexShader = m_GraphicsDevice->CreateShader(type, bytes, shaderSize);
				break;
			case ShaderType::PS:
				m_PipeDesc.PixelShader = m_GraphicsDevice->CreateShader(type, bytes, shaderSize);
				break;
			case ShaderType::HS:
				m_PipeDesc.HullShader = m_GraphicsDevice->CreateShader(type, bytes, shaderSize);
				break;
			case ShaderType::DS:
				m_PipeDesc.DomainShader = m_GraphicsDevice->CreateShader(type, bytes, shaderSize);
				break;
			case ShaderType::GS:
				m_PipeDesc.GeometryShader = m_GraphicsDevice->CreateShader(type, bytes, shaderSize);
				break;
			default:
				break;
			}

			delete[] bytes;
		}

		m_Pipeline = m_GraphicsDevice->CreatePipeline(&m_PipeDesc);
	}
}

void Shader::LoadFromXML()
{
	// Some defaults.
	m_PipeDesc.DepthState	= CommonStates::DepthDefault;
	m_PipeDesc.BlendState	= CommonStates::Opaque;
	m_PipeDesc.RasterState	= CommonStates::CullCounterClockwise;
	m_PipeDesc.Topology		= PrimitiveTopology::TriangleList;

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(m_FilePath.c_str()) == tinyxml2::XMLError::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pRootElement = doc.FirstChildElement("Shader");
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

					if (pPropertyElement->Attribute("min") && pPropertyElement->Attribute("max"))
					{
						m_Properties.push_back(Property(pPropertyElement->Attribute("name"), propertyType, true, Vector2(std::stof(pPropertyElement->Attribute("min")), std::stof(pPropertyElement->Attribute("max")))));
					}
					else
					{
						m_Properties.push_back(Property(pPropertyElement->Attribute("name"), propertyType, false, Vector2::Zero));

					}

					pPropertyElement = pPropertyElement->NextSiblingElement("Property");
				}
			}

			//--Load Render States--
			tinyxml2::XMLElement* pRenderState = pRootElement->FirstChildElement("RenderState");
			if (pRenderState != nullptr)
			{
				//--Blend State--
				tinyxml2::XMLElement* pBlendState = pRenderState->FirstChildElement("BlendState");
				if (pBlendState != nullptr)
				{
					std::string blendText = pBlendState->GetText();
					if (blendText == "Opaque")
					{
						m_Blend = ShaderBlendMode::Opaque;
					}
					else if (blendText == "AlphaBlend")
					{
						m_Blend = ShaderBlendMode::AlphaBlend;
					}
					else if (blendText == "Additive")
					{
						m_Blend = ShaderBlendMode::Additive;
					}
					else if (blendText == "NonPremultiplied")
					{
						m_Blend = ShaderBlendMode::NonPremultiplied;
					}

					m_PipeDesc.BlendState = blendTable[(Uint32)m_Blend];
				}

				//--Depth State--
				tinyxml2::XMLElement* pDepth = pRenderState->FirstChildElement("DepthState");
				if (pDepth != nullptr)
				{
					std::string depthText = pDepth->GetText();
					if (depthText == "DepthNone")
					{
						m_DepthMode = ShaderDepthMode::DepthNone;
					}
					else if (depthText == "DepthDefault")
					{
						m_DepthMode = ShaderDepthMode::DepthDefault;
					}
					else if (depthText == "DepthRead")
					{
						m_DepthMode = ShaderDepthMode::DepthRead;
					}
					m_PipeDesc.DepthState = depthTable[(Uint32)m_DepthMode];
				}

				//--Raster State--
				tinyxml2::XMLElement* pRaster = pRenderState->FirstChildElement("RasterState");
				if (pRaster != nullptr)
				{
					std::string rasterText = pRaster->GetText();
					if (rasterText == "CullNone")
					{
						m_RasterMode = ShaderRasterMode::CullNone;
					}
					else if (rasterText == "CullClockwise")
					{
						m_RasterMode = ShaderRasterMode::CullClockwise;
					}
					else if (rasterText == "CullCounterClockwise")
					{
						m_RasterMode = ShaderRasterMode::CullCounterClockwise;
					}
					else if (rasterText == "Wireframe")
					{
						m_RasterMode = ShaderRasterMode::Wireframe;
					}
					m_PipeDesc.RasterState = rasterTable[(Uint32)m_RasterMode];
				}

				//--topology State--
				tinyxml2::XMLElement* pTopology = pRenderState->FirstChildElement("Topology");
				if (pTopology != nullptr)
				{
					std::string topology = pTopology->GetText();
					if (topology == "PointList")
					{
						m_PipeDesc.Topology = PrimitiveTopology::PointList;
					}
					else if (topology == "LineList")
					{
						m_PipeDesc.Topology = PrimitiveTopology::LineList;
					}
					else if (topology == "TriangleList")
					{
						m_PipeDesc.Topology = PrimitiveTopology::TriangleList;
					}
					else if (topology == "TriangleStrip")
					{
						m_PipeDesc.Topology = PrimitiveTopology::TriangleStrip;
					}
					else if (topology == "PatchList")
					{
						m_PipeDesc.Topology = PrimitiveTopology::PatchList;
					}
				}
			}

			//--Load Render Queue--
			tinyxml2::XMLElement* pRenderQueue = pRootElement->FirstChildElement("RenderQueue");
			if (pRenderQueue != nullptr)
			{
				std::string type = pRenderQueue->Attribute("type");
				if (type == "Geometry")
				{
					m_RenderQueue = RenderType::Opaque;
				}
				else if (type == "Transparent")
				{
					m_RenderQueue = RenderType::Transparent;
				}
			}

			//--Load shaders--
			tinyxml2::XMLElement* pShaderList = pRootElement->FirstChildElement("Shaders");
			if (pShaderList != nullptr)
			{
				tinyxml2::XMLElement* pShader = pShaderList->FirstChildElement("ShaderPath");
				while (pShader != nullptr)
				{
					std::string source	= pShader->Attribute("source");
					std::string entry	= pShader->Attribute("entry");
					std::string type	= pShader->Attribute("type");
					Byte* bytes = nullptr;
					Uint32 length = 0;

					if (strcmp(type.c_str(), "VS") == 0)
					{
						m_GraphicsDevice->CompileShader(source, entry.c_str(), "vs_5_0", &bytes, length);
						m_PipeDesc.VertexShader = m_GraphicsDevice->CreateShader(ShaderType::VS, bytes, length);
					}
					else if (strcmp(type.c_str(), "FS") == 0)
					{
						m_GraphicsDevice->CompileShader(source, entry.c_str(), "ps_5_0", &bytes, length);
						m_PipeDesc.PixelShader = m_GraphicsDevice->CreateShader(ShaderType::PS, bytes, length);
					}
					else if (strcmp(type.c_str(), "HS") == 0)
					{
						m_GraphicsDevice->CompileShader(source, entry.c_str(), "hs_5_0", &bytes, length);
						m_PipeDesc.HullShader = m_GraphicsDevice->CreateShader(ShaderType::HS, bytes, length);
					}
					else if (strcmp(type.c_str(), "DS") == 0)
					{
						m_GraphicsDevice->CompileShader(source, entry.c_str(), "ds_5_0", &bytes, length);
						m_PipeDesc.DomainShader = m_GraphicsDevice->CreateShader(ShaderType::DS, bytes, length);
					}
					else if (strcmp(type.c_str(), "GS") == 0)
					{
						m_GraphicsDevice->CompileShader(source, entry.c_str(), "gs_5_0", &bytes, length);
						m_PipeDesc.GeometryShader = m_GraphicsDevice->CreateShader(ShaderType::CS, bytes, length);
					}
					else if (strcmp(type.c_str(), "CS") == 0)
					{
						m_GraphicsDevice->CompileShader(source, entry.c_str(), "cs_5_0", &bytes, length);
						m_ComputeShader = m_GraphicsDevice->CreateShader(ShaderType::CS, bytes, length);
						m_IsCompute = true;
					}

					if (bytes)
					{
						delete[] bytes;
					}
					pShader = pShader->NextSiblingElement("ShaderPath");
					++m_ShaderCount;
				}
			}
			else
			{
				assert(0 && "No Shader Defined");
			}


			//--Load Attributes--
			tinyxml2::XMLElement* pAttributes = pRootElement->FirstChildElement("Attributes");
			if (pAttributes != nullptr)
			{
				InputLayoutDesc layoutDesc;
				tinyxml2::XMLElement* pAttribute = pAttributes->FirstChildElement("Attribute");
				while (pAttribute != nullptr)
				{
					// Get data
					std::string name = pAttribute->Attribute("name");
					if (name.empty()) { assert(0 && "Attribute Invalid."); }

					std::string index = pAttribute->Attribute("index");
					if (index.empty()) { assert(0 && "Attribute Invalid."); }

					std::string slotClass = pAttribute->Attribute("slotClass");
					if (slotClass.empty()) { assert(0 && "Attribute Invalid."); }

					std::string stepRate = pAttribute->Attribute("stepRate");
					if (stepRate.empty()) { assert(0 && "Attribute Invalid."); }

					std::unordered_map<std::string, SurfaceFormat>::const_iterator itr = attributeMap.find(name);
					if (itr == attributeMap.end())
					{
						assert(0 && "Semantic name not recognised");
					}

					InputElement element;
					element.SemanticName = name.c_str();
					element.SemanticIndex = std::stoi(index);
					element.Format = itr->second;
					element.InputSlot = 0;  // Interleved vertex layouts only!
					element.AlignedByteOffset = 0xffffffff;
					element.InputSlotClass = (slotClass == "PER_VERTEX") ? InputType::VertexData : InputType::InstanceData;
					element.InstanceDataStepRate = std::stoi(stepRate);

					layoutDesc.Add(element);
					pAttribute = pAttribute->NextSiblingElement("Attribute");
				}

				m_PipeDesc.InputLayout = layoutDesc;
			}


			if (m_IsCompute == false)
			{
				// Everything loaded, now set the pipline and return
				m_Pipeline = m_GraphicsDevice->CreatePipeline(&m_PipeDesc);
				assert(m_Pipeline.IsValid() && "Failed To create Shader Pipline");
			}
		}
	}
}