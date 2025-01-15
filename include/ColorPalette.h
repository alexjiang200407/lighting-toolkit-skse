#pragma once
#include "Color.h"
#include "ImGui/ImGuiPresetSelector.h"
#include "ImGui/ImGuiValueEditor.h"
#include "Preset/PresetDatabase.h"

typedef ImGui::ImGuiSelector<preset::Color>                           ImGuiColorSelector;
typedef ImGui::ImGuiPresetSelector<preset::Color>                     ImGuiColorPresetSelector;
typedef ImGui::ImGuiValueEditor<ImGuiColorSelector, preset::Color, 2> ImGuiColorEditor;

class ColorPalette :
	public ImGuiColorEditor
{
public:
	ColorPalette(preset::PresetDatabase* presetDB);
	ColorPalette(preset::PresetDatabase* presetDB, preset::Color color);

public:
	void                 Serialize(SKSE::SerializationInterface* a_intfc) const;
	static preset::Color Deserialize(SKSE::SerializationInterface* a_intfc, preset::PresetDatabase* presetDB);
};
