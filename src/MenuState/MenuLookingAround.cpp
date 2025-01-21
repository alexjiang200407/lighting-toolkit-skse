#include "MenuState/MenuLookingAround.h"
#include "MenuState/MenuOpen.h"

MenuLookingAround::MenuLookingAround(Input::MenuInputContext* inputCtx)
{
	inputCtx->StartLookingAround();
}

MenuStatePtr MenuLookingAround::Transition(Input::MenuInputContext* inputCtx)
{
	if (!ImGui::IsKeyDownA(ImGuiKey_LeftCtrl))
	{
		return std::make_unique<MenuOpen>(MenuOpen(inputCtx));
	}
	return MenuStatePtr(nullptr);
}

void MenuLookingAround::DrawMenu(Chiaroscuro* menu)
{
	ImGui::BeginDisabled();
	{
		MenuOpen::DrawMenu(menu);
	}
	ImGui::EndDisabled();
}

bool MenuLookingAround::ShouldDrawCursor() const
{
	return false;
}
