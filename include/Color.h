#pragma once
#include "Preset.h"

class Color :
	public Preset
{
public:
	Color(PresetID id, std::string name, RE::NiColor color);

public:
	RE::NiColor GetColor() const;

private:
	RE::NiColor color;
};
