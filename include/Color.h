#pragma once
#include "Preset/Preset.h"
#include "Preset/PresetDatabase.h"
#include "Preset/SerializationStrategy.h"

namespace preset
{
	class Color : public Preset, public RE::NiColor
	{
	public:
		class Deserializer : public DeserializationStrategy
		{
		public:
			PresetPtr operator()(PresetID a_id, json json) const override;
		};

	public:
		Color(bool isCustom);
		Color(RE::NiColor color, bool isCustom);
		Color(PresetID a_id, RE::NiColor color, bool isCustom);
		Color(std::string name, RE::NiColor color, bool isCustom);

		operator RE::NiColor() const;

	public:
		RE::NiColor GetColor() const;
		Color       Clone(bool isCustom) const;
		Color       Clone(std::string newName, bool isCustom) const;
		json        Serialize() const override;
		bool        IsCustom() const;

	public:
		static constexpr PresetTID TID = PresetTID::kColor;

	private:
		bool isCustom;
	};
}
