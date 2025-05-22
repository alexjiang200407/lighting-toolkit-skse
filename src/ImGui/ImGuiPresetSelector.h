#pragma once
#include "ImGui/ImGuiSelector.h"
#include "Preset/Preset.h"
#include "Preset/PresetDatabase.h"
#include <type_traits>

namespace ImGui
{
	template <typename T>
	class ImGuiPresetSelector : public ImGuiSelector<T>
	{
		ASSERT_BASE(T, preset::Preset);

	public:
		ImGuiPresetSelector(const char* selectionID, preset::PresetDatabase* presetDB,std::optional<T> current = std::nullopt) :
			ImGuiSelector<T>(selectionID, current), presetDB(presetDB){};

	public:
		bool DrawValueEditor() override
		{
			const char* label = ImGuiSelector<T>::label.c_str();
			ImGui::PushItemWidth(
				ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x -
				ImGui::GetStyle().ItemSpacing.x);

			auto [st, end] = presetDB->GetAllPresetsOfType(T::TID);

			if (st == end)
			{
				if (ImGui::BeginCombo(label, "No Presets Found"))
				{
					ImGui::EndCombo();
				}
				ImGuiSelector<T>::selection.reset();
				ImGui::PopItemWidth();
				return false;
			}

			bool                             doUpdate = false;
			preset::PresetDatabase::iterator selectedIt;

			if (ImGuiSelector<T>::selection.has_value())
				selectedIt = presetDB->Find(ImGuiSelector<T>::selection.value().GetID());
			if (!ImGuiSelector<T>::selection.has_value() || presetDB->IsEnd(selectedIt))
			{
				doUpdate   = true;
				selectedIt = st;
			}

			preset::Preset* selected = selectedIt->get();
			if (ImGui::BeginCombo(label, (*selectedIt)->GetSelectionName()))
			{
				while (st != end)
				{
					bool isSelected = selectedIt == st;
					if (ImGui::Selectable((*st)->GetUniqueName(), isSelected))
						selected = const_cast<preset::Preset*>(st->get());
					if (isSelected)
						ImGui::SetItemDefaultFocus();
					st++;
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			if (selected != selectedIt->get())
				doUpdate = true;

			auto ans = dynamic_cast<T*>(selected);

			if (!ans)
			{
				logger::error(
					"DrawSelectionComboBox could not cast {} to {}",
					typeid(*selected).name(),
					typeid(T).name());
				return false;
			}

			if (doUpdate)
			{
				ImGuiSelector<T>::selection = *ans;
			}

			return doUpdate;
		}

	protected:
		preset::PresetDatabase* presetDB;
	};
}
