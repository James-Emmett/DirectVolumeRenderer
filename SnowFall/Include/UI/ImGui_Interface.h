//Note:
/*
	Basic managment of ImGui,
	Maybe allow extensions? other than just ShowStates, ShowSettings?
*/

#pragma once
#include "imgui/imgui.h"
#include "System/Types.h"
#include <string>

class GraphicsDevice;
class ImGui_Interface
{
private:
	GraphicsDevice* m_GraphicsDevice = nullptr;
	bool m_ShowDebugBar = true;
	bool m_ShowStats = false;
	bool m_ShowGraphicsSettings = false;

public:
	void Initialize(GraphicsDevice* device, std::string fontPath);
	void BeginDraw();
	void EndDraw();
	void OnGui();
	void ShutDown();

	void ShowDebugBar(bool value);
	void ShowStats(bool value);
	void ShowGraphicsSettings(bool value);
};