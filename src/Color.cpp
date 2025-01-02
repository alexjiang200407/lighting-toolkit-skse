#include "Color.h"

Color::Color(PresetID id, std::string name, RE::NiColor color) :
	Preset(id, name), color(color)
{
}

Color::Color(std::string name, RE::NiColor color) :
	Preset(PresetID::GenID<Color>(), name), color(color)
{}


RE::NiColor Color::GetColor() const
{
	return color;
}
