#include "Preset.h"
#include "PresetDatabase.h"
#include <fstream>

namespace preset
{
	class PresetSerializationControl
	{
	public:
		void Serialize(const PresetDatabase& a_presetDB);
		void Deserialize(PresetDatabase& a_presetDB);

	private:
		void RegisterPresets(PresetDatabase& a_presetDB, PresetTID tid, json array) const;

	private:
		static const constexpr char* file = "./Data/SKSE/Plugins/SceneCraftPresets.json";
	};
}
