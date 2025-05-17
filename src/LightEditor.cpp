#include "LightEditor.h"
#include "LightDesigner.h"

LightEditor::LightEditor(preset::PresetDatabase* presetDB) :
	ImGuiLightEditor(
		"Lighting",
		{ new ImGuiLightPresetSelector("Lighting Presets", presetDB), new LightDesigner(presetDB) })
{}

LightEditor::LightEditor(preset::PresetDatabase* presetDB, preset::LightingPreset lighting) :
	ImGuiLightEditor(
		"Lighting",
		{ new ImGuiLightPresetSelector("Lighting Presets", presetDB, lighting),
          new LightDesigner(presetDB) })
{}
