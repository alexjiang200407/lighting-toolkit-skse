#pragma once
#include "Preset/Preset.h"
#include "Preset/PresetDatabase.h"
#include "Preset/SerializationStrategy.h"

namespace preset
{
	class Color :
		public Preset,
		public RE::NiColor
	{
	public:
		class Deserializer :
			public DeserializationStrategy
		{
		public:
			PresetPtr operator()(PresetID a_id, std::string a_name, json json) const override;
		};

	public:
		Color(PresetID a_id, std::string a_name, RE::NiColor color);
		Color(std::string name, RE::NiColor color);

		operator RE::NiColor() const;

	public:
		RE::NiColor GetColor() const;
		json        Serialize() const override;

	public:
		static constexpr PresetTID TID = PresetTID::kColor;
	};
}
