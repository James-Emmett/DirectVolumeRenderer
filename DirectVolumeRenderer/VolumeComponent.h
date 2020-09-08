#pragma once
#include "World/Component/Component.h"
#include "Content/ContentManager.h"
#include "Content/Texture.h"
#include "World/Component/MeshRenderer.h"
#include "VolumeGenerator.h"
#include "VolumeOccupancy.h"
#include "TransferFunction.h"

enum class VolumeMethod { MIP, Alpha, PBR };

struct VolumeData
{
	float Width = 1;
	float Height = 1;
	float Depth = 1;
	float IsoValue = 0.2f;
	float AlphaAmount = 0.5f;
};

class VolumeComponent : public Component<VolumeComponent>
{
public:
	std::shared_ptr<Texture> m_VolumeMap;
	std::shared_ptr<Texture> m_OccupancyMap;
	std::shared_ptr<Texture> m_Noise;
	TransferFunction m_TransferFunction;

public:
	ContentManager* m_ContentManager = nullptr;
	GraphicsDevice* m_GraphicsDevice = nullptr;
	std::shared_ptr<MeshRenderer> m_MeshRenderer;
	std::shared_ptr<Material>	  m_VolumeMaterials[3];
	VolumeMethod				  m_VolumeMethod = VolumeMethod::PBR;
	VolumeData					  m_VolumeData;
	VolumeGenerator				  m_VolumeGenerator;
	VolumeOccupancy				  m_OccupancyGenerator;

private:
	std::string m_VolumePath;
	std::string m_VolumeMeta;
	Uint32 m_CurrentItem;
	int dimensions[3];
	bool m_ShowMetaPop = false;

public:
	// Sets up the volume materials
	void Initialize(GraphicsDevice* graphicsDevice, ContentManager* contentManager);
	// Loads the volume from raw and bakes, or loads pre-baked texture
	void LoadVolume(std::string volumePath);
	void OnGui();
	void Shutdown();
	void Something();

private:
	void UpdateMaterial();
};