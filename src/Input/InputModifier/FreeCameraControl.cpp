#include "Input/InputModifier/FreeCameraControl.h"

Input::FreeCameraControl::FreeCameraControl() :
	FreeCameraControl(false)
{
}

Input::FreeCameraControl::FreeCameraControl(bool block) :
	FreeCameraZTranslate(block), CanMouseMove(block), CanMoveAround(block)
{
}

void Input::FreeCameraControl::ApplyKeySuppressionModifier(InputFilter& filter) const
{
	FreeCameraZTranslate::ApplyKeySuppressionModifier(filter);
	CanMoveAround::ApplyKeySuppressionModifier(filter);
}

bool Input::FreeCameraControl::ApplyBlockMouseMoveModifier(bool prevBlockMouseMove) const
{
	return CanMouseMove::ApplyBlockMouseMoveModifier(prevBlockMouseMove);
}
