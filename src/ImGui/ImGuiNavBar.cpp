#include "ImGui/ImGuiNavBar.h"

ImGui::ImGuiNavBarItem::ImGuiNavBarItem(const char* label) :
	label(label)
{
}

bool ImGui::ImGuiNavBarItem::DrawTabItem()
{
	if (ImGui::BeginTabItem(label.c_str()))
	{
		ImGui::EndTabItem();
		return true;
	}
	return false;
}

const char* ImGui::ImGuiNavBarItem::GetLabel() const
{
	return label.c_str();
}
