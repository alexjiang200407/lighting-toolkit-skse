#include "LightModelPreset.h"

using namespace preset;

LightModelPreset::LightModelPreset(PresetID id, RE::FormID modelSubID, std::string file, RE::NiPoint3 offset) :
	ModelPreset(id, modelSubID, file), offset(offset)
{
}

RE::NiPoint3 preset::LightModelPreset::GetOffset() const
{
	return offset;
}

json preset::LightModelPreset::Serialize() const
{
	json base      = ModelPreset::Serialize();
	base["offset"] = { offset.x, offset.y, offset.z };
	return base;
}

PresetPtr LightModelPreset::Deserializer::operator()(PresetID id, json json) const
{
	if (!json.contains("formID") || !json.contains("file") || !json.contains("offset"))
		throw std::runtime_error("LightModelPreset must include formID, file and offset field");

	if (!json["offset"].is_array())
		throw std::runtime_error("ModelPreset must include formID, file and offset field");

	RE::FormID   formID = json["formID"];
	std::string  file   = json["file"];
	RE::NiPoint3 offset;

	offset.x = json["offset"][0];
	offset.y = json["offset"][1];
	offset.z = json["offset"][2];

	return std::make_unique<LightModelPreset>(LightModelPreset(id, formID, file, offset));
}
