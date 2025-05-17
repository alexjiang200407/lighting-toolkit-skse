#include "LightingPreset.h"

using namespace preset;

LightingPreset::LightingPreset(
	PresetID   id,
	LightFlags flags,
	float      fov,
	float      depthBias,
	float      nearDistance,
	float      intensity,
	float      radius) :
	Preset(id),
	flags(flags), fov(fov * (RE::NI_PI / 180.0f)), depthBias(depthBias), nearDistance(nearDistance),
	intensity(intensity), radius(radius)
{}

LightingPreset::LightingPreset(std::string name, LightFlags flags) :
	Preset(PresetID::GenID<LightingPreset>(name)), flags(flags)
{}

bool preset::LightingPreset::IsShadowLight() const { return flags.all(Flags::kCastsShadow); }

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
	json buf            = Preset::Serialize();
	buf["flags"]        = flags.underlying();
	buf["intensity"]    = intensity;
	buf["radius"]       = radius;
	buf["fov"]          = fov;
	buf["depthBias"]    = depthBias;
	buf["falloff"]      = falloff;
	buf["nearDistance"] = nearDistance;
	return buf;
}

LightingPreset::operator RE::ShadowSceneNode::LIGHT_CREATE_PARAMS() const
{
	return ToLightCreateParams();
}

PresetPtr LightingPreset::Deserializer::operator()(PresetID a_id, json json) const
{
	static constexpr const char* requiredKeys[] = { "flags",     "intensity", "radius",      "fov",
		                                            "depthBias", "falloff",   "nearDistance" };

	for (auto key : requiredKeys)
	{
		if (!json.contains(key))
		{
			logger::error("field {} missing", key);
			throw std::runtime_error("LightingPreset has incorrect fields!!");
		}
	}

	uint32_t lightFlags = json["flags"];
	return std::make_unique<LightingPreset>(LightingPreset(
		a_id,
		LightFlags(lightFlags),
		json["fov"],
		json["depthBias"],
		json["nearDistance"],
		json["intensity"],
		json["radius"]));
}
