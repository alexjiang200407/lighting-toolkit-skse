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

void ColorPalette::Serialize(SKSE::SerializationInterface* a_intfc) const
{
	a_intfc->WriteRecordData(GetSelection()->GetSID());
	a_intfc->WriteRecordData(static_cast<RE::NiColor>(*GetSelection()));

	size_t strLen = GetSelection()->GetName().size();
	a_intfc->WriteRecordData(strLen);
	a_intfc->WriteRecordData(GetSelection()->GetName().c_str(), strLen);
}

preset::Color ColorPalette::Deserialize(SKSE::SerializationInterface* a_intfc, preset::PresetDatabase* presetDB)
{
	preset::PresetSID       sid;
	RE::NiColor             color;

	a_intfc->ReadRecordData(sid);
	logger::info("{}", uuids::to_string(sid));

	a_intfc->ReadRecordData(color);

	size_t nameSz;
	a_intfc->ReadRecordData(nameSz);
	auto buf = std::unique_ptr<char[]>(new char[nameSz]);
	a_intfc->ReadRecordData(buf.get(), nameSz);

	if (const auto existing = presetDB->Find({ preset::Color::TID, sid, std::string(buf.get()) }); !presetDB->IsEnd(existing))
	{
		return *dynamic_cast<preset::Color*>(existing->get());
	}

	return preset::Color(color, true);
}
