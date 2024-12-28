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
	ImGui::ImGuiRenderer renderer;
	static SceneCraft    singleton;
};
