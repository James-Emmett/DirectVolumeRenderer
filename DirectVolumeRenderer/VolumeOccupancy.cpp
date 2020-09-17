#include "VolumeOccupancy.h"
#include "Content/ContentManager.h"
#include "Content/Shader.h"

void VolumeOccupancy::Initialize(GraphicsDevice* device, ContentManager* contentManager, std::string computePath)
{
	if (m_ComputeShader == nullptr || computePath != m_ComputePath)
	{
		m_GraphicsDevice = device;
		m_ContentManager = contentManager;
		m_ComputePath = computePath;
		m_ComputeShader = m_ContentManager->Load<Shader>(m_ComputePath);

		if (m_ConstantBuffer.IsValid() == false)
		{
			m_ConstantBuffer = m_GraphicsDevice->CreateConstantBuffer(sizeof(GridData), (Byte*)&m_GridData);
		}
	}
}

void VolumeOccupancy::GenerateVolumeGrid(std::shared_ptr<Texture> source, std::shared_ptr<Texture> transfer)
{
	Uint32 width = source->GetWidth();
	Uint32 height = source->GetHeight();
	Uint32 depth = source->GetDepth();

	m_GridData.m_VolumeDims.x = (float)width;
	m_GridData.m_VolumeDims.y = (float)height;
	m_GridData.m_VolumeDims.z = (float)depth;
	m_GridData.m_VoxelsPerCell = Vector3(4, 4, 4);

	m_GraphicsDevice->UpdateBuffer(m_ConstantBuffer, (Byte*)&m_GridData, sizeof(GridData));
	m_GraphicsDevice->BindConstantBuffer(m_ConstantBuffer, 0);

	// Bind source & Transfer
	source->Bind(0);
	transfer->Bind(1);

	// Bind a UAV texture for Compute Result
	Texture computeResult;
	computeResult.Create3D(width / m_GridData.m_VoxelsPerCell.x, height / m_GridData.m_VoxelsPerCell.y, depth / m_GridData.m_VoxelsPerCell.z, BufferUsage::Default, SurfaceFormat::R8_Unorm);
	computeResult.SetUnorderedAccess();
	computeResult.Apply();
	computeResult.Bind(0, ShaderType::CS);

	// 8,8,8 threads per group so dispatch is  (size / voxels per cell) / 8 that way we know how many dispatchs to groups we need.
	Vector3 dispatch = Vector3(((width / m_GridData.m_VoxelsPerCell.x) / 8), ((height / m_GridData.m_VoxelsPerCell.y) / 8),
		((depth / m_GridData.m_VoxelsPerCell.z) / 8));

	// Run Compute Shader
	m_ComputeShader->Bind();
	m_GraphicsDevice->Dispatch((Uint32)dispatch.x, (Uint32)dispatch.y, (Uint32)dispatch.z);
	//m_GraphicsDevice->Dispatch(width, height, depth);

	// Create a new occupancy map for storage.
	if (m_OccupancyMap == nullptr)
	{
		m_OccupancyMap = std::make_shared<Texture>();
		m_OccupancyMap->Create3D(width / m_GridData.m_VoxelsPerCell.x, height / m_GridData.m_VoxelsPerCell.y, depth / m_GridData.m_VoxelsPerCell.z, BufferUsage::Dynamic, SurfaceFormat::R8_Unorm);
		m_OccupancyMap->SetFilter(FilterMode::MaximunMinMagMipLinear);
		m_OccupancyMap->SetWrapMode(WrapMode::Clamp);
	}

	computeResult.GetGPUData(m_OccupancyMap->GetData(), m_OccupancyMap->GetByteCount());
	m_OccupancyMap->Apply(true);
	computeResult.Release();
}

std::shared_ptr<Texture> VolumeOccupancy::GetOccupancyTexture() const
{
	return m_OccupancyMap;
}

Vector3 VolumeOccupancy::VoxelsPerCell() const
{
	return m_GridData.m_VoxelsPerCell;
}

void VolumeOccupancy::Release()
{
	if (m_OccupancyMap && m_OccupancyMap->IsDisposed() == false)
	{
		m_OccupancyMap->Release();
		m_OccupancyMap.reset();
	}

}