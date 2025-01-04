#pragma once
#include "Preset.h"
#include "PresetDatabase.h"

namespace preset
{
	class DeserializationStrategy
	{
	public:
		virtual PresetPtr operator()(PresetID id, std::string name, json json) const = 0;
	};
}
