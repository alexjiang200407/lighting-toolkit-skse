#pragma once
#include "ImGui/ImGuiComponent.h"
#include "Preset/PresetDatabase.h"
#include "Preset/PresetSerializationControl.h"
#include "Prop.h"

class SceneCraft :
	ImGui::ImGuiComponent,
	public RE::BSTEventSink<RE::BGSActorCellEvent>
{
public:
	void               Init();
	void               OnDataLoaded();
	void               DoFrame();
	ImGuiStyle         Style();
	static SceneCraft* GetSingleton();

private:
	SceneCraft() = default;

private:
	void                     ToggleMenu();
	bool                     CanOpenWindow();
	bool                     ShouldDrawCursor() override;
	static float*            GetCameraMoveSpeed();
	void                     SuppressDXInput();
	void                     UpdateLookingAround();
	int                      DrawTabBar();
	void                     DrawPropControlWindow(int activePropIndex);
	void                     DrawCameraControlWindow();
	void                     DrawSceneControlWindow();
	RE::BSEventNotifyControl ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>* a_eventSource) override;

private:
	preset::PresetSerializationControl presetSerializationControl;
	preset::PresetDatabase             config;
	bool                               lookingAround              = false;
	bool                               previouslyInFreeCameraMode = false;
	bool                               previouslyFreezeTime       = false;
	bool                               showWindow                 = false;
	std::vector<std::unique_ptr<Prop>> props;
	static SceneCraft                  singleton;
	static constexpr ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
};
