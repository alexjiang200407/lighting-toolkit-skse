#include "MenuState/MenuOpen.h"
#include "MenuState/MenuHidden.h"
#include "MenuState/MenuLookingAround.h"
#include "MenuState/MenuPositioning.h"
#include "MenuState/MenuItemActive.h"

MenuOpen::MenuOpen(Input::MenuInputContext* inputCtx)
{
	inputCtx->MenuOpen();
	inputCtx->StopTextInput();
}

MenuStatePtr MenuOpen::Transition(Input::MenuInputContext* inputCtx)
{
	if (ImGui::IsKeyPressedA(ImGuiKey_H, false))
	{
		return std::make_unique<MenuHidden>(inputCtx);
	}
	if (ImGui::IsKeyDownA(ImGuiKey_LeftCtrl))
	{
		return std::make_unique<MenuLookingAround>(inputCtx);
	}
	if (ImGui::IsKeyDownA(ImGuiKey_LeftAlt))
	{
		return std::make_unique<MenuPositioning>(inputCtx);
	}
	if (ImGui::GetIO().WantTextInput)
	{
		return std::make_unique<MenuItemActive>(inputCtx);
	}
	return nullptr;
}

void MenuOpen::DoFrame(Chiaroscuro* menu)
{
	ImGui::Begin("##SCMain", nullptr, windowFlags);
	{
		DrawMenu(menu);
	}
	ImGui::End();
}

void MenuOpen::DrawMenu(Chiaroscuro* menu)
{
	menu->DrawTabBar();
	menu->DrawPropControlWindow();
	menu->DrawCameraControlWindow();
	menu->DrawSceneControlWindow();
}
