#pragma once
#include "Preset.h"

namespace preset
{
	class DeserializationStrategy
	{
	public:
		virtual PresetPtr operator()(PresetID id, std::string name, json json) const = 0;
	};
}
