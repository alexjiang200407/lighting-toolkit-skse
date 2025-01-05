#include "ColorPalette.h"
#include "ColorDesigner.h"

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB) :
	ImGuiColorEditor(
		"Color",
		{ new ImGuiColorPresetSelector("Color Presets", presetDB), new ColorDesigner(presetDB) })
{
}

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB, preset::Color color) :
	ImGuiColorEditor(
		"Color",
		{ new ImGuiColorPresetSelector("Color Presets", presetDB, color), new ColorDesigner(presetDB) })
{
}
