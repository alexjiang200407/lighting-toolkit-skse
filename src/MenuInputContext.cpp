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
	AddModifier(new CanMouseMove(false, ModifierSlots::kPlayerLookAround));
	AddModifier(new CanMouseButton(false, ModifierSlots::kMouseButton));
	Update();
}

void Input::MenuInputContext::StopLookingAround()
{
	AddModifier(new CanMouseMove(true, ModifierSlots::kPlayerLookAround));
	AddModifier(new CanMouseButton(true, ModifierSlots::kMouseButton));
	Update();
}

void Input::MenuInputContext::StartTextInput()
{
	AddModifier(new CanMoveAround(true, ModifierSlots::kPlayerMove));
	AddModifier(new CanCharInput(false, ModifierSlots::kCharInput));
	Update();
}

void Input::MenuInputContext::StopTextInput()
{
	AddModifier(new CanMoveAround(false, ModifierSlots::kPlayerMove));
	AddModifier(new CanCharInput(true, ModifierSlots::kCharInput));
	Update();
}

void Input::MenuInputContext::MenuOpen()
{
	AddModifier(new CanMoveAround(false, ModifierSlots::kPlayerMove));
	Update();
}

void Input::MenuInputContext::MenuClose()
{
	ClearAllModifiers();
	ImGui::ImGuiInputAdapter::GetSingleton()->DisableSupression();
}
