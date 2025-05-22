#pragma once

namespace ImGui
{
	template <typename T>
	bool SliderAutoFill(
		const char*      label,
		T*               v,
		T                v_min,
		T                v_max,
		const char*      format = "%.3f",
		ImGuiSliderFlags flags  = 0)
	{
		float availableWidth = ImGui::GetContentRegionAvail().x;

		ImGui::PushItemWidth(
			availableWidth - CalcTextSize(label).x - ImGui::GetStyle().ItemInnerSpacing.x);

		bool retVal = false;
		if constexpr (std::is_same<T, float>::value)
		{
			retVal = ImGui::SliderFloat(label, v, v_min, v_max, format, flags);
		}
		else if constexpr (std::is_same<T, int>::value)
		{
			retVal = ImGui::SliderInt(label, v, v_min, v_max, flags);
		}
		ImGui::PopItemWidth();
		return retVal;
	}

	template <std::size_t N>
	void NavBar(
		const char* str_id,
		const char* (&nav_item_id)[N],
		bool        (&enabled_tabs)[N],
		size_t*     selected        = nullptr,
		bool*       changedSelected = nullptr,
		size_t*     set_selected    = nullptr)
	{
		if (changedSelected)
			*changedSelected = false;

		if (ImGui::BeginTabBar(str_id))
		{
			for (size_t i = 0; i < N; i++)
			{
				ImGuiTabBarFlags flags = ImGuiTabItemFlags_None;

				if (set_selected && i == *set_selected)
				{
					flags = ImGuiTabItemFlags_SetSelected;
				}

				ImGui::BeginDisabled(!enabled_tabs[i]);
				{
					if (ImGui::BeginTabItem(nav_item_id[i], nullptr, flags))
					{
						if ((!set_selected || *set_selected == i) && selected)
						{
							*selected = i;

							if (changedSelected)
								*changedSelected = true;
						}
						ImGui::EndTabItem();
					}
				}
				ImGui::EndDisabled();
			}
			ImGui::EndTabBar();
		}
	}

	bool IsKeyDownA(ImGuiKey key);
	bool IsKeyPressedA(ImGuiKey key, bool repeat = true);
	bool ConditionalCheckbox(const char* label, bool cond, bool* v);

	IMGUI_API bool InputText(
		const char*            label,
		std::string*           str,
		ImGuiInputTextFlags    flags     = 0,
		ImGuiInputTextCallback callback  = NULL,
		void*                  user_data = NULL);


	bool BeginPanel(const char* str_id);
	void EndPanel();
}
