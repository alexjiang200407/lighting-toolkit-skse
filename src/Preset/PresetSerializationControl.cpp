#include "../Preset/PresetSerializationControl.h"
#include "../Color.h"
#include "../LightingPreset.h"
#include <fstream>

using namespace preset;

void PresetSerializationControl::Serialize(const PresetDatabase& a_presetDB)
{
	PresetTID currentTID = kInvalid;
	auto      it         = a_presetDB.cbegin();
	json      jsonBuf;

	while (it != a_presetDB.cend())
	{
		currentTID           = (*it)->GetTID();
		std::string tidAsStr = std::to_string(currentTID);
		jsonBuf[tidAsStr]    = json::array();

		while (it != a_presetDB.cend() && currentTID == (*it)->GetTID())
		{
			logger::info(
				"Serialising preset with tid: {} and id: {}",
				static_cast<int>(currentTID),
				(*it)->GetSIDAsString());
			jsonBuf[tidAsStr].push_back((*it)->Serialize());
			it++;
		}
	}

	std::ofstream f(file);
	f << jsonBuf.dump();
}

void PresetSerializationControl::Deserialize(PresetDatabase& a_presetDB)
{
	std::ifstream f(file);

	if (f.fail())
	{
		logger::warn("File {} not found", file);
		return;
	}

	try
	{
		json data = json::parse(f);
		if (!data.is_object())
			throw std::runtime_error("JSON is not an object!");

		for (const auto& [key, value] : data.items())
		{
			if (!value.is_array())
				throw std::runtime_error("Value must be array");
			auto uintkey = std::stoul(key);
			if (uintkey >= PresetTID::kInvalid)
			{
				throw std::out_of_range("TypeID is invalid");
			}
			PresetTID tid = static_cast<PresetTID>(uintkey);
			RegisterPresets(a_presetDB, tid, value);
		}
	}
	catch (const std::exception& e)
	{
		logger::error("Could not parse presets file");
		logger::error("REASON: {}", e.what());
	}
}

void PresetSerializationControl::RegisterPresets(
	PresetDatabase& a_presetDB,
	PresetTID       tid,
	json            json) const
{
	for (const auto& preset : json)
	{
		if (!preset.contains("sid") || !preset.contains("name"))
			throw std::runtime_error("Must include SID and Name fields");

		const auto presetID =
			PresetID::SIDToID(tid, std::string(preset["sid"]), std::string(preset["name"]));

		if (presetID.IsNull())
			throw std::runtime_error("invalid sid");

		switch (tid)
		{
		case LightingPreset::TID:
			a_presetDB.Insert(LightingPreset::Deserializer{}(presetID, preset));
			break;
		case Color::TID:
			a_presetDB.Insert(Color::Deserializer{}(presetID, preset));
			break;
		default:
			throw std::out_of_range("TypeID is invalid");
			break;
		}
	}
}
