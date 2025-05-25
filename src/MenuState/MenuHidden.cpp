#include "MenuHidden.h"
#include "ImGui/ImGuiInputAdapter.h"
#include "MenuOpen.h"

MenuHidden::MenuHidden(Input::MenuInputContext* inputCtx)
{
	inputCtx->MenuHidden();
	RE::Main::GetSingleton()->freezeTime = true;
}

MenuHidden::~MenuHidden() { RE::Main::GetSingleton()->freezeTime = false; }

MenuStatePtr MenuHidden::Transition(Input::MenuInputContext* inputCtx)
{
	if (ImGui::ImGuiInputAdapter::IsKeyPressed("iHideKey", false))
	{
		return std::make_unique<MenuOpen>(inputCtx);
	}

	return nullptr;
}

bool MenuHidden::ShouldDrawCursor() const { return false; }
