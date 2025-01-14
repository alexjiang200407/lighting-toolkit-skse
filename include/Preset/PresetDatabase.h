#pragma once
#include "Preset.h"

namespace preset
{
	class PresetDatabase
	{
	public:
		typedef std::set<PresetPtr>::iterator       iterator;
		typedef std::set<PresetPtr>::const_iterator const_iterator;

	public:
		PresetDatabase()                                  = default;
		PresetDatabase(const PresetDatabase&&)            = delete;
		PresetDatabase& operator=(const PresetDatabase&&) = delete;
		PresetDatabase(const PresetDatabase&)             = delete;
		PresetDatabase& operator=(const PresetDatabase&)  = delete;

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

			bool operator()(const PresetTID& lhs, const PresetTID& rhs) const
			{
				return lhs < rhs;
			}

			bool operator()(const PresetTID& lhs, const PresetPtr& rhs) const
			{
				return lhs < rhs->GetID().GetTID();
			}

			bool operator()(const PresetPtr& lhs, const PresetTID& rhs) const
			{
				return lhs->GetID().GetTID() < rhs;
			}
		};

	public:
		const_iterator                cbegin() const;
		const_iterator                cend() const;
		std::pair<iterator, iterator> GetAllPresetsOfType(PresetTID type) const;
		bool                          IsEnd(const iterator& it) const;
		iterator                      Find(PresetID key) const;

		template <typename T, typename std::enable_if<std::is_base_of<Preset, T>::value>::type* = nullptr>
		PresetID FirstOfType() const
		{
			iterator lo = presets.lower_bound(T::TID);
			if (lo == presets.end() || (*lo)->GetTID() != T::TID)
			{
				return PresetID();
			}
			return (*lo)->GetID();
		}

		void Insert(PresetPtr preset);

	private:
		std::set<PresetPtr, PresetPtrComparator> presets;
		static constexpr std::string_view        filePath = "./Data/SKSE/Plugins/Chiaroscuro.json"sv;
	};
}
