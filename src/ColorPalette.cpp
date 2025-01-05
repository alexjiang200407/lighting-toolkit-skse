#include "ColorPalette.h"

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB) :
	ImGuiColorEditor(
		"Color",
		{ ImGuiColorSelector("Color Presets", presetDB) })
{
}

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB, preset::Color color) :
	ImGuiColorEditor(
		"Color",
		{ ImGuiColorSelector("Color Presets", presetDB, color) })
{
}
