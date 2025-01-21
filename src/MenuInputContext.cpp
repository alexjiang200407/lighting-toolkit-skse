#include "MenuInputContext.h"
#include "Input/InputModifier/CanMoveAround.h"
#include "Input/InputModifier/CanMouseMove.h"
#include "Input/InputModifier/CanCharInput.h"
#include "Input/InputModifier/FreeCameraControl.h"
#include "ImGui/ImGuiInputAdapter.h"

Input::MenuInputContext::MenuInputContext() :
	InputContext({ KeyboardSupressionMask().set(), MouseSupressionMask().set(), GamePadSupressionMask().set(), true, true })
{
}

void Input::MenuInputContext::StartLookingAround()
{
	AddModifier(kFreeCameraMoveMode, new FreeCameraControl());
	Update();
}

void Input::MenuInputContext::StopLookingAround()
{
	AddModifier(kFreeCameraMoveMode, new CanMoveAround(false));
	Update();
}

void Input::MenuInputContext::StartTextInput()
{
	AddModifier(kTextInput, new CanMoveAround(true));
	Update();
}

void Input::MenuInputContext::StopTextInput()
{
	RemoveModifier(kTextInput);
	Update();
}

void Input::MenuInputContext::MenuOpen()
{
	AddModifier(kFreeCameraMoveMode, new CanMoveAround(false));
	Update();
}

void Input::MenuInputContext::MenuClose()
{
	ClearAllModifiers();
	ImGui::ImGuiInputAdapter::GetSingleton()->DisableSupression();
}
