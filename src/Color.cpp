#include "Color.h"

using namespace preset;

preset::Color::Color() :
	Color("Unnamed Color", {})
{
}

Color::Color(PresetID id, RE::NiColor color) :
	Preset(id), RE::NiColor(color)
{
}

Color::Color(std::string name, RE::NiColor color) :
	Preset(PresetID::GenID<Color>(name)), RE::NiColor(color)
{}

RE::NiColor Color::GetColor() const
{
	return *this;
}

Color preset::Color::Clone() const
{
	return Clone(GetName());
}

Color preset::Color::Clone(std::string newName) const
{
	return Color(GetName(), *this);
}

json preset::Color::Serialize() const
{
	json buf = Preset::Serialize();
	buf["colorcode"] = ToInt();
	return buf;
}

PresetPtr Color::Deserializer::operator()(PresetID a_id, nlohmann::json json) const
{
	if (!json.contains("colorcode"))
		throw std::runtime_error("Requires colorcode");
	uint32_t colorHex = json["colorcode"];
	return std::make_unique<Color>(Color(a_id, RE::NiColor(colorHex)));
}
