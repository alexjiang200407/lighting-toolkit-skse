#pragma once
#include "Color.h"
#include "ImGui/ImGuiSelector.h"
#include "ImGui/ImGuiValueEditor.h"
#include "Preset/PresetDatabase.h"

class ColorPalette :
	public ImGui::ImGuiValueEditor<ImGui::ImGuiSelector<preset::Color>, preset::Color, 1>
{
public:
	ColorPalette(preset::PresetDatabase* presetDB);
	ColorPalette(preset::PresetDatabase* presetDB, preset::Color color);

private:
	typedef ImGui::ImGuiSelector<preset::Color>                           ImGuiColorSelector;
	typedef ImGui::ImGuiValueEditor<ImGuiColorSelector, preset::Color, 1> ImGuiColorEditor;
};
