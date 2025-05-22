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

	void NavBar(
		const char* str_id,
		const char* nav_item_id[],
		bool        enabled_tabs[],
		size_t      len,
		size_t*     selected        = nullptr,
		bool*       changedSelected = nullptr,
		size_t*     set_selected    = nullptr);

	void Toolbar(
		const char* str_id,
		const char* toolbar_item_id[],
		bool        enabled_items[],
		size_t      len,
		size_t*     selected,
		bool*       changedSelected = nullptr,
		size_t*     set_selected    = nullptr);

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
