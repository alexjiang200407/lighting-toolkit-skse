#pragma once
#include "MyEnumSet.h"
#include "Preset/Preset.h"
#include "Preset/PresetDatabase.h"
#include "Preset/SerializationStrategy.h"

namespace preset
{
	struct LightingPreset :
		Preset
	{
		class Deserializer : public DeserializationStrategy
		{
		public:
			PresetPtr operator()(PresetID id, json json) const override;
		};

		enum class Flags
		{
			kPortalStrict = 1,
			kDynamic      = 1 << 1,
			kCastsShadow  = (1 << 2) | kDynamic | kPortalStrict,
			kAffectLand   = 1 << 3,
			kAffectWater  = 1 << 4,
			kAffectAll    = kAffectLand | kAffectWater,
			kNeverFades   = 1 << 5
		};

		typedef lib::EnumSet<Flags, uint32_t> LightFlags;
		LightingPreset() = default;
		LightingPreset(PresetID id, LightFlags flags, float fov, float depthBias, float nearDistance, float intensity, float radius);
		LightingPreset(std::string name, LightFlags flags);

		bool                                     IsShadowLight() const;
		RE::ShadowSceneNode::LIGHT_CREATE_PARAMS ToLightCreateParams() const;
		json                                     Serialize() const override;

		operator RE::ShadowSceneNode::LIGHT_CREATE_PARAMS() const;

		LightFlags flags        = Flags::kAffectAll;
		float      fov          = RE::NI_TWO_PI;
		float      falloff      = 1.0f;
		float      depthBias    = 1.0f;
		float      nearDistance = 6.0f;
		float      intensity    = 1.0f;
		float      radius       = 248.0f;

		static constexpr PresetTID TID = PresetTID::kLightTemplate;
	};
}
