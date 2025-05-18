#include "ColorPalette.h"

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB) :
	presetSelector("Color Preset", presetDB), presetDB(presetDB), mode(ColorSelectionMode::kPreset)
{}

ColorPalette::ColorPalette(preset::PresetDatabase* presetDB, preset::Color color) :
	presetSelector("Color Preset", presetDB, color), presetDB(presetDB)
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

bool DrawColorEditor(
	std::string&            nameInput,
	preset::Color&          color,
	preset::PresetDatabase* presetDB);

bool ColorPalette::DrawEditor()
{
	bool changedMode = false;
	if (ImGui::BeginTabBar("Choose Color"))
	{
		auto presetTabFlags = (firstRender && mode == ColorSelectionMode::kPreset) ?
		                          ImGuiTabItemFlags_SetSelected :
		                          ImGuiTabItemFlags_None;
		auto customTabFlags = (firstRender && mode == ColorSelectionMode::kCustom) ?
		                          ImGuiTabItemFlags_SetSelected :
		                          ImGuiTabItemFlags_None;

		if (ImGui::BeginTabItem("Presets", nullptr, presetTabFlags))
		{
			if (customTabFlags != ImGuiTabItemFlags_SetSelected)
			{
				mode        = ColorSelectionMode::kPreset;
				changedMode = true;
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Custom", nullptr, customTabFlags))
		{
			if (presetTabFlags != ImGuiTabItemFlags_SetSelected)
			{
				mode        = ColorSelectionMode::kCustom;
				changedMode = true;
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	firstRender = false;

	switch (mode)
	{
	case ColorSelectionMode::kPreset:
		return presetSelector.DrawValueEditor() || changedMode;
	case ColorSelectionMode::kCustom:
		return DrawColorEditor(nameInput, editorColor, presetDB) || changedMode;
	default:
		logger::error("Invalid Color Selection Mode");
		return false;
	}
}

bool DrawColorEditor(std::string& nameInput, preset::Color& color, preset::PresetDatabase* presetDB)
{
	float rgb[3]  = { color.red, color.green, color.blue };
	bool  changed = false;
	ImGui::InputText("Color Name", &nameInput);

	if (ImGui::ColorEdit3("Color Picker", rgb))
	{
		color.red   = rgb[0];
		color.green = rgb[1];
		color.blue  = rgb[2];
		changed     = true;
	}

	if (ImGui::Button("Save Color"))
	{
		presetDB->Insert(std::make_unique<preset::Color>(color.Clone(nameInput, false)));
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
