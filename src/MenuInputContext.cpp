#include "MenuInputContext.h"
#include "Input/InputModifier/CanMoveAround.h"
#include "Input/InputModifier/CanMouseMove.h"
#include "Input/InputModifier/CanCharInput.h"
#include "Input/InputModifier/FreeCameraControl.h"
#include "ImGui/ImGuiInputAdapter.h"
#include "Input/InputModifier/CanMouseMove.h"

Input::MenuInputContext::MenuInputContext()
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
	AddModifier(kTextInput, new FreeCameraControl(true));
	Update();
}

void Input::MenuInputContext::StopTextInput()
{
	RemoveModifier(kTextInput);
	Update();
}

void Input::MenuInputContext::MenuOpen()
{
	AddModifier(kFreeCameraLook, new CanMouseMove(true));
	AddModifier(kFreeCameraMoveMode, new CanMoveAround(false));
	Update();
}

void Input::MenuInputContext::MenuClose()
{
	ClearAllModifiers();
	ImGui::ImGuiInputAdapter::GetSingleton()->DisableSupression();
}
