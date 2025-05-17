#include "ImGui/ImGuiNavBar.h"

typedef ImGui::ImGuiTabBarAbstract<std::string, std::vector<std::string>::iterator> abc;

ImGui::ImGuiNavBarItem::ImGuiNavBarItem(const char* label) : label(label)
{
	//abc a{ "asdasd", 0 };
}

bool ImGui::ImGuiNavBarItem::DrawTabItem()
{
	ImGuiTabItemFlags flags = 0;
	if (setActive)
	{
		flags |= ImGuiTabItemFlags_SetSelected;
		setActive = false;
	}
	if (ImGui::BeginTabItem(label.c_str(), nullptr, flags))
	{
		ImGui::EndTabItem();
		return true;
	}
	return false;
}

const char* ImGui::ImGuiNavBarItem::GetLabel() const { return label.c_str(); }
