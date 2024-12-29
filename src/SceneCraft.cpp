#include "SceneCraft.h"
#include "ImGui/ImGuiInputAdapter.h"

SceneCraft SceneCraft::singleton;

void SceneCraft::Init()
{
	singleton.renderer.RegisterRenderTarget(&singleton);

	ImGui::ImGuiInputAdapter::KeyboardSupressionMask kbd;
	ImGui::ImGuiInputAdapter::MouseSupressionMask mouse;
	kbd.set(RE::BSWin32KeyboardDevice::Key::kW);

	mouse.set(RE::BSWin32MouseDevice::Key::kLeftButton);

	ImGui::ImGuiInputAdapter::GetSingleton()->EnableSupression(kbd, mouse, 0, false, false);
}

void SceneCraft::DoFrame()
{
	ImGui::ShowDemoWindow();
}
