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

	template <typename T>
	std::optional<T*> DrawSelectionComboBox(PresetTID tid, const char* selectionID, PresetID oldID) const;

private:
	std::set<PresetPtr, PresetPtrComparator> presets;
	static constexpr std::string_view        filePath = "./Data/SKSE/Plugins/SceneCraft.json"sv;
};

template <typename T>
inline std::optional<T*> PresetDatabase::DrawSelectionComboBox(PresetTID tid, const char* selectionID, PresetID oldID) const
{
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(selectionID).x - ImGui::GetStyle().ItemSpacing.x);

	auto [st, end] = GetAllPresetsOfType(tid);

	if (st == end)
	{
		if (ImGui::BeginCombo(selectionID, "No Presets Found"))
		{
			ImGui::EndCombo();
		}
		return std::nullopt;
	}

	bool     doUpdate   = false;
	iterator selectedIt = presets.find(oldID);

	if (selectedIt == presets.end())
	{
		doUpdate   = true;
		selectedIt = st;
	}

	Preset* selected = selectedIt->get();
	if (ImGui::BeginCombo(selectionID, (*selectedIt)->GetSelectionName()))
	{
		while (st != end)
		{
			bool isSelected = selectedIt == st;
			if (ImGui::Selectable((*st)->GetSelectionName(), isSelected))
				selected = const_cast<Preset*>(st->get());
			if (isSelected)
				ImGui::SetItemDefaultFocus();
			st++;
		}
		ImGui::EndCombo();
	}

	if (selected != selectedIt->get())
		doUpdate = true;

	auto ans = dynamic_cast<T*>(selected);
	return doUpdate ? std::optional(ans) : std::nullopt;
}
