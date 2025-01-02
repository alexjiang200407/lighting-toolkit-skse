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

private:
	using json = nlohmann::json;

	//template<typename T>
	class DeserializationStrategy
	{
	public:
		virtual Preset* operator()(PresetID id, std::string name, json json) const = 0;
	};

	class ColorDeserialization : public DeserializationStrategy
	{
		Preset* operator()(PresetID id, std::string name, json json) const override;
	};

	class LightingPresetDeserialization : public DeserializationStrategy
	{
		Preset* operator()(PresetID id, std::string name, json json) const override;
	};

private:
	void RegisterPresets(PresetDatabase& presetDB, PresetTID tid, json array) const;

private:
	static const constexpr char* file = "./Data/SKSE/Plugins/SceneCraftPresets.json";
};
