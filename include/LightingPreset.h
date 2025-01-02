#pragma once
#include "Preset.h"

struct LightingPreset :
	Preset
{
	enum class Flags
	{
		kPortalStrict = 1,
		kDynamic      = 1 << 1,
		kCastsShadow  = (1 << 2) | kDynamic,
		kAffectLand   = 1 << 3,
		kAffectWater  = 1 << 4,
		kAffectAll    = kAffectLand | kAffectWater,
		kNeverFades   = 1 << 5
	};
	typedef stl::enumeration<Flags, uint32_t> LightFlags;
	LightingPreset() = default;
	LightingPreset(PresetID id, std::string name, LightFlags flags);


	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS ToLightCreateParams() const;

	operator RE::ShadowSceneNode::LIGHT_CREATE_PARAMS() const;

	LightFlags  flags;
	float       fov = RE::NI_PI;
	float       falloff = 1.0f;
	float       depthBias = 1.0f;
	float       nearDistance = 6.0f;
};
