#pragma once
#include "ImGui/ImGuiSelector.h"
#include "ImGui/ImGuiValueEditor.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"

typedef ImGui::ImGuiSelector<preset::LightingPreset>                                 ImGuiLightPresetSelector;
typedef ImGui::ImGuiValueEditor<ImGuiLightPresetSelector, preset::LightingPreset, 1> ImGuiLightEditor;

class LightEditor :
	public ImGuiLightEditor
{
public:
	LightEditor(preset::PresetDatabase* presetDB);
	LightEditor(preset::PresetDatabase* presetDB, preset::LightingPreset lighting);
};
