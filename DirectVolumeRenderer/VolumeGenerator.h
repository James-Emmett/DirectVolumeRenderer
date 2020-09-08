//Note:
/*
	Class to hold and run the volume gen compute shader
*/

#pragma once
#include "Content/Shader.h"
#include "Content/Texture.h"
#include <memory>

struct DataRange
{
	float _MinValue = 0;
	float _MaxValue = 255;
	float _Range = 255;
	float _InitialValue = 255;
};

class ContentManager;
class GraphicsDevice;
class VolumeGenerator
{
private:
	GraphicsDevice* m_GraphicsDevice = nullptr;
	ContentManager* m_ContentManager = nullptr;
	std::shared_ptr<Shader>	m_ComputeShader;
	std::string				m_ComputePath;
	BufferHandle			m_ConstantBuffer;

public:
	void Initialize(GraphicsDevice* device, ContentManager* contentManager, std::string computePath);
	std::shared_ptr<Texture> GenerateVolume(std::shared_ptr<Texture> src);
	Vector2 GetRange(std::shared_ptr<Texture> src)const;
};