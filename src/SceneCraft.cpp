#include "SceneCraft.h"
#include "ImGui/ImGuiInputAdapter.h"

SceneCraft SceneCraft::singleton;

void SceneCraft::Init()
{
	singleton.renderer.RegisterRenderTarget(&singleton);
}

void SceneCraft::DoFrame()
{
	if (ImGui::IsKeyPressed(ImGuiKey_End, false))
	{
		ToggleMenu();
	}
	
	if (showWindow)
	{
		ImGui::ShowDemoWindow();
	}
}

void SceneCraft::ToggleMenu()
{
	showWindow = !showWindow;

	if (showWindow)
	{
		ImGui::ImGuiInputAdapter::KeyboardSupressionMask kbd;
		ImGui::ImGuiInputAdapter::MouseSupressionMask    mouse;
		ImGui::ImGuiInputAdapter::GamePadSupressionMask  gamepad;

		kbd.set();
		mouse.set();
		gamepad.set();

		ImGui::ImGuiInputAdapter::GetSingleton()->EnableSupression(kbd, mouse, gamepad, true, true);
	}
	else
	{
		ImGui::ImGuiInputAdapter::GetSingleton()->DisableSupression();
	}
}
