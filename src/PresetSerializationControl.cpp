#include "PresetSerializationControl.h"
#include <fstream>

using json = nlohmann::json;

void PresetSerializationControl::Serialize(const PresetDatabase& presetDB)
{
}

void PresetSerializationControl::Deserialize(PresetDatabase& presetDB)
{
	std::ifstream f(file);

	if (f.fail())
	{
		logger::warn("File {} not found", file);
		return;
	}
	std::this_thread::sleep_for(std::chrono::seconds(3));
	try
	{
		json data = json::parse(f);
		if (!data.is_object())
			throw std::runtime_error("JSON is not an object!");

		for (const auto& [key, value] : data.items())
		{
			auto uintkey = std::stoul(key);
			if (uintkey >= PresetTID::kInvalid)
			{
				throw std::out_of_range("TypeID is invalid");
			}
			PresetTID tid = static_cast<PresetTID>(uintkey);
			RegisterPresets(presetDB, tid, value);
		}
	}
	catch (const std::exception& e)
	{
		logger::error("Could not parse presets file");
		logger::error("REASON: {}", e.what());
	}
}

void PresetSerializationControl::RegisterPresets(PresetDatabase& presetDB, PresetTID tid, json json) const
{
	std::unique_ptr<DeserializationStrategy> strat;

	switch (tid)
	{
	case LightingPreset::TID:
		strat = std::make_unique<LightingPresetDeserialization>();
		break;
	case Color::TID:
		strat = std::make_unique<ColorDeserialization>();
		break;
	default:
		throw std::out_of_range("TypeID is invalid");
		break;
	}

	for (const auto& preset : json)
	{
		const auto presetID = PresetID::SIDToID(tid, std::string(preset["sid"]));

		if (presetID.IsNull())
			throw std::runtime_error("invalid sid");

		presetDB.Insert((*strat)(presetID, preset["name"], preset));
	}
}

PresetPtr PresetSerializationControl::ColorDeserialization::operator()(PresetID id, std::string name, json json) const
{
	uint32_t color = json["colorcode"];
	return std::make_unique<Color>(Color(id, name, RE::NiColor(color)));
}

PresetPtr PresetSerializationControl::LightingPresetDeserialization::operator()(PresetID id, std::string name, json json) const
{
	LightingPreset::LightFlags flags;
	for (const auto& value : json["flags"])
	{
		int idx = value;
		flags.set(static_cast<LightingPreset::Flags>(idx));
	}
	return std::make_unique<LightingPreset>(LightingPreset(id, name, flags));
}
