#pragma once
#include "ImGui/ImGuiPresetSelector.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"

class LightDesigner : public ImGui::ImGuiSelector<preset::LightingPreset>
{
public:
	LightDesigner(preset::PresetDatabase* presetDB);

public:
	bool DrawValueEditor() override;

private:
	preset::PresetDatabase* presetDB;
};
