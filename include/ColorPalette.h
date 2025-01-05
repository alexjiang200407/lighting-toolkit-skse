#pragma once
#include "Color.h"
#include "ImGui/ImGuiSelector.h"
#include "ImGui/ImGuiValueEditor.h"
#include "Preset/PresetDatabase.h"

typedef ImGui::ImGuiSelector<preset::Color>                         ImGuiColorSelector;
typedef ImGui::ImGuiValueEditor<ImGuiColorSelector, RE::NiColor, 1> ImGuiColorEditor;

class ColorPalette :
	public ImGuiColorEditor
{
public:
	ColorPalette(preset::PresetDatabase* presetDB);
	ColorPalette(preset::PresetDatabase* presetDB, preset::Color color);
};
