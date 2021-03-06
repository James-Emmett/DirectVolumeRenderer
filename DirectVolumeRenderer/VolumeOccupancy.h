//Note:
/*
	Class to hold and run the volume gen compute shader
*/

#pragma once
#include "Content/Shader.h"
#include "Content/Texture.h"
#include "Math/Vector3.h"
#include <memory>

struct GridData
{
	Vector3 m_VolumeDims;
	float	_pad_01 = 0.0f;
	Vector3 m_VoxelsPerCell;
	float	_pad_02 = 0.0f;
};

class ContentManager;
class GraphicsDevice;
class VolumeOccupancy
{
private:
	GridData m_GridData;

	GraphicsDevice*			m_GraphicsDevice = nullptr;
	ContentManager*			m_ContentManager = nullptr;
	std::shared_ptr<Shader>	m_ComputeShader;
	std::string				m_ComputePath;
	BufferHandle			m_ConstantBuffer;
	std::shared_ptr<Texture> m_OccupancyMap;

public:
	void Initialize(GraphicsDevice* device, ContentManager* contentManager, std::string computePath);
	void GenerateVolumeGrid(std::shared_ptr<Texture> src, std::shared_ptr<Texture> transfer);
	std::shared_ptr<Texture> GetOccupancyTexture()const;
	Vector3 VoxelsPerCell()const;
	void Release();
};