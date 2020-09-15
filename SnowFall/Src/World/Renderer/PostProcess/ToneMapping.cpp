#include "World/Renderer/PostProcess/ToneMapping.h"
#include "Content/ContentManager.h"
#include "Application/Application.h"
#include "Content/Shader.h"
#include "imgui/imgui.h"

const char* toneItems[] = { "Reinhard", "Uncharted" };

void ToneMapping::Initialize(GraphicsDevice* device)
{
	m_GraphicsDevice = device;
	m_Effect = Application::contentManager->Load<Shader>("Assets/Shaders/PostProcess/tonemap.shader");
	m_ToneConstBuffer = m_GraphicsDevice->CreateConstantBuffer(sizeof(ToneMappingSettings), (Byte*)&m_ToneSettings);
}

void ToneMapping::Release()
{
	m_Effect.reset();
	m_GraphicsDevice->DestroyBuffer(m_ToneConstBuffer);
	m_GraphicsDevice = nullptr;
}

void ToneMapping::Apply(RenderHandle source)
{
	m_GraphicsDevice->BindConstantBuffer(m_ToneConstBuffer, 2);
	m_GraphicsDevice->BlitToBound(source, m_Effect->GetPipeline());
}

void ToneMapping::OnGui()
{
	float dirty = false;
	if (ImGui::TreeNode("ToneMapping"))
	{
		if (ImGui::BeginCombo("##combo", toneItems[m_ToneSettings._Tonetype]))
		{
			for (Uint32 n = 0; n < IM_ARRAYSIZE(toneItems); n++)
			{
				bool is_selected = (m_ToneSettings._Tonetype == n);
				if (ImGui::Selectable(toneItems[n], is_selected))
				{
					m_ToneSettings._Tonetype = n;
					dirty = true;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::SliderFloat("Exposure", &m_ToneSettings._Exposure, 0.1f, 20))
		{
			dirty = true;
		}

		if (ImGui::SliderFloat("Gamma", &m_ToneSettings._Gamma, 0.001f, 4.0f))
		{
			dirty = true;
		}
		ImGui::TreePop();
	}

	if (dirty)
	{
		// Set buffer and draw to screen using our Post Effect.
		m_GraphicsDevice->UpdateBuffer(m_ToneConstBuffer, (Byte*)&m_ToneSettings, sizeof(ToneMappingSettings));
	}
}
