#pragma once
#include "Resource.h"
#include "Graphics/GraphicsDevice.h"

// Crazy numbers are hashes for a faster check than strcmp 8 times
enum class PropertyType : Uint32
{
	Undefined	= 0,
	Color		= 2811717613,
	Vector4		= 3232351376,
	Vector3		= 1590513971,
	Vector2		= 701129125,
	Float		= 140800401,
	Int			= 740072529,
	Texture		= 1306211566
};

struct Property
{
	std::string	 m_Name		= "Default";
	PropertyType m_Type		= PropertyType::Undefined;
	Vector2		 m_Range	= Vector2::Zero;
	bool		 m_IsRanged = false;

	Property() {}
	Property(std::string name, PropertyType type, bool IsRanged, Vector2 range)
	{
		m_Name		= name;
		m_Type		= type;
		m_IsRanged	= IsRanged;
		m_Range		= range;
	}
};

enum class ShaderBlendMode { Opaque, AlphaBlend, Additive, NonPremultiplied };
enum class ShaderDepthMode { DepthNone, DepthDefault, DepthRead };
enum class ShaderRasterMode { CullNone, CullCounterClockwise, CullClockwise, Wireframe };


class Shader : public Resource
{
private:
	GraphicsDevice*			m_GraphicsDevice = nullptr;
	PipelineHandle			m_Pipeline;	 // Shader holds a pipline state handle
	PipelineDesc			m_PipeDesc;  // Keep unless ClearCpuData(), Shaderallocator CLEARS it by default.
	std::vector<Property>	m_Properties;
	RenderType				m_RenderQueue = RenderType::Opaque; // Use to sort into render buckets in future!
	Uint32					m_ShaderCount = 0;

	//--Compute Shader--
	ShaderHandle m_ComputeShader;
	bool		 m_IsCompute = false;

	//--Render States--
	ShaderBlendMode			m_Blend = ShaderBlendMode::Opaque;
	ShaderDepthMode			m_DepthMode = ShaderDepthMode::DepthDefault;
	ShaderRasterMode		m_RasterMode = ShaderRasterMode::CullCounterClockwise;

public:
	Shader();
	~Shader();

public:
	Property& operator[](int id);

public:
	int PropertyToId(std::string name)const;
	int GetPropertyCount()const;
	std::string GetPropertyName(int id)const;
	Vector2 PropertyRange(int id)const;
	PropertyType GetPropertyType(int id)const;
	const std::vector<Property>& PropertyList()const;
	std::vector<Property>& PropertyList();
	const Property& GetProperty(int id)const;
	Property& GetProperty(int id);
	RenderType GetRenderQueue()const;
	void LoadFromFile(const std::string& filePath);
	void Reload();
	void OnGui();
	bool SaveToFile(std::string fileName);
	void Release();
	const PipelineHandle GetPipeline()const;
	void Bind()const;

private:
	void LoadFromBinary();
	void LoadFromXML();
};