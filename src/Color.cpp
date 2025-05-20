#include "Color.h"

using namespace preset;

preset::Color::Color(bool isCustom) : Color(RE::NiColor{}, isCustom) {}

preset::Color::Color(RE::NiColor color, bool isCustom) :
	Color(isCustom ? "Custom Color" : "Unnamed Color", color, isCustom)
{}

Color::Color(PresetID id, RE::NiColor color, bool isCustom) :
	Preset(id), RE::NiColor(color), isCustom(isCustom)
{}

Color::Color(std::string name, RE::NiColor color, bool isCustom) :
	Preset(PresetID::GenID<Color>(name)), RE::NiColor(color), isCustom(isCustom)
{}

RE::NiColor Color::GetColor() const { return *this; }

Color preset::Color::Clone(bool newIsCustom) const { return Clone(GetName(), newIsCustom); }

Color preset::Color::Clone(std::string newName, bool newIsCustom) const
{
	return Color(newName, *this, newIsCustom);
}

json preset::Color::Serialize() const
{
	json buf         = Preset::Serialize();
	buf["colorcode"] = ToInt();
	return buf;
}

bool preset::Color::IsCustom() const { return isCustom; }

PresetPtr Color::Deserializer::operator()(PresetID a_id, nlohmann::json json) const
{
	if (!json.contains("colorcode"))
		throw std::runtime_error("Requires colorcode");
	uint32_t colorHex = json["colorcode"];
	return std::make_unique<Color>(Color(a_id, RE::NiColor(colorHex), false));
}
