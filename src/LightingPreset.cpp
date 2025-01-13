#include "LightingPreset.h"

using namespace preset;

LightingPreset::LightingPreset(PresetID id, LightFlags flags) :
	Preset(id), flags(flags)
{
}

LightingPreset::LightingPreset(std::string name, LightFlags flags) :
	Preset(PresetID::GenID<LightingPreset>(name)), flags(flags)
{
}

bool preset::LightingPreset::IsShadowLight() const
{
	return flags.all(Flags::kCastsShadow);
}

RE::ShadowSceneNode::LIGHT_CREATE_PARAMS LightingPreset::ToLightCreateParams() const
{
	using FLAGS = LightingPreset::Flags;
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS params{};

	params.affectLand      = flags.all(FLAGS::kAffectLand);
	params.affectWater     = flags.all(FLAGS::kAffectWater);
	params.depthBias       = depthBias;
	params.dynamic         = flags.all(FLAGS::kDynamic);
	params.falloff         = falloff;
	params.fov             = fov;
	params.lensFlareData   = nullptr;
	params.nearDistance    = nearDistance;
	params.neverFades      = flags.all(FLAGS::kNeverFades);
	params.portalStrict    = flags.all(FLAGS::kPortalStrict);
	params.restrictedNode  = nullptr;
	params.shadowLight     = flags.all(FLAGS::kCastsShadow);
	params.sceneGraphIndex = 0;

	return params;
}

json preset::LightingPreset::Serialize() const
{
	json buf     = Preset::Serialize();
	buf["flags"] = flags.underlying();
	return buf;
}

LightingPreset::operator RE::ShadowSceneNode::LIGHT_CREATE_PARAMS() const
{
	return ToLightCreateParams();
}

PresetPtr LightingPreset::Deserializer::operator()(PresetID id, json json) const
{
	if (!json.contains("flags"))
		throw std::runtime_error("LightingPreset must include flags field");

	uint32_t lightFlags = json["flags"];
	return std::make_unique<LightingPreset>(LightingPreset(id, LightFlags(lightFlags)));
}
