#pragma once
#include "ImGui/ImGuiRenderTarget.h"
#include "LightSelector.h"
#include "Lighting.h"
#include "MCM/Settings.h"
#include "MenuInputContext.h"
#include "MenuState/MenuState.h"
#include "Preset/PresetDatabase.h"
#include "Preset/PresetSerializationControl.h"
#include "SKSE/SerializableCollection.h"

class MenuState;
class MenuOpen;

class LightingToolkit :
	ImGui::ImGuiRenderTarget,
	public RE::BSTEventSink<RE::BGSActorCellEvent>,
	public SKSE::SerializableCollection
{
public:
	void                    Init();
	void                    OnDataLoaded();
	void                    OnSavePostLoaded();
	void                    DoFrame();
	ImGuiStyle              Style();
	static LightingToolkit* GetSingleton();
	void                    DrawPropControlWindow();
	void                    DrawCameraControlWindow();
	void                    DrawSceneControlWindow();
	void                    PositionLight();
	void                    DrawTabBar();

private:
	LightingToolkit();

private:
	void               ToggleMenu();
	bool               CanOpenWindow();
	bool               ShouldDrawCursor() override;
	static float*      GetCameraMoveSpeed();
	void               SerializeItems(SKSE::CoSaveIO io) const override;
	void               DeserializeItems(SKSE::CoSaveIO io) override;
	void               Revert(SKSE::CoSaveIO io) override;
	constexpr uint32_t GetKey() override;

	RE::BSEventNotifyControl ProcessEvent(
		const RE::BGSActorCellEvent*               a_event,
		RE::BSTEventSource<RE::BGSActorCellEvent>* a_eventSource) override;

	Lighting* GetCurrentLight();

private:
	bool                               firstRender = true;
	std::unique_ptr<MenuState>         menuState{ nullptr };
	Input::MenuInputContext            inputCtx;
	preset::PresetSerializationControl presetSerializationControl;
	preset::PresetDatabase             config;
	LightSelector                      lightSelector{ &config };
	bool                               previouslyInFreeCameraMode = false;
	bool                               previouslyFreezeTime       = false;
	bool                               doProcess                  = false;
	static constexpr uint32_t          serializationKey           = 'CHIA';
	static LightingToolkit             singleton;
	std::vector<Lighting>              lights;
	std::optional<size_t>              currentTab;
	std::optional<RE::FormID>          currentFormID;
};
