#pragma once
#include "Color.h"
#include "ImGui/ImGuiNavBar.h"
#include "ImGui/ImGuiSelector.h"
#include "ImGui/ImGuiValueEditor.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"
#include "Prop.h"

class Lighting :
	public Prop
{
private:
	//typedef ImGui::ImGuiSelector<preset::LightingPreset> LightPresetSelector;
	//typedef ImGui::ImGuiSelector<preset::Color>          ColorSelector;

	class ColorPalette :
		public ImGui::ImGuiValueEditor<ImGui::ImGuiSelector<preset::Color>, preset::Color, 1>
	{
	public:
		ColorPalette(preset::PresetDatabase* presetDB) :
			ImGui::ImGuiValueEditor<ImGui::ImGuiSelector<preset::Color>, preset::Color, 1>(
				"Color",
				{ ImGui::ImGuiSelector<preset::Color>("Color Presets", presetDB) }
			)
		{
		}

		ColorPalette(preset::PresetDatabase* presetDB, preset::Color color) :
			ImGui::ImGuiValueEditor<ImGui::ImGuiSelector<preset::Color>, preset::Color, 1>(
				"Color",
				{ ImGui::ImGuiSelector<preset::Color>("Color Presets", presetDB, color) })
		{
		}
	};

	class LightPresetSelector :
		public ImGui::ImGuiValueEditor<ImGui::ImGuiSelector<preset::LightingPreset>, preset::LightingPreset, 1>
	{
	public:
		LightPresetSelector(preset::PresetDatabase* presetDB) :
			ImGui::ImGuiValueEditor<ImGui::ImGuiSelector<preset::LightingPreset>, preset::LightingPreset, 1>(
				"Lighting",
				{ ImGui::ImGuiSelector<preset::LightingPreset>("Lighting Presets", presetDB) })
		{
		}

		LightPresetSelector(preset::PresetDatabase* presetDB, preset::LightingPreset lighting) :
			ImGui::ImGuiValueEditor<ImGui::ImGuiSelector<preset::LightingPreset>, preset::LightingPreset, 1>(
				"Lighting",
				{ ImGui::ImGuiSelector<preset::LightingPreset>("Lighting Presets", presetDB, lighting) })
		{
		}
	};

public:
	Lighting(RE::TESObjectREFRPtr ref, preset::PresetDatabase* presetDB);
	Lighting(RE::TESObjectREFRPtr ref, preset::Color color, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset);

public:
	void DrawControlPanel() override;
	void UpdateLightColor();
	void UpdateLightTemplate();
	void Remove() override;
	void MoveToCameraLookingAt(float distanceFromCamera) override;
	void OnEnterCell() override;

private:
	void FindOrCreateLight();

private:
	RE::NiPointer<RE::BSLight>      bsLight = nullptr;
	RE::NiPointer<RE::NiPointLight> niLight = nullptr;
	ColorPalette                    colorPalette;
	LightPresetSelector             lightingPreset;
};
