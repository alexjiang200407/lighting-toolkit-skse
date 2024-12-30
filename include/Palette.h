#pragma once
#include "ImGui/ImGuiSelector.h"

class Palette :
	public ImGui::ImGuiSelector<RE::NiColor>
{
public:
	Palette(int colorIdx);

public:
	typedef std::pair<std::string, RE::NiColor> Color;

	const char* SelectionName(int idx) const override;
	int         GetSelectionCount() const override;
	static void LoadPaletteFile();
	const char* GetSelectionID() override;

private:
	static inline std::vector<Color>  colors;
	static constexpr std::string_view filePath = "./Data/SKSE/Plugins/SceneCraft.json"sv;
};
