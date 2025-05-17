#include "ImGui/ImGuiTabBarAbstract.h"

bool ImGui::ImGuiTabBarItem::operator==(const ImGuiTabBarItem& rhs) const { return id == rhs.id; }

ImGui::TabID ImGui::ImGuiTabBarItem::GetTabID() const { return id; }

void ImGui::ImGuiTabBarItem::SetActive() { setActive = true; }
