#include "MenuInputContext.h"
#include "ImGui/ImGuiInputAdapter.h"
#include "Input/InputModifier/CanCharInput.h"
#include "Input/InputModifier/CanMouseMove.h"
#include "Input/InputModifier/CanMoveAround.h"
#include "Input/InputModifier/CanRun.h"
#include "Input/InputModifier/FreeCameraControl.h"

Input::MenuInputContext::MenuInputContext() {}

void Input::MenuInputContext::StartLookingAround()
{
	AddModifier(kFreeCameraMoveMode, new FreeCameraControl());
}

void Input::MenuInputContext::StopLookingAround()
{
	AddModifier(kFreeCameraMoveMode, new CanMoveAround(false));
}

void Input::MenuInputContext::StartTextInput()
{
	AddModifier(kTextInput, new FreeCameraControl(true));
}

void Input::MenuInputContext::StopTextInput() { RemoveModifier(kTextInput); }

void Input::MenuInputContext::StopAllInput() { ClearAllModifiers(); }

void Input::MenuInputContext::MenuOpen()
{
	AddModifier(kCanRun, new CanRun(false));
	AddModifier(kFreeCameraLook, new CanMouseMove(true));
	AddModifier(kFreeCameraMoveMode, new CanMoveAround(false));
}

void Input::MenuInputContext::MenuHidden()
{
	AddModifier(kCanRun, new CanRun(false));
	AddModifier(kFreeCameraLook, new CanMouseMove(false));
	AddModifier(kFreeCameraMoveMode, new FreeCameraControl(false));
}

void Input::MenuInputContext::MenuClose()
{
	ClearAllModifiers();
	ImGui::ImGuiInputAdapter::GetSingleton()->DisableSupression();
}

void Input::MenuInputContext::EnablePositioning() { allowPositioning = true; }

void Input::MenuInputContext::DisablePositioning() { allowPositioning = false; }

bool Input::MenuInputContext::AllowsPositioning() const { return allowPositioning; }
