#pragma once
#include "ImGui/ImGuiComponent.h"
#include "LightEditor.h"
#include "Lighting.h"
#include "Preset/PresetDatabase.h"
#include "Preset/PresetSerializationControl.h"
#include "SKSE/SerializableCollection.h"

class Chiaroscuro :
	ImGui::ImGuiComponent,
	public RE::BSTEventSink<RE::BGSActorCellEvent>,
	public ImGui::ImGuiTabBar<Lighting>,
	public SKSE::SerializableCollection
{
public:
	void                Init();
	void                OnDataLoaded();
	void                OnSavePostLoaded();
	void                DoFrame();
	ImGuiStyle          Style();
	static Chiaroscuro* GetSingleton();

private:
	Chiaroscuro();

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
	void                     SerializeItems(SKSE::CoSaveIO io) const override;
	void                     DeserializeItems(SKSE::CoSaveIO io) override;
	void                     Revert(SKSE::CoSaveIO io) override;
	constexpr uint32_t       GetKey() override;

private:
	preset::PresetSerializationControl presetSerializationControl;
	preset::PresetDatabase             config;
	ImGuiLightPresetSelector           lightSelector{ "Light Template", &config };
	bool                               lookingAround                     = false;
	bool                               previouslyInFreeCameraMode        = false;
	bool                               previouslyFreezeTime              = false;
	bool                               previouslyFreezeTimeLookingAround = false;
	bool                               doProcess                         = false;
	static Chiaroscuro                 singleton;
	static constexpr ImGuiWindowFlags  windowFlags      = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
	bool                               hidden           = false;
	static constexpr uint32_t          serializationKey = 'CHIA';
};
