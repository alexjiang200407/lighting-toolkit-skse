#pragma once
#include "Preset/Preset.h"
#include "Preset/SerializationStrategy.h"
#include "ModelPreset.h"

namespace preset
{
	class LightModelPreset :
		public ModelPreset
	{
	public:
		class Deserializer : DeserializationStrategy
		{
		public:
			PresetPtr operator()(PresetID id, std::string name, json json) const override;
		};

	public:
		LightModelPreset() = default;
		LightModelPreset(PresetID id, std::string name, RE::FormID modelSubID, std::string file, RE::NiPoint3 offset);

	public:
		RE::NiPoint3 GetOffset() const;
	public:
		static constexpr PresetTID TID = PresetTID::kLightModel;

	private:
		RE::NiPoint3 offset;
	};
}
