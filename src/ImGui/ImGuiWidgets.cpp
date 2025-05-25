#include "ImGuiWidgets.h"

void ImGui::NavBar(
	const char* str_id,
	const char* nav_item_id[],
	bool        enabled_tabs[],
	size_t      len,
	size_t*     selected,
	bool*       changedSelected,
	size_t*     set_selected)
{
	if (changedSelected)
		*changedSelected = false;

	if (ImGui::BeginTabBar(str_id))
	{
		for (size_t i = 0; i < len; i++)
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

void ImGui::Toolbar(
	const char* str_id,
	const char* toolbar_item_id[],
	bool        enabled_items[],
	size_t      len,
	size_t*     selected,
	bool*       changedSelected,
	size_t*     set_selected)
{
	assert(selected);
	assert(len < 256);

	if (changedSelected)
		*changedSelected = false;

	ImGuiStyle& style    = ImGui::GetStyle();
	float       spacing  = style.ItemSpacing.x;
	float       paddingX = 10.0f;
	float       paddingY = 8.0f;

	float widths[256] = {};
	float buttonWidth = 0.0f;
	for (size_t i = 0; i < len; i++)
	{
		ImVec2 textSize = ImGui::CalcTextSize(toolbar_item_id[i]);
		widths[i]       = textSize.x + paddingX * 2;
		buttonWidth     = std::max(buttonWidth, widths[i]);
	}

	float totalWidth = (buttonWidth * len) + spacing * (len - 1);
	float cursorX    = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
	ImGui::SetCursorPosX(std::max(cursorX, 0.0f));

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(paddingX, paddingY));

	for (size_t i = 0; i < len; i++)
	{
		ImGui::BeginDisabled(!enabled_items[i]);
		{
			bool didColor = false;

			if (*selected == i)
			{
				auto color = ImColor(33, 118, 255, 255).Value;

				ImGui::PushStyleColor(ImGuiCol_Button, color);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
				didColor = true;
			}

			if (ImGui::Button(toolbar_item_id[i], ImVec2(buttonWidth, 0.0f)))
			{
				if (!set_selected || *set_selected == i)
				{
					*selected = i;
					if (changedSelected)
						*changedSelected = true;
				}
			}

			if (didColor)
				ImGui::PopStyleColor(3);
		}
		ImGui::EndDisabled();

		if (i != len - 1)
			ImGui::SameLine();
	}

	ImGui::PopStyleVar();
}

bool ImGui::IsKeyDownA(ImGuiKey key)
{
	if (ImGui::IsAnyItemActive())
		return false;
	return ImGui::IsKeyDown(key);
}

bool ImGui::IsKeyPressedA(ImGuiKey key, bool repeat)
{
	if (ImGui::IsAnyItemActive())
		return false;
	return ImGui::IsKeyPressed(key, repeat);
}

bool ImGui::ConditionalCheckbox(const char* label, bool cond, bool* v)
{
	bool retVal;
	BeginDisabled(!cond);
	{
		retVal = Checkbox(label, v);
	}
	EndDisabled();
	return retVal;
}

bool ImGui::BeginPanel(const char* str_id)
{
	return ImGui::BeginChild(
		str_id,
		ImVec2(0, 0),
		ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
}

void ImGui::EndPanel() { ImGui::EndChild(); }

bool ImGui::NiColorEditor(const char* label, RE::NiColor& niColor, ImGuiColorEditFlags flags)
{
	float colors[3] = { niColor.red, niColor.green, niColor.blue };
	bool  retVal    = ImGui::ColorEdit3(label, colors);
	niColor.red     = colors[0];
	niColor.green   = colors[1];
	niColor.blue    = colors[2];

	return retVal;
}

bool ImGui::NiPointEditor(
	const char*        xLabel,
	const char*        yLabel,
	const char*        zLabel,
	RE::NiPoint3&      niPoint,
	const RE::NiPoint3 rangeMin,
	const RE::NiPoint3 rangeMax)
{
	assert(rangeMin.x <= rangeMax.x);
	assert(rangeMin.y <= rangeMax.y);
	assert(rangeMin.z <= rangeMax.z);

	bool changed = false;
	changed      = ImGui::SliderAutoFill(xLabel, &niPoint.x, rangeMin.x, rangeMax.x);
	changed      = ImGui::SliderAutoFill(yLabel, &niPoint.y, rangeMin.y, rangeMax.y) || changed;
	changed      = ImGui::SliderAutoFill(zLabel, &niPoint.z, rangeMin.z, rangeMax.z) || changed;
	return changed;
}

void ImGui::PushFullItemWidth(const char* label)
{
	float availableWidth = ImGui::GetContentRegionAvail().x;
	float textSize = label ? CalcTextSize(label).x + ImGui::GetStyle().ItemInnerSpacing.x : 0.0f;
	ImGui::PushItemWidth(availableWidth - textSize);
}
