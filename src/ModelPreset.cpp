#include "ModelPreset.h"

using namespace preset;

ModelPreset::ModelPreset(PresetID id, RE::FormID modelSubID, std::string file) :
	Preset(id), modelSubID(modelSubID), file(file)
{
}

RE::FormID preset::ModelPreset::GetFullFormID()
{
	if (!modelFullID)
	{
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		modelFullID       = dataHandler->LookupFormID(modelSubID, file);

		if (!modelFullID)
		{
			logger::error("Model with sub id {:x} on file {} not found", modelSubID, file);
		}
	}

	return modelFullID;
}

RE::TESBoundObject* preset::ModelPreset::ToBoundObj()
{
	auto formID = GetFullFormID();

	if (!formID)
		return nullptr;

	auto* form = RE::TESForm::LookupByID(formID);
	if (!form)
	{
		logger::error("0x{:x} form id not found", formID);
		return nullptr;
	}

	return form->As<RE::TESBoundObject>();
}

json preset::ModelPreset::Serialize() const
{
	json base = Preset::Serialize();
	base["formID"] = modelSubID;
	base["file"]   = file;
	return base;
}

PresetPtr ModelPreset::Deserializer::operator()(PresetID id, json json) const
{
	if (!json.contains("formID") || !json.contains("file"))
		throw std::runtime_error("ModelPreset must include formID and file field");

	RE::FormID  formID = json["formID"];
	std::string fileStr   = json["file"];
	return std::make_unique<ModelPreset>(ModelPreset(id, formID, fileStr));
}
