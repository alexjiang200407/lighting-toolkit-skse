#pragma once
#include "ImGui/ImGuiRenderTarget.h"
#include "LightEditor.h"
#include "Lighting.h"
#include "MenuInputContext.h"
#include "MenuState/MenuState.h"
#include "Preset/PresetDatabase.h"
#include "Preset/PresetSerializationControl.h"
#include "SKSE/SerializableCollection.h"
#include "MCM/Settings.h"

class MenuState;
class MenuOpen;

class Chiaroscuro :
	ImGui::ImGuiRenderTarget,
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
	void                DrawPropControlWindow();
	void                DrawCameraControlWindow();
	void                DrawSceneControlWindow();
	void                PositionLight();

private:
	Chiaroscuro();

private:
	void                     ToggleMenu();
	bool                     CanOpenWindow();
	bool                     ShouldDrawCursor() override;
	static float*            GetCameraMoveSpeed();
	RE::BSEventNotifyControl ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>* a_eventSource) override;
	void                     SerializeItems(SKSE::CoSaveIO io) const override;
	void                     DeserializeItems(SKSE::CoSaveIO io) override;
	void                     Revert(SKSE::CoSaveIO io) override;
	constexpr uint32_t       GetKey() override;

private:
	bool                               firstRender = true;
	std::unique_ptr<MenuState>         menuState{ nullptr };
	Input::MenuInputContext            inputCtx;
	preset::PresetSerializationControl presetSerializationControl;
	preset::PresetDatabase             config;
	ImGuiLightPresetSelector           lightSelector{ "Light Template", &config };
	bool                               previouslyInFreeCameraMode        = false;
	bool                               previouslyFreezeTime              = false;
	bool                               doProcess                         = false;
	static constexpr uint32_t          serializationKey                  = 'CHIA';
	static Chiaroscuro                 singleton;
};
