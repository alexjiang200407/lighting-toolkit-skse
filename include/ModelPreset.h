#pragma once
#include "Preset/Preset.h"
#include "Preset/SerializationStrategy.h"

namespace preset
{
	class ModelPreset :
		public Preset
	{
	public:
		class Deserializer : DeserializationStrategy
		{
		public:
			PresetPtr operator()(PresetID id, std::string name, json json) const override;
		};

	public:
		ModelPreset() = default;
		ModelPreset(PresetID id, std::string name, RE::FormID modelSubID, std::string file);

	public:
		RE::FormID          GetFullFormID();
		RE::TESBoundObject* ToBoundObj();

	public:
		static constexpr PresetTID TID = PresetTID::kProp;

	private:
		RE::FormID  modelSubID;
		RE::FormID  modelFullID = 0x0;
		std::string file;
	};
}
