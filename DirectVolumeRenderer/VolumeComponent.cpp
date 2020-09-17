#include "VolumeComponent.h"
#include "Application/Application.h"
#include "Content/Material.h"
#include "System/File.h"
#include "Math/Mathf.h"
#include "UI/ImGui_Interface.h"
#include "World/Entity.h"
#include "System/FileDialog.h"

const char* items[] = { "MIP", "ALPHA", "PBR", "PBR_ESS"};
const char* itemsMetaFormat[] = {"Uint8", "Uint16"};

void VolumeComponent::Initialize(GraphicsDevice* graphicsDevice, ContentManager* contentManager)
{
	m_ContentManager = Application::contentManager;
	m_GraphicsDevice = Application::graphicsDevice;
	m_VolumeMaterials[(Uint32)VolumeMethod::MIP]	 = std::make_shared<Material>(m_ContentManager->Load<Shader>("Assets/Shaders/Volume/Mip_Volume.shader"));
	m_VolumeMaterials[(Uint32)VolumeMethod::Alpha]	 = std::make_shared<Material>(m_ContentManager->Load<Shader>("Assets/Shaders/Volume/Alpha_Volume.shader"));
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR]	 = std::make_shared<Material>(m_ContentManager->Load<Shader>("Assets/Shaders/Volume/PBR_Volume.shader"));
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS] = std::make_shared<Material>(m_ContentManager->Load<Shader>("Assets/Shaders/Volume/PBR_Volume_ESS.shader"));

	// Load the cube
	m_MeshRenderer = m_Entity->AddComponent<MeshRenderer>();
	m_MeshRenderer->m_Mesh = m_ContentManager->Load<Mesh>("Assets/Models/cube.mesh");
	m_MeshRenderer->m_Material = m_VolumeMaterials[(Uint32)VolumeMethod::PBR];

	// load noise used for ray jiggle
	m_Noise = m_ContentManager->Load<Texture>("Assets/Textures/Noise.png");
	m_Noise->SetFilter(FilterMode::MinMagMipLinear);

	// Bind Noise
	m_VolumeMaterials[(Uint32)VolumeMethod::MIP]->SetTexture(2, m_Noise);
	m_VolumeMaterials[(Uint32)VolumeMethod::Alpha]->SetTexture(2, m_Noise);
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR]->SetTexture(3, m_Noise);
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS]->SetTexture(4, m_Noise);

	// Init our compute shader generators.
	m_VolumeGenerator.Initialize(m_GraphicsDevice, m_ContentManager, "Assets/Shaders/Compute/VolumeNormalGen.shader");
	m_OccupancyGenerator.Initialize(m_GraphicsDevice, m_ContentManager, "Assets/Shaders/Compute/VolumeIntensityGen.shader");
}

void VolumeComponent::UpdateMaterial()
{
	m_VolumeMaterials[(int)m_VolumeMethod]->SetFloat("Hounsfield", m_VolumeData.IsoValue);

	if (VolumeMethod::Alpha == m_VolumeMethod)
	{
		m_VolumeMaterials[(int)m_VolumeMethod]->SetFloat("AlphaAmount", m_VolumeData.AlphaAmount);
	}
}

