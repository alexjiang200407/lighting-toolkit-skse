#pragma once
#include "Preset.h"
#include "PresetDatabase.h"
#include <type_traits>
  
namespace ImGui
{
	template <typename T, typename std::enable_if<std::is_base_of<Preset, T>::value>::type* = nullptr>
	class ImGuiSelector
	{
	public:
		ImGuiSelector() = default;
		ImGuiSelector(PresetID id):
			currentSelection(id)
		{

		}
	public:
		T* DrawSelectionComboBox(const PresetDatabase& presetDB, const char* selectionID)
		{
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(selectionID).x - ImGui::GetStyle().ItemSpacing.x);

			auto [st, end] = presetDB.GetAllPresetsOfType(T::TID);

			if (st == end)
			{
				if (ImGui::BeginCombo(selectionID, "No Presets Found"))
				{
					ImGui::EndCombo();
				}
				ImGui::PopItemWidth();
				return nullptr;
			}

			bool doUpdate   = false;
			PresetDatabase::iterator selectedIt;
				
			selectedIt = presetDB.Find(currentSelection);
			if (presetDB.IsEnd(selectedIt))
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
			ImGui::PopItemWidth();

			if (selected != selectedIt->get())
				doUpdate = true;

			auto ans = dynamic_cast<T*>(selected);

			if (!ans)
			{
				logger::error("DrawSelectionComboBox could not cast {} to {}", typeid(*selected).name(), typeid(T).name());
				return nullptr;
			}

			if (doUpdate)
				currentSelection = ans->GetID();

			return doUpdate ? ans : nullptr;
		}

	protected:
		PresetID currentSelection;
	};
}
