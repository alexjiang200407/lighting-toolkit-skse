#pragma once
#include "Preset/Preset.h"
#include "Preset/PresetDatabase.h"
#include <type_traits>

namespace ImGui
{
	template <typename T, typename std::enable_if<std::is_base_of<preset::Preset, T>::value>::type* = nullptr>
	class ImGuiSelector
	{
	public:
		ImGuiSelector() = default;
		ImGuiSelector(T current) :
			selection(current)
		{
		}

	public:
		bool DrawSelectionComboBox(const preset::PresetDatabase& presetDB, const char* selectionID)
		{
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(selectionID).x - ImGui::GetStyle().ItemSpacing.x);

			auto [st, end] = presetDB.GetAllPresetsOfType(T::TID);

			if (st == end)
			{
				if (ImGui::BeginCombo(selectionID, "No Presets Found"))
				{
					ImGui::EndCombo();
				}
				selection.reset();
				ImGui::PopItemWidth();
				return false;
			}

			bool                             doUpdate = false;
			preset::PresetDatabase::iterator selectedIt;

			if (selection.has_value())
				selectedIt = presetDB.Find(selection.value().GetID());
			if (!selection.has_value() || presetDB.IsEnd(selectedIt))
			{
				doUpdate   = true;
				selectedIt = st;
			}

			preset::Preset* selected = selectedIt->get();
			if (ImGui::BeginCombo(selectionID, (*selectedIt)->GetSelectionName()))
			{
				while (st != end)
				{
					bool isSelected = selectedIt == st;
					if (ImGui::Selectable((*st)->GetSelectionName(), isSelected))
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
				logger::error("DrawSelectionComboBox could not cast {} to {}", typeid(*selected).name(), typeid(T).name());
				return false;
			}

			if (doUpdate)
			{
				selection = *ans;
			}

			return doUpdate;
		}

		bool HasSelection() const
		{
			return selection.has_value();
		}
		T GetSelection() const
		{
			assert(selection.has_value());
			return selection.value();
		}

	protected:
		std::optional<T> selection;
	};
}
