#pragma once
#include "Preset.h"
#include "PresetDatabase.h"

namespace preset
{
	class DeserializationStrategy
	{
	public:
		virtual PresetPtr operator()(PresetID id, std::string name, json json) const = 0;
	};

	template <typename T>
	class SerializationStrategy
	{
	public:
		json operator()(const PresetPtr& preset) const
		{
			return ToJSON(dynamic_cast<T*>(preset.get()));
		}

	protected:
		virtual json ToJSON(T* it) const
		{
			json obj    = json::object();
			obj["sid"]  = it->GetSIDAsString();
			obj["name"] = it->GetSelectionName();
			return obj;
		}
	};
}
