#include "Color.h"

Color::Color(PresetID id, std::string name, RE::NiColor color) :
	Preset(PresetTID::kColor, id, name), color(color)
{}


RE::NiColor Color::GetColor() const
{
	return color;
}
