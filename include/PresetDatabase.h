#pragma once
#include "Preset.h"

class PresetDatabase
{
public:
	typedef std::unique_ptr<Preset>       PresetPtr;
	typedef std::set<PresetPtr>::iterator iterator;

private:
	struct PresetPtrComparator
	{
		using is_transparent = void;

		bool operator()(const PresetID& lhs, const PresetID& rhs) const
		{
			return lhs < rhs;
		}

		bool operator()(const PresetPtr& lhs, const PresetID& rhs) const
		{
			return lhs->GetID() < rhs;
		}

		bool operator()(const PresetID& lhs, const PresetPtr& rhs) const
		{
			return lhs < rhs->GetID();
		}

		bool operator()(const PresetPtr& lhs, const PresetPtr& rhs) const
		{
			return lhs->GetID() < rhs->GetID();
		}
	};

public:
	void                          LoadConfigFromFile();
	void                          SaveConfigToFile();
	std::pair<iterator, iterator> GetAllPresetsOfType(PresetTID type) const;
	bool                          IsEnd(const iterator& it) const;
	iterator                      Find(PresetID key) const;

private:
	std::set<PresetPtr, PresetPtrComparator> presets;
	static constexpr std::string_view        filePath = "./Data/SKSE/Plugins/SceneCraft.json"sv;
};

