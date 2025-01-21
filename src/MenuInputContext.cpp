#include "MenuInputContext.h"
#include "Input/InputModifier/CanMoveAround.h"
#include "Input/InputModifier/CanMouseMove.h"
#include "Input/InputModifier/CanCharInput.h"
#include "Input/InputModifier/CanMouseButton.h"
#include "ImGui/ImGuiInputAdapter.h"

Input::MenuInputContext::MenuInputContext() :
	InputContext({ KeyboardSupressionMask().set(), MouseSupressionMask().set(), GamePadSupressionMask().set(), true, true })
{
}

void Input::MenuInputContext::StartLookingAround()
{
	AddModifier(ModifierSlots::kPlayerLookAround, new CanMouseMove(false));
	AddModifier(ModifierSlots::kMouseButton, new CanMouseButton(false));
	Update();
}

void Input::MenuInputContext::StopLookingAround()
{
	AddModifier(ModifierSlots::kPlayerLookAround, new CanMouseMove(true));
	AddModifier(ModifierSlots::kMouseButton, new CanMouseButton(true));
	Update();
}

void Input::MenuInputContext::StartTextInput()
{
	AddModifier(ModifierSlots::kPlayerMove, new CanMoveAround(true));
	AddModifier(ModifierSlots::kCharInput, new CanCharInput(false));
	Update();
}

void Input::MenuInputContext::StopTextInput()
{
	AddModifier(ModifierSlots::kPlayerMove, new CanMoveAround(false));
	AddModifier(ModifierSlots::kCharInput, new CanCharInput(true));
	Update();
}

void Input::MenuInputContext::MenuOpen()
{
	AddModifier(ModifierSlots::kPlayerMove, new CanMoveAround(false));
	Update();
}

void Input::MenuInputContext::MenuClose()
{
	ClearAllModifiers();
	ImGui::ImGuiInputAdapter::GetSingleton()->DisableSupression();
}
