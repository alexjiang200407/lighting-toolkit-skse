#pragma once
#include "Color.h"
#include "ImGui/ImGuiNavBar.h"
#include "ImGui/ImGuiSelector.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"
#include "Prop.h"

class Lighting :
	public Prop
//public ImGui::ImGuiNavBar
{
private:
	typedef ImGui::ImGuiSelector<preset::LightingPreset> LightPresetSelector;
	typedef ImGui::ImGuiSelector<preset::Color>          ColorSelector;

public:
	Lighting(RE::TESObjectREFRPtr ref);
	Lighting(RE::TESObjectREFRPtr ref, preset::Color color, preset::LightingPreset lightPreset);

public:
	void DrawControlPanel(preset::PresetDatabase& config) override;
	void UpdateLightColor();
	void UpdateLightTemplate();
	void Remove() override;
	void MoveToCameraLookingAt(float distanceFromCamera) override;
	void OnEnterCell() override;

private:
	void FindOrCreateLight();

private:
	ImGui::ImGuiNavBar<2>           navBar{ "##lightingnavbar", { "a", "b" } };
	RE::NiPointer<RE::BSLight>      bsLight = nullptr;
	RE::NiPointer<RE::NiPointLight> niLight = nullptr;
	ColorSelector                   colorPalette;
	LightPresetSelector             lightingPreset;
};
