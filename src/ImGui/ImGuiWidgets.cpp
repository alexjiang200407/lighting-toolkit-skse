#include "ImGui/ImGuiWidgets.h"

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
