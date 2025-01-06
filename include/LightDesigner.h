#pragma once
#include "LightingPreset.h"
#include "ImGui/ImGuiPresetSelector.h"
#include "Preset/PresetDatabase.h"

class LightDesigner :
	public ImGui::ImGuiSelector<preset::LightingPreset>
{
public:
	LightDesigner(preset::PresetDatabase* presetDB);

public:
	bool DrawValueEditor() override;

private:
	preset::PresetDatabase* presetDB;
};
