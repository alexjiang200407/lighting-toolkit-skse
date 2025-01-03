#pragma once
#include "Preset.h"
#include "SerializationStrategy.h"

class Color :
	public Preset
{
public:
	class Deserializer :
		public DeserializationStrategy
	{
	public:
		PresetPtr operator()(PresetID id, std::string name, nlohmann::json json) const override;
	};

public:
	Color(PresetID id, std::string name, RE::NiColor color);
	Color(std::string name, RE::NiColor color);

public:
	RE::NiColor GetColor() const;


public:
	static constexpr PresetTID TID = PresetTID::kColor;

private:
	RE::NiColor color;
};
