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

void ColorPalette::Serialize(SKSE::CoSaveIO io) const
{
	io.Write(GetSelection()->GetSID());
	io.Write(static_cast<RE::NiColor>(*GetSelection()));

	size_t strLen = GetSelection()->GetName().size();
	io.Write(strLen);
	io.Write(GetSelection()->GetName().c_str(), strLen);
}

preset::Color ColorPalette::Deserialize(SKSE::CoSaveIO io, preset::PresetDatabase* presetDB)
{
	preset::PresetSID       sid;
	RE::NiColor             color;

	io.Read(sid);
	logger::info("{}", uuids::to_string(sid));

	io.Read(color);

	size_t nameSz;
	io.Read(nameSz);
	auto buf = std::unique_ptr<char[]>(new char[nameSz]);
	io.Read(buf.get(), nameSz);

	if (const auto existing = presetDB->Find({ preset::Color::TID, sid, std::string(buf.get()) }); !presetDB->IsEnd(existing))
	{
		return *dynamic_cast<preset::Color*>(existing->get());
	}

	return preset::Color(color, true);
}
