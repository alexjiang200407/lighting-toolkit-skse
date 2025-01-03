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

PresetPtr Color::Deserializer::operator()(PresetID a_id, std::string a_name, nlohmann::json json) const
{
	if (!json.contains("colorcode"))
		throw std::runtime_error("Requires colorcode");
	uint32_t colorHex = json["colorcode"];
	return std::make_unique<Color>(Color(a_id, a_name, RE::NiColor(colorHex)));
}

json preset::Color::Serializer::ToJSON(Color* a_color) const
{
	json obj         = SerializationStrategy<Color>::ToJSON(a_color);
	obj["colorcode"] = a_color->color.ToInt();
	return obj;
}
