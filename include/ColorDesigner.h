#pragma once
#include "Color.h"
#include "ImGui/ImGuiPresetSelector.h"
#include "Preset/PresetDatabase.h"

class ColorDesigner :
	public ImGui::ImGuiSelector<preset::Color>
{
public:
	ColorDesigner(preset::PresetDatabase* presetDB);
	ColorDesigner(preset::PresetDatabase* presetDB, preset::Color color);

public:
	bool DrawValueEditor() override;

private:
	preset::PresetDatabase* presetDB;
	std::string             nameInput = "Unknown Color";
};
