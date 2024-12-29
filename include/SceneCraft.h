#pragma once
#include "ImGui/ImGuiComponent.h"
#include "ImGui/ImGuiRenderer.h"

class SceneCraft :
	ImGui::ImGuiComponent
{
public:
	static void Init();
	void        DoFrame();

private:
	SceneCraft() = default;
	
private:
	void ToggleMenu();

private:
	ImGui::ImGuiRenderer renderer;
	bool                 showWindow = false;
	static SceneCraft    singleton;
};
