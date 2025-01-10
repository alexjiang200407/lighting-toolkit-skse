#pragma once
#include "Color.h"
#include "ImGui/ImGuiPresetSelector.h"
#include "Preset/PresetDatabase.h"

class ColorDesigner :
	public ImGui::ImGuiSelector<preset::Color>
{
public:
	ColorDesigner(preset::PresetDatabase* presetDB);

public:
	bool DrawValueEditor() override;

private:
	preset::PresetDatabase* presetDB;
};
