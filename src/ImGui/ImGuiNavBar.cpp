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

ImGui::ImGuiNavBarItem::operator ImGui::ImGuiNavBarItemPtr() const
{
	return std::make_unique<ImGuiNavBarItem>(*this);
}
