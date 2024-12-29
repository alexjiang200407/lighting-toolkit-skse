#include "ImGui/ImGuiComponent.h"

bool ImGui::ImGuiComponent::ShouldSkip() { return false; }

bool ImGui::ImGuiComponent::ShouldDrawCursor()
{
	return false;
}
