#include "Input/InputModifier/FreeCameraControl.h"

Input::FreeCameraControl::FreeCameraControl() :
	FreeCameraControl(false)
{
}

Input::FreeCameraControl::FreeCameraControl(bool block) :
	CanMouseButton(block), CanMouseMove(block), CanMoveAround(block)
{
}