void VolumeComponent::LoadVolume(std::string volumePath)
{
	if (volumePath.empty())
	{
		return;
	}

	if (m_VolumeMap && m_VolumeMap->IsDisposed() == false)
	{
		m_VolumeMap->Release();
		m_VolumeMap.reset();
	}

	m_OccupancyGenerator.Release();

	//--Get Extension--
	std::string ext = volumePath.c_str();
	ext = ext.substr(ext.find_last_of(".") + 1);
	for (int i = 0; i < ext.size(); ++i)
	{
		ext[i] = tolower(ext[i]);
	}

	std::string rawPath = volumePath.substr(0, volumePath.find_last_of("."));
	std::string volumeTransferPath = rawPath + ".transfer";

	if (ext == "raw")
	{
		// create a volume texture from raw.
		std::shared_ptr<Texture> source = m_ContentManager->Load<Texture>(volumePath);
		//source->SetWrapMode(WrapMode::Clamp);
		assert(source->IsLoaded() && "Failed to Load Source Texture");

		// Generate volume with normals
		m_VolumeMap = m_VolumeGenerator.GenerateVolume(source);
		source->Release();
	}
	else
	{
		assert(0 && "Only Raw Files Supported, Or .meta file missing!");
	}

	m_VolumeMap->SetWrapMode(WrapMode::Border);


	//--Initialize the transferFunction--
	m_TransferFunction.Initialize(volumeTransferPath);

	//--Generate intensity grid for Empty Space-Skipping--
	m_OccupancyGenerator.GenerateVolumeGrid(m_VolumeMap, m_TransferFunction.GetDiffuseTransfer());

	m_VolumeData.Width = (float)m_VolumeMap->GetWidth();
	m_VolumeData.Height = (float)m_VolumeMap->GetHeight();
	m_VolumeData.Depth = (float)m_VolumeMap->GetDepth();

	Vector3 dims = Vector3(m_VolumeData.Width, m_VolumeData.Height, m_VolumeData.Depth);
	Vector3 occupancyDims = dims / 8;

	// Calculate the max size and step size to take, its always 1/maxsize
	float maxSize = (float)Mathf::Max(m_VolumeData.Width, Mathf::Max(m_VolumeData.Height, m_VolumeData.Depth));
	Vector3 stepSize = Vector3(1.0f / (m_VolumeData.Width * (maxSize / m_VolumeData.Width)),
		1.0f / (m_VolumeData.Height * (maxSize / m_VolumeData.Height)),
		1.0f / (m_VolumeData.Depth * (maxSize / m_VolumeData.Depth)));


	// Mip Vol Textures
	m_VolumeMaterials[(Uint32)VolumeMethod::MIP]->SetTexture(0, m_VolumeMap);
	m_VolumeMaterials[(Uint32)VolumeMethod::MIP]->SetTexture(1, m_OccupancyGenerator.GetOccupancyTexture());
	m_VolumeMaterials[(Uint32)VolumeMethod::MIP]->SetVector3("VolumeDims", dims);
	m_VolumeMaterials[(Uint32)VolumeMethod::MIP]->SetVector3("OccupancyDims", occupancyDims);
	m_VolumeMaterials[(Uint32)VolumeMethod::MIP]->SetVector3("StepSize", stepSize);
	m_VolumeMaterials[(Uint32)VolumeMethod::MIP]->SetFloat("Iterations", maxSize);

	// Alpha Vol Textures
	m_VolumeMaterials[(Uint32)VolumeMethod::Alpha]->SetTexture(0, m_VolumeMap);
	m_VolumeMaterials[(Uint32)VolumeMethod::Alpha]->SetTexture(1, m_OccupancyGenerator.GetOccupancyTexture());
	m_VolumeMaterials[(Uint32)VolumeMethod::Alpha]->SetVector3("VolumeDims", dims);
	m_VolumeMaterials[(Uint32)VolumeMethod::Alpha]->SetVector3("OccupancyDims", occupancyDims);
	m_VolumeMaterials[(Uint32)VolumeMethod::Alpha]->SetVector3("StepSize", stepSize);
	m_VolumeMaterials[(Uint32)VolumeMethod::Alpha]->SetFloat("Iterations", maxSize);

	// PBR Vol Textures
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR]->SetTexture(0, m_VolumeMap);
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR]->SetTexture(1, m_TransferFunction.GetDiffuseTransfer());
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR]->SetTexture(2, m_TransferFunction.GetSurfaceTransfer());
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR]->SetVector3("VolumeDims", dims);
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR]->SetVector3("StepSize", stepSize);
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR]->SetFloat("Iterations", maxSize);


	// PBR ESS
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS]->SetTexture(0, m_VolumeMap);
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS]->SetTexture(1, m_OccupancyGenerator.GetOccupancyTexture());
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS]->SetTexture(2, m_TransferFunction.GetDiffuseTransfer());
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS]->SetTexture(3, m_TransferFunction.GetSurfaceTransfer());
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS]->SetVector3("VolumeDims", dims);
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS]->SetVector3("OccupancyDims", dims / m_OccupancyGenerator.VoxelsPerCell());
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS]->SetVector3("StepSize", stepSize);
	m_VolumeMaterials[(Uint32)VolumeMethod::PBR_ESS]->SetFloat("Iterations", maxSize);

	UpdateMaterial();
}

