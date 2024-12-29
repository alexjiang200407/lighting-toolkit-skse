#pragma once
#include "ImGui/ImGuiComponent.h"

class SceneCraft :
	ImGui::ImGuiComponent
{
public:
	void               Init();
	void               DoFrame();
	ImGuiStyle         Style();
	static SceneCraft* GetSingleton();

private:
	SceneCraft() = default;

private:
	void ToggleMenu();
	bool ShouldDrawCursor() override;

private:
	bool              showWindow = false;
	static SceneCraft singleton;
};
