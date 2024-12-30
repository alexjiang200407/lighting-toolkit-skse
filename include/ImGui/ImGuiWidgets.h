#pragma once

namespace ImGui
{
	template<typename T>
	bool SliderAutoFill(const char* label, T* v, T v_min, T v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
	{
		float availableWidth = ImGui::GetContentRegionAvail().x;

		ImGui::PushItemWidth(availableWidth - CalcTextSize(label).x - ImGui::GetStyle().ItemInnerSpacing.x);

		bool retVal = false;
		if constexpr (std::is_same<T, float>::value)
		{
			retVal = ImGui::SliderFloat(label, v, v_min, v_max, format, flags);
		}
		else if constexpr (std::is_same<T, int>::value)
		{
			retVal = ImGui::SliderInt(label, v, v_min, v_max, flags);
		}

		return retVal;
	}

}
