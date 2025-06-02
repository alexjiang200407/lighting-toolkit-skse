#include "ColorPalette.h"

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB) :
	presetSelector("Color Preset", presetDB), presetDB(presetDB), mode(ColorSelectionMode::kPreset)
{}

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB, preset::Color color) :
	presetSelector(
		"Color Preset",
		presetDB,
		color.IsCustom() ? std::nullopt : std::optional(color)),
	presetDB(presetDB), editorColor(color)
{
	mode = color.IsCustom() ? ColorSelectionMode::kCustom : ColorSelectionMode::kPreset;
}

void ColorPalette::Serialize(SKSE::CoSaveIO io) const
{
	io.Write(GetSelection()->GetSID());
	io.Write(static_cast<RE::NiColor>(*GetSelection()));

	uint32_t strLen = (uint32_t)GetSelection()->GetName().size() + 1;
	io.Write(strLen);
	io.Write(GetSelection()->GetName().c_str(), strLen);
}

preset::Color ColorPalette::Deserialize(SKSE::CoSaveIO io, preset::PresetDatabase* presetDB)
{
	preset::PresetSID sid;
	RE::NiColor       color;

	io.Read(sid);
	logger::info("{}", uuids::to_string(sid));

	io.Read(color);

	uint32_t nameSz;
	io.Read(nameSz);
	auto buf = std::unique_ptr<char[]>(new char[nameSz]);
	io.Read(buf.get(), nameSz);

	if (const auto existing = presetDB->Find({ preset::Color::TID, sid, std::string(buf.get()) });
	    !presetDB->IsEnd(existing))
	{
		return *dynamic_cast<preset::Color*>(existing->get());
	}

	return preset::Color(buf.get(), color, true);
}

bool DrawColorEditor(preset::Color& color);

bool ColorPalette::DrawEditor()
{
	bool               changedMode = false;
	static const char* tabs[]      = { "Presets", "Custom" };
	static bool        enabled[]   = { true, true };
	ImGui::NavBar(
		"Choose Color",
		tabs,
		enabled,
		sizeof(tabs) / sizeof(const char*),
		reinterpret_cast<size_t*>(&mode),
		&changedMode,
		firstRender ? reinterpret_cast<size_t*>(&mode) : nullptr);

	firstRender = false;

	switch (mode)
	{
	case ColorSelectionMode::kPreset:
		return presetSelector.DrawValueEditor() || changedMode;
	case ColorSelectionMode::kCustom:
		return DrawColorEditor(editorColor) || changedMode;
	default:
		logger::error("Invalid Color Selection Mode");
		return false;
	}
}

bool DrawColorEditor(preset::Color& color)
{
	float rgb[3]  = { color.red, color.green, color.blue };
	bool  changed = false;

	if (ImGui::ColorEdit3("Color Picker", rgb))
	{
		color.red   = rgb[0];
		color.green = rgb[1];
		color.blue  = rgb[2];
		changed     = true;
	}

	return changed;
}

const preset::Color* ColorPalette::GetSelection() const
{
	switch (mode)
	{
	case ColorSelectionMode::kPreset:
		return presetSelector.GetSelection();
	case ColorSelectionMode::kCustom:
		return &editorColor;
	default:
		logger::error("Invalid Color Selection");
		return nullptr;
	}
}

void ColorPalette::SetCustomColor(RE::NiColor color)
{
	editorColor.red   = color.red;
	editorColor.green = color.green;
	editorColor.blue  = color.blue;
	mode              = ColorSelectionMode::kCustom;
}