void VolumeComponent::OnGui()
{
	bool dirty = false;
	if (ImGui::Begin("Volume"))
	{
		if (m_ShowMetaPop == false)
		{
			if (ImGui::Button("Open"))
			{
				if (FileDialog::OpenDialog("Raw Files (*.raw)|*.raw", "", m_VolumePath) == DialogResult::Ok)
				{
					m_VolumeMeta = m_VolumePath + ".meta";
					if (File::Exists(m_VolumeMeta) == false)
					{
						m_ShowMetaPop = true;
					}
					else
					{
						LoadVolume(m_VolumePath);
					}
				}
			}
		}
		else
		{
			ImGui::OpenPopup("EnterMeta");
			if (ImGui::BeginPopupModal("EnterMeta"))
			{
				ImGui::InputInt3("Dimensions: ", dimensions);

				if (ImGui::BeginCombo("##combo", itemsMetaFormat[m_CurrentItem]))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (itemsMetaFormat[m_CurrentItem] == items[n]);
						if (ImGui::Selectable(items[n], is_selected))
						{
							m_CurrentItem = n;
						}

						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Ok"))
				{
					m_ShowMetaPop = false;
					File file;
					file.Open(m_VolumeMeta.c_str(), FileMode::Write);
					file.WriteLine(std::string("Width="  + std::to_string(dimensions[0])));
					file.WriteLine(std::string("Height=" + std::to_string(dimensions[1])));
					file.WriteLine(std::string("Depth="  + std::to_string(dimensions[2])));
					file.WriteLine("NumDims=3");
					file.WriteLine("Format=" + std::string(itemsMetaFormat[m_CurrentItem]));
					file.Close();
					LoadVolume(m_VolumePath);
				}

				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					m_ShowMetaPop = false;
				}
				ImGui::EndPopup();
			}
		}

		//--All the Volume Data stuff--
		if (ImGui::CollapsingHeader("Volume Data"))
		{
			if (ImGui::BeginCombo("##combo", items[(Uint32)m_VolumeMethod]))
			{
				for (Uint32 n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = ((Uint32)m_VolumeMethod == n);
					if (ImGui::Selectable(items[n], is_selected))
					{
						m_VolumeMethod = (VolumeMethod)n;
						m_MeshRenderer->m_Material = m_VolumeMaterials[(Uint32)m_VolumeMethod];
						dirty = true;
					}

					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::SliderFloat("Iso Value", &m_VolumeData.IsoValue, 0.001f, 1.0f))
			{
				dirty = true;
			}

			if (m_VolumeMethod == VolumeMethod::Alpha)
			{
				if (ImGui::SliderFloat("AlphaAmount", &m_VolumeData.AlphaAmount, 0.001f, 1.0f))
				{
					dirty = true;
				}
			}

			if (m_VolumeMethod == VolumeMethod::PBR || m_VolumeMethod == VolumeMethod::PBR_ESS)
			{
				if (m_TransferFunction.DisplayEditor())
				{
					m_RequiresUpdate = true;
				}
			}

			if (m_VolumeMethod == VolumeMethod::PBR_ESS && m_RequiresUpdate && m_TransferFunction.IsUserInteracting() == false)
			{
				m_OccupancyGenerator.GenerateVolumeGrid(m_VolumeMap, m_TransferFunction.GetDiffuseTransfer());
				m_RequiresUpdate = false;
			}
		}
	}
	ImGui::End();

	if (dirty)
	{
		UpdateMaterial();
	}
}

void VolumeComponent::Shutdown()
{
	m_TransferFunction.ShutDown();
}