#include "ColorPalette.h"
#include "ColorDesigner.h"

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB) :
	ImGuiColorEditor(
		"Color",
		{ new ImGuiColorPresetSelector("Color Presets##ColorPresetSelector", presetDB), new ColorDesigner(presetDB) })
{
}

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB, preset::Color color) :
	ImGuiColorEditor(
		"Color",
		{ new ImGuiColorPresetSelector("Color Presets##ColorPresetSelector", presetDB, color), new ColorDesigner(presetDB) })
{
}
