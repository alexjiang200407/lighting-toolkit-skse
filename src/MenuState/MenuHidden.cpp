#include "MenuState/MenuHidden.h"
#include "MenuState/MenuOpen.h"

MenuHidden::MenuHidden(Input::MenuInputContext* inputCtx)
{
	inputCtx->StartLookingAround();
}

MenuStatePtr MenuHidden::Transition(Input::MenuInputContext* inputCtx)
{
	if (ImGui::IsKeyPressedA(ImGuiKey_H, false))
	{
		return std::make_unique<MenuOpen>(MenuOpen(inputCtx));
	}

	return nullptr;
}

bool MenuHidden::ShouldDrawCursor() const
{
	return false;
}
