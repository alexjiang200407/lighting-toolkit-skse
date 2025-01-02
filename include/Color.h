#pragma once
#include "Preset.h"

class Color :
	public Preset
{
public:
	Color(PresetID id, std::string name, RE::NiColor color);

public:
	RE::NiColor GetColor() const;


public:
	static constexpr PresetTID TID = PresetTID::kColor;

private:
	RE::NiColor color;
};
