#pragma once
#include "Color.h"
#include "ImGui/ImGuiPresetSelector.h"
#include "Preset/PresetDatabase.h"
#include "SKSE/CoSaveIO.h"

class ColorPalette
{
public:
	ColorPalette(preset::PresetDatabase* presetDB);
	ColorPalette(preset::PresetDatabase* presetDB, preset::Color color);

private:
	enum class ColorSelectionMode
	{
		kPreset = 0,
		kCustom = 1,
	};

public:
	void                 Serialize(SKSE::CoSaveIO io) const;
	static preset::Color Deserialize(SKSE::CoSaveIO io, preset::PresetDatabase* presetDB);
	bool                 DrawEditor();
	const preset::Color* GetSelection() const;

private:
	bool                                      firstRender = true;
	preset::PresetDatabase*                   presetDB;
	ColorSelectionMode                        mode;
	preset::Color                             editorColor{ true };
	ImGui::ImGuiPresetSelector<preset::Color> presetSelector;
};
