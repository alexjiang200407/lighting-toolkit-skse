#pragma once
#include "Environment/Environment.h"
#include "ImGui/ImGuiRenderTarget.h"
#include "MCM/Settings.h"
#include "MenuInputContext.h"
#include "MenuState/MenuState.h"
#include "Preset/PresetDatabase.h"
#include "Preset/PresetSerializationControl.h"
#include "SKSE/SerializableCollection.h"
#include "SceneLighting/SceneLighting.h"

class MenuState;
class MenuOpen;

class LightingToolkit : ImGui::ImGuiRenderTarget, public SKSE::SerializableCollection
{
private:
	enum class Tool : size_t
	{
		kSceneLight = 0,
		kCharacterLight,
		kCamera,
		kEnvironment
	};

public:
	void                    Init();
	void                    OnDataLoaded();
	void                    OnSavePostLoaded();
	void                    DoFrame();
	ImGuiStyle              Style();
	static LightingToolkit* GetSingleton();
	void                    DrawMenu();
	void                    Position();

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
	void               DrawCameraControlWindow();
	void               HandleToolMenuOpen();

private:
	SceneLighting                      sceneLighting;
	Environment                        environment;
	std::unique_ptr<MenuState>         menuState{ nullptr };
	Input::MenuInputContext            inputCtx;
	preset::PresetSerializationControl presetSerializationControl;
	preset::PresetDatabase             config;
	bool                               previouslyInFreeCameraMode = false;
	bool                               previouslyFreezeTime       = false;
	bool                               doProcess                  = false;
	static constexpr uint32_t          serializationKey           = 'CHIA';
	static LightingToolkit             singleton;
	Tool                               currentTool;
};
