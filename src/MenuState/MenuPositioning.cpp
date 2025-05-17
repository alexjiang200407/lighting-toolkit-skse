#include "MenuState/MenuPositioning.h"
#include "ImGui/ImGuiInputAdapter.h"

MenuPositioning::MenuPositioning(Input::MenuInputContext* inputCtx) :
	MenuLookingAround(inputCtx)
{
	prevFreezeTime                       = RE::Main::GetSingleton()->freezeTime;
	RE::Main::GetSingleton()->freezeTime = true;
}

MenuPositioning::~MenuPositioning()
{
	RE::Main::GetSingleton()->freezeTime = prevFreezeTime;
}

MenuStatePtr MenuPositioning::Transition(Input::MenuInputContext* inputCtx)
{
	if (!ImGui::ImGuiInputAdapter::IsKeyDown("iPositionLightKey"))
	{
		return std::make_unique<MenuOpen>(MenuOpen(inputCtx));
	}
	return MenuStatePtr(nullptr);
}

void MenuPositioning::DrawMenu(LightingToolkit* menu)
{
	MenuLookingAround::DrawMenu(menu);
	menu->PositionLight();
}
