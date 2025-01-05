#pragma once
#include "ImGui/ImGuiSelector.h"
#include "ImGui/ImGuiValueEditor.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"

class LightEditor :
	public ImGui::ImGuiValueEditor<ImGui::ImGuiSelector<preset::LightingPreset>, preset::LightingPreset, 1>
{
public:
	LightEditor(preset::PresetDatabase* presetDB);
	LightEditor(preset::PresetDatabase* presetDB, preset::LightingPreset lighting);

private:
	typedef ImGui::ImGuiSelector<preset::LightingPreset>                                 ImGuiLightPresetSelector;
	typedef ImGui::ImGuiValueEditor<ImGuiLightPresetSelector, preset::LightingPreset, 1> ImGuiLightEditor;
};
