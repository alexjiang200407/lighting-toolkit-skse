#include "ImGui/ImGuiBaseWidget.h"

ImGui::ImGuiBaseWidget::ImGuiBaseWidget() :
	id(uuids::uuid_system_generator{}())
{
}

bool ImGui::ImGuiBaseWidget::operator==(const ImGuiBaseWidget& rhs) const
{
	return id == rhs.id;
}
