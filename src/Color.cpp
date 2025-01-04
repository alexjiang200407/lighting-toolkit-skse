#include "Color.h"

using namespace preset;

Color::Color(PresetID id, std::string name, RE::NiColor color) :
	Preset(id, name), color(color)
{
}

Color::Color(std::string name, RE::NiColor color) :
	Preset(PresetID::GenID<Color>(), name), color(color)
{}

preset::Color::operator RE::NiColor() const
{
	return color;
}


RE::NiColor Color::GetColor() const
{
	return color;
}

json preset::Color::Serialize() const
{
	json buf = Preset::Serialize();
	buf["colorcode"] = color.ToInt();
	return buf;
}

PresetPtr Color::Deserializer::operator()(PresetID a_id, std::string a_name, nlohmann::json json) const
{
	if (!json.contains("colorcode"))
		throw std::runtime_error("Requires colorcode");
	uint32_t colorHex = json["colorcode"];
	return std::make_unique<Color>(Color(a_id, a_name, RE::NiColor(colorHex)));
}
