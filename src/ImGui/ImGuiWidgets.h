#pragma once

namespace ImGui
{
	void PushFullItemWidth(const char* label = nullptr);

	template <typename T>
	bool SliderAutoFill(
		const char*      label,
		T*               v,
		T                v_min,
		T                v_max,
		const char*      format = nullptr,
		ImGuiSliderFlags flags  = 0)
	{
		PushFullItemWidth(label);

		bool retVal = false;
		if constexpr (std::is_same<T, float>::value)
		{
			retVal = ImGui::SliderFloat(label, v, v_min, v_max, format ? "%.3f" : format, flags);
		}
		else if constexpr (std::is_same<T, int>::value)
		{
			retVal = ImGui::SliderInt(label, v, v_min, v_max, format ? "%d" : format, flags);
		}
		else if constexpr (std::is_same<T, uint8_t>::value || std::is_same_v<T, int8_t>)
		{
			int tmp = static_cast<int>(*v);
			retVal  = ImGui::SliderInt(label, &tmp, v_min, v_max, format ? "%d" : format, flags);
			*v      = static_cast<T>(tmp);
		}
		else
		{
			static_assert(false, "Unsupported type passed to generic SliderAutoFill");
		}
		ImGui::PopItemWidth();
		return retVal;
	}

	template <typename T, typename U, typename F>
	bool SliderAutoFill(
		const char*      label,
		T*               v,
		F                getter,
		U                v_min,
		U                v_max,
		const char*      format = nullptr,
		ImGuiSliderFlags flags  = 0)
	{
		if (v)
		{
			if (U* val = getter(v))
				return ImGui::SliderAutoFill(label, val, v_min, v_max, format, flags);
		}

		U dummy = v_min;
		ImGui::SliderAutoFill(label, &dummy, v_min, v_max, format, flags);
		return false;
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

	bool NiColorEditor(const char* label, RE::NiColor& niColor, ImGuiColorEditFlags flags = 0);

	bool NiPointEditor(
		const char*   xLabel,
		const char*   yLabel,
		const char*   zLabel,
		RE::NiPoint3& niPoint,
		RE::NiPoint3  rangeMin,
		RE::NiPoint3  rangeMax);

	bool ComboWithFilter(
		const char*                     label,
		int*                            currentItem,
		const std::vector<std::string>& items,
		int                             popupMaxHeightInItems);
}
