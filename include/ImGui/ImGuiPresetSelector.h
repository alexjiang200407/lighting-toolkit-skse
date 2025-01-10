#pragma once
#include "Preset/Preset.h"
#include "Preset/PresetDatabase.h"
#include <type_traits>
#include "ImGui/ImGuiSelector.h"

namespace ImGui
{
	template <
		typename T,
		typename std::enable_if<std::is_base_of<preset::Preset, T>::value>::type* = nullptr>
	class ImGuiPresetSelector :
		public ImGuiSelector<T>
	{
	public:
		ImGuiPresetSelector(const char* selectionID, preset::PresetDatabase* presetDB) :
			ImGuiSelector<T>(selectionID), presetDB(presetDB){};
		ImGuiPresetSelector(const char* selectionID, preset::PresetDatabase* presetDB, T current) :
			ImGuiSelector<T>(selectionID, current),
			presetDB(presetDB)
		{
		}

	public:
		bool DrawValueEditor() override
		{
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ImGuiNavBarItem::GetLabel()).x - ImGui::GetStyle().ItemSpacing.x);

			auto [st, end] = presetDB->GetAllPresetsOfType(T::TID);

			if (st == end)
			{
				if (ImGui::BeginCombo(ImGuiNavBarItem::GetLabel(), "No Presets Found"))
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
			if (ImGui::BeginCombo(ImGuiNavBarItem::GetLabel(), (*selectedIt)->GetSelectionName()))
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
				ImGuiSelector<T>::selection = *ans;
			}

			return doUpdate;
		}
	protected:
		preset::PresetDatabase* presetDB;
	};
}
