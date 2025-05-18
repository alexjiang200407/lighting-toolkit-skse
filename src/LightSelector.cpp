#include "LightSelector.h"

LightSelector::LightSelector(preset::PresetDatabase* presets) :
	ImGui::ImGuiPresetSelector<preset::LightingPreset>("Light Selector", presets)
{}
