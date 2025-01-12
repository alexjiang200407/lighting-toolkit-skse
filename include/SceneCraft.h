#pragma once
#include "ImGui/ImGuiComponent.h"
#include "LightEditor.h"
#include "Preset/PresetDatabase.h"
#include "Preset/PresetSerializationControl.h"
#include "Prop.h"

class SceneCraft :
	ImGui::ImGuiComponent,
	public RE::BSTEventSink<RE::BGSActorCellEvent>,
	public ImGui::ImGuiTabBar<Prop>
{
public:
	void               Init();
	void               OnDataLoaded();
	void               DoFrame();
	ImGuiStyle         Style();
	static SceneCraft* GetSingleton();

private:
	SceneCraft();

private:
	void                     ToggleMenu();
	bool                     CanOpenWindow();
	bool                     ShouldDrawCursor() override;
	static float*            GetCameraMoveSpeed();
	void                     SuppressDXInput();
	void                     UpdateLookingAround();
	void                     DrawPropControlWindow();
	void                     DrawCameraControlWindow();
	void                     DrawSceneControlWindow();
	RE::BSEventNotifyControl ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>* a_eventSource) override;

private:
	preset::PresetSerializationControl presetSerializationControl;
	preset::PresetDatabase             config;
	ImGuiLightPresetSelector           lightSelector{ "Light Template", &config };
	bool                               lookingAround                     = false;
	bool                               previouslyInFreeCameraMode        = false;
	bool                               previouslyFreezeTime              = false;
	bool                               previouslyFreezeTimeLookingAround = false;
	bool                               doProcess                         = false;
	static SceneCraft                  singleton;
	static constexpr ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
	bool                               hidden      = false;
};
