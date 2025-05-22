#include "MenuHidden.h"
#include "ImGui/ImGuiInputAdapter.h"
#include "MenuOpen.h"

MenuHidden::MenuHidden(Input::MenuInputContext* inputCtx) { inputCtx->StopAllInput(); }

MenuStatePtr MenuHidden::Transition(Input::MenuInputContext* inputCtx)
{
	if (ImGui::ImGuiInputAdapter::IsKeyPressed("iHideKey", false))
	{
		return std::make_unique<MenuOpen>(MenuOpen(inputCtx));
	}

	return nullptr;
}

bool MenuHidden::ShouldDrawCursor() const { return false; }
