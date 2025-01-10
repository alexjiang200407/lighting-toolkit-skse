#pragma once
#include "ImGui/ImGuiPresetSelector.h"
#include "ImGui/ImGuiValueEditor.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"

typedef ImGui::ImGuiSelector<preset::LightingPreset>                           ImGuiLightSelector;
typedef ImGui::ImGuiPresetSelector<preset::LightingPreset>                     ImGuiLightPresetSelector;
typedef ImGui::ImGuiValueEditor<ImGuiLightSelector, preset::LightingPreset, 2> ImGuiLightEditor;

class LightEditor :
	public ImGuiLightEditor
{
public:
	LightEditor(preset::PresetDatabase* presetDB);
	LightEditor(preset::PresetDatabase* presetDB, preset::LightingPreset lighting);
};
