#include "MenuState/MenuHidden.h"
#include "MenuState/MenuOpen.h"
#include "ImGui/ImGuiInputAdapter.h"

MenuHidden::MenuHidden(Input::MenuInputContext* inputCtx)
{
	inputCtx->StartLookingAround();
}

MenuStatePtr MenuHidden::Transition(Input::MenuInputContext* inputCtx)
{
	if (ImGui::ImGuiInputAdapter::IsKeyPressed("iHideKey", false))
	{
		return std::make_unique<MenuOpen>(MenuOpen(inputCtx));
	}

	return nullptr;
}

bool MenuHidden::ShouldDrawCursor() const
{
	return false;
}
