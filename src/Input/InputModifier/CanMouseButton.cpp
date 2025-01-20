#include "Input/InputModifier/CanMouseButton.h"

Input::CanMouseButton::CanMouseButton(bool blockMouseButtons, int slot) :
	InputModifier(slot), blockMouseButtons(blockMouseButtons)
{
}

Input::MouseSupressionMask Input::CanMouseButton::ApplyMouseKeyModifier(MouseSupressionMask mouse) const
{
	if (blockMouseButtons)
	{
		mouse.set(RE::BSWin32MouseDevice::Key::kLeftButton);
		mouse.set(RE::BSWin32MouseDevice::Key::kRightButton);
	}
	else
	{
		mouse.reset(RE::BSWin32MouseDevice::Key::kLeftButton);
		mouse.reset(RE::BSWin32MouseDevice::Key::kRightButton);
	}

	return mouse;
}
