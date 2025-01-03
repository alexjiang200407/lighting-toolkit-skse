#include "LightingPreset.h"

using namespace preset;

LightingPreset::LightingPreset(PresetID id, std::string name, LightFlags flags) :
	Preset(id, name), flags(flags)
{
}

LightingPreset::LightingPreset(std::string name, LightFlags flags) :
	Preset(PresetID::GenID<LightingPreset>(), name), flags(flags)
{
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

LightingPreset::operator RE::ShadowSceneNode::LIGHT_CREATE_PARAMS() const
{
	return ToLightCreateParams();
}

PresetPtr LightingPreset::Deserializer::operator()(PresetID id, std::string name, json json) const
{
	if (!json.contains("flags"))
		throw std::runtime_error("LightingPreset must include flags field");

	uint32_t lightFlags = json["flags"];
	return std::make_unique<LightingPreset>(LightingPreset(id, name, LightFlags(lightFlags)));
}

json preset::LightingPreset::Serializer::ToJSON(LightingPreset* it) const
{
	json obj = SerializationStrategy<LightingPreset>::ToJSON(it);
	obj["flags"] = it->flags.underlying();	
	return obj;
}
