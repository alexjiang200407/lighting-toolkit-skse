#include "Input/InputModifier/FreeCameraControl.h"

Input::FreeCameraControl::FreeCameraControl() :
	FreeCameraControl(false)
{
}

Input::FreeCameraControl::FreeCameraControl(bool block) :
	CanMouseButton(block), CanMouseMove(block), CanMoveAround(block)
{
}

void Input::FreeCameraControl::ApplyKeySuppressionModifier(InputFilter& filter) const
{
	CanMouseButton::ApplyKeySuppressionModifier(filter);
	CanMoveAround::ApplyKeySuppressionModifier(filter);
}
