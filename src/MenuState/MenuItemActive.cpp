#include "MenuItemActive.h"
#include "MenuOpen.h"

MenuItemActive::MenuItemActive(Input::MenuInputContext* inputCtx) : MenuOpen(inputCtx)
{
	inputCtx->StartTextInput();
}

MenuStatePtr MenuItemActive::Transition(Input::MenuInputContext* inputCtx)
{
	if (!ImGui::GetIO().WantTextInput)
	{
		return std::make_unique<MenuOpen>(inputCtx);
	}
	return nullptr;
}
