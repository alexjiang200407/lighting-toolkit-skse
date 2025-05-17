#include "ColorPalette.h"
#include "ColorDesigner.h"

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB) :
	ColorPalette(
		new ImGuiColorPresetSelector("Color Presets##ColorPresetSelector", presetDB),
		new ColorDesigner(presetDB))
{}

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB, preset::Color color) :
	ColorPalette(
		color.IsCustom() ?
			ColorPalette(
				new ImGuiColorPresetSelector("Color Presets##ColorPresetSelector", presetDB),
				new ColorDesigner(presetDB, color)) :
			ColorPalette(
				new ImGuiColorPresetSelector("Color Presets##ColorPresetSelector", presetDB, color),
				new ColorDesigner(presetDB)))
{
	if (color.IsCustom())
		SetSelected(1);
}

ColorPalette::ColorPalette(ImGuiColorPresetSelector* presetSelector, ColorDesigner* color) :
	ImGuiColorEditor("Color", { presetSelector, color })
{}

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
