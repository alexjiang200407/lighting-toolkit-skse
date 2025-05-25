#include "MenuState/MenuPositioning.h"
#include "ImGui/ImGuiInputAdapter.h"
#include "MenuState/MenuOpen.h"

MenuPositioning::MenuPositioning(Input::MenuInputContext* inputCtx) : MenuLookingAround(inputCtx)
{
	RE::Main::GetSingleton()->freezeTime = true;
}

MenuPositioning::~MenuPositioning() { RE::Main::GetSingleton()->freezeTime = false; }

MenuStatePtr MenuPositioning::Transition(Input::MenuInputContext* inputCtx)
{
	if (!ImGui::ImGuiInputAdapter::IsKeyDown("iPositionLightKey"))
	{
		return std::make_unique<MenuOpen>(inputCtx);
	}
	return MenuStatePtr(nullptr);
}

void MenuPositioning::DrawMenu(LightingToolkit* menu)
{
	MenuLookingAround::DrawMenu(menu);
	menu->Position();
}
