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

std::shared_ptr<Texture> VolumeOccupancy::GenerateVolumeGrid(std::shared_ptr<Texture> source)
{
	Uint32 width = source->GetWidth();
	Uint32 height = source->GetHeight();
	Uint32 depth = source->GetDepth();

	m_GridData.m_VolumeDims.x = (float)width;
	m_GridData.m_VolumeDims.y = (float)height;
	m_GridData.m_VolumeDims.z = (float)depth;
	m_GridData.m_VoxelsPerCell = Vector3(8, 8, 8);

	m_GraphicsDevice->UpdateBuffer(m_ConstantBuffer, (Byte*)&m_GridData, sizeof(GridData));
	m_GraphicsDevice->BindConstantBuffer(m_ConstantBuffer, 0);

	// Bind source
	source->Bind(0);

	// Bind a UAV texture for Compute Result
	Texture computeResult;
	computeResult.Create3D(width / 8, height / 8, depth / 8, BufferUsage::Default, SurfaceFormat::R8_Unorm);
	computeResult.SetUnorderedAccess();
	computeResult.Apply();
	computeResult.Bind(0, ShaderType::CS);

	Vector3 dispatch = Vector3(((width / m_GridData.m_VoxelsPerCell.x) / 8), ((height / m_GridData.m_VoxelsPerCell.y) / 8),
		((depth / m_GridData.m_VoxelsPerCell.z) / 8));

	// Run Compute Shader
	m_ComputeShader->Bind();
	m_GraphicsDevice->Dispatch((Uint32)dispatch.x, (Uint32)dispatch.y, (Uint32)dispatch.z);
	//m_GraphicsDevice->Dispatch(width, height, depth);

	std::shared_ptr<Texture> volume = std::make_shared<Texture>();
	volume->Create3D(width / 8, height / 8, depth / 8, BufferUsage::Immutable, SurfaceFormat::R8_Unorm);
	computeResult.GetGPUData(volume->GetData(), volume->GetByteCount());
	volume->Apply();

	//computeResult.Release();
	return volume;
}