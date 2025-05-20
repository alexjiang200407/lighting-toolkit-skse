#pragma once
#include "ImGui/ImGuiPresetSelector.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"

class LightSelector : public ImGui::ImGuiPresetSelector<preset::LightingPreset>
{
public:
	LightSelector(preset::PresetDatabase* presets);
};
