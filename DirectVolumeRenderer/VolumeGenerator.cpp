#include "VolumeGenerator.h"
#include "Content/ContentManager.h"
#include "Math/Mathf.h"

void VolumeGenerator::Initialize(GraphicsDevice* device, ContentManager* contentManager, std::string computePath)
{
	if (m_ComputeShader == nullptr || computePath != m_ComputePath)
	{
		m_GraphicsDevice = device;
		m_ContentManager = contentManager;
		m_ComputePath = computePath;
		m_ComputeShader = m_ContentManager->Load<Shader>(m_ComputePath);

		if (m_ConstantBuffer.IsValid() == false)
		{
			m_ConstantBuffer = m_GraphicsDevice->CreateConstantBuffer(sizeof(DataRange), nullptr);
		}
	}
}

std::shared_ptr<Texture> VolumeGenerator::GenerateVolume(std::shared_ptr<Texture> source)
{
	Uint32 width = source->GetWidth();
	Uint32 height = source->GetHeight();
	Uint32 depth = source->GetDepth();

	// Bind source
	source->Bind(0);

	Vector2 range = GetRange(source);

	DataRange dataRange;
	dataRange._MinValue = range.x;
	dataRange._MaxValue = range.y;
	dataRange._Range = (range.y - range.x);

	SurfaceFormat format = source->GetFormat();

	if (BytesPerBlock(format) == 2)
	{
		dataRange._InitialValue = 65535;
	}

	m_GraphicsDevice->UpdateBuffer(m_ConstantBuffer, (Byte*)&dataRange, sizeof(DataRange));
	m_GraphicsDevice->BindConstantBuffer(m_ConstantBuffer, 0);

	// Bind a UAV texture for Compute Result
	Texture computeResult;
	computeResult.Create3D(width, height, depth, BufferUsage::Default, SurfaceFormat::R8G8B8A8_Uint);
	computeResult.SetUnorderedAccess();
	computeResult.Apply();
	computeResult.Bind(0, ShaderType::CS);

	// Run Compute Shader
	m_ComputeShader->Bind();
	m_GraphicsDevice->Dispatch(width / 8, height / 8, depth / 8);

	std::shared_ptr<Texture> volume = std::make_shared<Texture>();
	volume->Create3D(width, height, depth, BufferUsage::Immutable, SurfaceFormat::R8G8B8A8_Unorm);
	computeResult.GetGPUData(volume->GetData(), volume->GetByteCount());
	volume->Apply();

	return volume;
}

Vector2 VolumeGenerator::GetRange(std::shared_ptr<Texture> src) const
{
	int bpp = BytesPerBlock(src->GetFormat());
	if (bpp != 2)
	{
		return Vector2(0, 255);
	}

	Vector2 range = Vector2(900000, -10000);
	int length = src->GetWidth() * src->GetHeight() * src->GetDepth();
	Byte* data = src->GetData();
	for (size_t i = 0; i < length; i++)
	{
		float value = ((Uint16*)(data))[i];
		range.x = Mathf::Min(range.x, value);
		range.y = Mathf::Max(range.y, value);
	}

	return range;
}
