#include "Palette.h"

Palette::Palette(int colorIdx) :
	ImGui::ImGuiSelector<RE::NiColor>(colorIdx)
{
}

const char* Palette::SelectionName(int idx) const
{
	return colors[idx].first.c_str();
}

int Palette::GetSelectionCount() const
{
	return (int)colors.size();
}

void Palette::LoadPaletteFile()
{
	// TODO Load JSON File
	colors.push_back({ "Red", { 1, 0, 0 } });
	colors.push_back({ "Green", { 0, 1, 0 } });
	colors.push_back({ "Blue", { 0, 0, 1 } });
}

const char* Palette::GetSelectionID()
{
	return "Color";
}

const RE::NiColor& Palette::GetCurrentSelection() const
{
	return colors[selectionIdx].second;
}
