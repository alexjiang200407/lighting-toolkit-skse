#include "MenuLookingAround.h"
#include "../ImGui/ImGuiInputAdapter.h"
#include "MenuOpen.h"

MenuLookingAround::MenuLookingAround(Input::MenuInputContext* inputCtx)
{
	inputCtx->StartLookingAround();
}

MenuStatePtr MenuLookingAround::Transition(Input::MenuInputContext* inputCtx)
{
	if (!ImGui::ImGuiInputAdapter::IsKeyDown("iLookAroundKey"))
	{
		return std::make_unique<MenuOpen>(MenuOpen(inputCtx));
	}
	return MenuStatePtr(nullptr);
}

void MenuLookingAround::DrawMenu(LightingToolkit* menu)
{
	ImGui::BeginDisabled();
	{
		MenuOpen::DrawMenu(menu);
	}
	ImGui::EndDisabled();
}

bool MenuLookingAround::ShouldDrawCursor() const { return false; }
