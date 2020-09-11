#include "UI/ImGui_Interface.h"
#include "Application/Game.h"
#include "Application/Application.h"
#include "imgui/icons_font_awesome.h"
#include "Application/GameSettings.h"

#ifdef D3D11
#include "imgui/imgui_impl_dx11.h"
#endif

#ifdef WIN32
#include "imgui/imgui_impl_win32.h"
#endif // WINDOWS

void ImGui_Interface::Initialize(GraphicsDevice* device, std::string fontPath)
{
	m_GraphicsDevice = device;

	// Grab path too the font
	std::string path = fontPath;// "/Font/fontawesome-webfont.ttf";

	// Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF(path.c_str(), 13.0f, &config, icon_ranges);

#ifdef WIN32
	ImGui_ImplWin32_Init(Application::game->m_Window.GetHandle());
#endif

#ifdef D3D11
	ImGui_ImplDX11_Init(device->GetDevice(), device->GetImmediateContext());
#endif // DX11
	ImGui::StyleColorsDark();
}

void ImGui_Interface::BeginDraw()
{
#ifdef D3D11
	ImGui_ImplDX11_NewFrame();
#endif

#ifdef WIN32
	ImGui_ImplWin32_NewFrame();
#endif

	ImGui::NewFrame();
}

void ImGui_Interface::EndDraw()
{
	ImGui::Render();
#ifdef D3D11
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
}

void ImGui_Interface::OnGui()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("ShowStats"))
			{
				m_ShowStats = true;
			}

			if (ImGui::MenuItem("Graphics"))
			{
				m_ShowGraphicsSettings = true;
			}

			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	if (m_ShowStats)
	{
		GameSettings* settings = Application::game->GetGameSettings();
		const GraphicsAdapter* adapter = m_GraphicsDevice->GetAdapter();

		ImGui::Begin("Stats", &m_ShowStats);
		ImGui::Text(adapter->DeviceName().c_str());
		ImGui::Text("VRAM: %llu GB", Uint64(adapter->VRam() * 1e-9));
		ImGui::Text("Resolution:  %d  %d", settings->GetWidth(), settings->GetHeight());
		ImGui::Text("FPS: %d", Application::time->FPS());
		ImGui::Text("MIN FPS: %d", Application::time->MinFPS());
		ImGui::Text("MAX FPS: %d", Application::time->MaxFPS());
		//ImGui::Text("AVR FPS: %d", m_Time.AverageFPS()); // Doesnt quite work as i expected
		ImGui::End();
	}

	if (m_ShowGraphicsSettings)
	{
		GameSettings* settings = Application::game->GetGameSettings();
		ImGui::Begin("Graphics Settings:", &m_ShowGraphicsSettings);

		bool fullscreen = settings->GetIsFullScreen();
		if (ImGui::Checkbox("FullScreen", &fullscreen))
		{
			settings->SetFullScreen(fullscreen);
		}

		bool vsync = settings->GetIsVsync();
		if (ImGui::Checkbox("Vsync: ", &vsync))
		{
			settings->SetVsync(vsync);
		}

		settings->Apply();

		ImGui::Separator();

		bool vrs = settings->IsVRS();
		if (ImGui::Checkbox("Enable VRS: ", &vrs))
		{
			settings->SetVRS(vrs);
		}

		//--Shader Performance DropDown--
		if (ImGui::BeginCombo("ShaderPerformance", FoveatedRenderHelper::s_ShaderPerformanceTable[(int)settings->GetShadingRatePerformance()].c_str()))
		{
			for (Uint32 n = 0; n < IM_ARRAYSIZE(FoveatedRenderHelper::s_ShaderPerformanceTable); n++)
			{
				bool is_selected = ((Uint32)settings->GetShadingRatePerformance() == n);
				if (ImGui::Selectable(FoveatedRenderHelper::s_ShaderPerformanceTable[n].c_str(), is_selected))
				{
					settings->SetShadingRatePerformance((FoveatedShaderPerformance)n);
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (settings->GetShadingRatePerformance() == FoveatedShaderPerformance::Custom)
		{
			//--Inner Shader Rate DropDown--
			if (ImGui::BeginCombo("InnerRate", FoveatedRenderHelper::s_ShaderRateTable[(int)settings->GetInnerRegionRate()].c_str()))
			{
				for (Uint32 n = 0; n < IM_ARRAYSIZE(FoveatedRenderHelper::s_ShaderRateTable); n++)
				{
					bool is_selected = ((Uint32)settings->GetInnerRegionRate() == n);
					if (ImGui::Selectable(FoveatedRenderHelper::s_ShaderRateTable[n].c_str(), is_selected))
					{
						settings->SetInnerRegionRate((FoveatedShadingRate)n);
					}

					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			//--Middle Shader Rate DropDown--
			if (ImGui::BeginCombo("MiddleRate", FoveatedRenderHelper::s_ShaderRateTable[(int)settings->GetMiddleRegionRate()].c_str()))
			{
				for (Uint32 n = 0; n < IM_ARRAYSIZE(FoveatedRenderHelper::s_ShaderRateTable); n++)
				{
					bool is_selected = ((Uint32)settings->GetMiddleRegionRate() == n);
					if (ImGui::Selectable(FoveatedRenderHelper::s_ShaderRateTable[n].c_str(), is_selected))
					{
						settings->SetMiddleRegionRate((FoveatedShadingRate)n);
					}

					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			//--Outer Shader Rate DropDown--
			if (ImGui::BeginCombo("OuterRate", FoveatedRenderHelper::s_ShaderRateTable[(int)settings->GetOuterRegionRate()].c_str()))
			{
				for (Uint32 n = 0; n < IM_ARRAYSIZE(FoveatedRenderHelper::s_ShaderRateTable); n++)
				{
					bool is_selected = ((Uint32)settings->GetOuterRegionRate() == n);
					if (ImGui::Selectable(FoveatedRenderHelper::s_ShaderRateTable[n].c_str(), is_selected))
					{
						settings->SetOuterRegionRate((FoveatedShadingRate)n);
					}

					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::End();
	}
}

void ImGui_Interface::ShutDown()
{
#ifdef D3D11
	ImGui_ImplDX11_Shutdown();
#endif
#ifdef WIN32
	ImGui_ImplWin32_Shutdown();
#endif
}

void ImGui_Interface::ShowDebugBar(bool value)
{
	m_ShowDebugBar = value;
}

void ImGui_Interface::ShowStats(bool value)
{
	m_ShowStats = value;
}

void ImGui_Interface::ShowGraphicsSettings(bool value)
{
	m_ShowGraphicsSettings = value;
}