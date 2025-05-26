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

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "common/fts_fuzzy_match.h"

static int IndexOfKey(std::vector<std::pair<int, int>> pairList, int key)
{
	for (int i = 0; i < pairList.size(); ++i)
	{
		auto& p = pairList[i];
		if (p.first == key)
		{
			return i;
		}
	}
	return -1;
}

// Copied from imgui_widgets.cpp
static float CalcMaxPopupHeightFromItemCount(int itemsCount)
{
	ImGuiContext& g = *GImGui;
	if (itemsCount <= 0)
	{
		return FLT_MAX;
	}
	return (g.FontSize + g.Style.ItemSpacing.y) * itemsCount - g.Style.ItemSpacing.y +
	       (g.Style.WindowPadding.y * 2);
}

// https://github.com/mbiron/MBIMGUI/blob/05f10e18a081673e2d0921e469e663959506f824/Sources/Widgets/imgui_combowithfilter.cpp#L90

void ImGui::PushFullItemWidth(const char* label)
{
	float availableWidth = ImGui::GetContentRegionAvail().x;
	float textSize = label ? CalcTextSize(label).x + ImGui::GetStyle().ItemInnerSpacing.x : 0.0f;
	ImGui::PushItemWidth(availableWidth - textSize);
}

bool ImGui::ComboWithFilter(
	const char*                     label,
	int*                            currentItem,
	const std::vector<std::string>& items,
	int                             popupMaxHeightInItems /*= -1 */)
{
	using namespace fts;

	ImGuiContext& g = *GImGui;

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
	{
		return false;
	}

	int itemsCount = static_cast<int>(items.size());

	// Use imgui Items_ getters to support more input formats.
	const char* previewValue = NULL;
	if (*currentItem >= 0 && *currentItem < itemsCount)
	{
		previewValue = items[*currentItem].c_str();
	}

	static int  focusIndex         = -1;
	static char patternBuffer[256] = { 0 };

	bool valueChanged = false;

	const ImGuiID id            = window->GetID(label);
	const ImGuiID popupId       = ImHashStr("##ComboPopup", 0, id);  // copied from BeginCombo
	const bool    isAlreadyOpen = IsPopupOpen(popupId, ImGuiPopupFlags_None);
	const bool    isFiltering   = isAlreadyOpen && patternBuffer[0] != '\0';

	int showCount = itemsCount;

	std::vector<std::pair<int, int>> itemScoreVector;
	if (isFiltering)
	{
		// Filter before opening to ensure we show the correct size window.
		// We won't get in here unless the popup is open.
		for (int i = 0; i < itemsCount; i++)
		{
			int  score   = 0;
			bool matched = fuzzy_match(patternBuffer, items[i].c_str(), score);
			if (matched)
			{
				itemScoreVector.push_back(std::make_pair(i, score));
			}
		}
		std::sort(
			itemScoreVector.begin(),
			itemScoreVector.end(),
			[](const std::pair<int, int>& a, const std::pair<int, int>& b) {
				return (b.second < a.second);
			});
		int currentScoreIndex = IndexOfKey(itemScoreVector, focusIndex);
		if (currentScoreIndex < 0 && !itemScoreVector.empty())
		{
			focusIndex = itemScoreVector[0].first;
		}
		showCount = static_cast<int>(itemScoreVector.size());
	}

	// Define the height to ensure our size calculation is valid.
	if (popupMaxHeightInItems == -1)
	{
		popupMaxHeightInItems = 5;
	}
	popupMaxHeightInItems = ImMin(popupMaxHeightInItems, showCount);

	if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
	{
		SetNextWindowSizeConstraints(
			ImVec2(0, 0),
			ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popupMaxHeightInItems + 3)));
	}

	if (!BeginCombo(label, previewValue, ImGuiComboFlags_None))
		return false;

	if (!isAlreadyOpen)
	{
		focusIndex = *currentItem;
		memset(patternBuffer, 0, IM_ARRAYSIZE(patternBuffer));
	}

	ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(240, 240, 240, 255));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0, 255));
	ImGui::PushItemWidth(-FLT_MIN);
	// Filter input
	if (!isAlreadyOpen)
		ImGui::SetKeyboardFocusHere();
	InputText("##ComboWithFilter_inputText", patternBuffer, 256, ImGuiInputTextFlags_AutoSelectAll);

	ImGui::PopStyleColor(2);

	int move_delta = 0;
	if (IsKeyPressed(ImGuiKey_UpArrow))
	{
		--move_delta;
	}
	else if (IsKeyPressed(ImGuiKey_DownArrow))
	{
		++move_delta;
	}
	else if (IsKeyPressed(ImGuiKey_PageUp))
	{
		move_delta -= popupMaxHeightInItems;
	}
	else if (IsKeyPressed(ImGuiKey_PageDown))
	{
		move_delta += popupMaxHeightInItems;
	}

	if (move_delta != 0)
	{
		if (isFiltering)
		{
			int currentScoreIndex = IndexOfKey(itemScoreVector, focusIndex);
			if (currentScoreIndex >= 0)
			{
				const int count   = static_cast<int>(itemScoreVector.size());
				currentScoreIndex = ImClamp(currentScoreIndex + move_delta, 0, count - 1);
				focusIndex        = itemScoreVector[currentScoreIndex].first;
			}
		}
		else
		{
			focusIndex = ImClamp(focusIndex + move_delta, 0, itemsCount - 1);
		}
	}

	// Copied from ListBoxHeader
	// If popup_max_height_in_items == -1, default height is maximum 7.
	float heightInItems =
		(popupMaxHeightInItems < 0 ? ImMin(itemsCount, 7) : popupMaxHeightInItems) + 0.25f;
	ImVec2 size;
	size.x = 0.0f;
	size.y = GetTextLineHeightWithSpacing() * heightInItems + g.Style.FramePadding.y * 2.0f;

	if (ImGui::BeginListBox("##ComboWithFilter_itemList", size))
	{
		for (int i = 0; i < showCount; i++)
		{
			int idx = isFiltering ? itemScoreVector[i].first : i;
			PushID((void*)(intptr_t)idx);
			const bool  itemSelected = (idx == focusIndex);
			const char* itemText     = items[idx].c_str();
			if (Selectable(itemText, itemSelected))
			{
				valueChanged = true;
				*currentItem = idx;
				CloseCurrentPopup();
			}

			if (itemSelected)
			{
				SetItemDefaultFocus();
				// SetItemDefaultFocus doesn't work so also check IsWindowAppearing.
				if (move_delta != 0 || IsWindowAppearing())
				{
					SetScrollHereY();
				}
			}
			PopID();
		}
		ImGui::EndListBox();

		if (IsKeyPressed(ImGuiKey_Enter))
		{
			valueChanged = true;
			*currentItem = focusIndex;
			CloseCurrentPopup();
		}
		else if (IsKeyPressed(ImGuiKey_Escape))
		{
			valueChanged = false;
			CloseCurrentPopup();
		}
	}
	ImGui::PopItemWidth();
	ImGui::EndCombo();

	if (valueChanged)
		MarkItemEdited(g.LastItemData.ID);

	return valueChanged;
}
