#include "LightEditor.h"

LightEditor::LightEditor(preset::PresetDatabase* presetDB) :
	ImGuiLightEditor(
		"Lighting",
		{ ImGuiLightPresetSelector("Lighting Presets", presetDB) })
{
}

LightEditor::LightEditor(preset::PresetDatabase* presetDB, preset::LightingPreset lighting) :
	ImGuiLightEditor(
		"Lighting",
		{ ImGuiLightPresetSelector("Lighting Presets", presetDB, lighting) })
{
}
