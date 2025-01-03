#include "Color.h"
#include "LightingPreset.h"
#include "Preset.h"
#include "PresetDatabase.h"
#include <fstream>
#include <nlohmann/json.hpp>

class PresetSerializationControl
{
public:
	void Serialize(const PresetDatabase& presetDB);
	void Deserialize(PresetDatabase& presetDB);

public:
	class DeserializationStrategy
	{
	public:
		virtual PresetPtr operator()(PresetID id, std::string name, nlohmann::json json) const = 0;
	};

	class ColorDeserialization : public DeserializationStrategy
	{
		PresetPtr operator()(PresetID id, std::string name, nlohmann::json json) const override;
	};

	class LightingPresetDeserialization : public DeserializationStrategy
	{
		PresetPtr operator()(PresetID id, std::string name, nlohmann::json json) const override;
	};

private:
	using json = nlohmann::json;
	void RegisterPresets(PresetDatabase& presetDB, PresetTID tid, json array) const;

private:
	static const constexpr char* file = "./Data/SKSE/Plugins/SceneCraftPresets.json";
};
