#pragma once
#include "ImGui/ImGuiComponent.h"
#include "Prop.h"

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
	void          ToggleMenu();
	bool          ShouldDrawCursor() override;
	static float* GetCameraMoveSpeed();
	void          SuppressDXInput();
	void          UpdateLookingAround();

private:
	bool                              lookingAround              = false;
	bool                              previouslyInFreeCameraMode = false;
	bool                              previouslyFreezeTime       = false;
	bool                              showWindow                 = false;
	std::vector<Prop>                 props;
	int                               activePropIndex = -1;
	static SceneCraft                 singleton;
	static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
};
