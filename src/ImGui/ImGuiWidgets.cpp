#include "ImGuiWidgets.h"

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
