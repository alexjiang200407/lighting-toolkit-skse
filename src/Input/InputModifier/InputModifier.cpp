#include "Input/InputModifier/InputModifier.h"

Input::InputModifier::InputModifier(int slot) :
	slot(slot)
{
}

Input::KeyboardSupressionMask Input::InputModifier::ApplyKeyModifier(KeyboardSupressionMask kbd) const
{
	return kbd;
}

Input::MouseSupressionMask Input::InputModifier::ApplyMouseKeyModifier(MouseSupressionMask mouse) const
{
	return mouse;
}

Input::GamePadSupressionMask Input::InputModifier::ApplyGamepadKeyModifier(GamePadSupressionMask gamepad) const
{
	return gamepad;
}

bool Input::InputModifier::ApplyBlockCharModifier(bool prevBlockChars) const
{
	return prevBlockChars;
}

bool Input::InputModifier::ApplyBlockMouseMoveModifier(bool prevBlockMouseMove) const
{
	return prevBlockMouseMove;
}

Input::InputFilter Input::InputModifier::ApplyModifier(InputFilter filter) const
{
	filter.blockCharEvents      = ApplyBlockCharModifier(filter.blockCharEvents);
	filter.blockMouseMoveEvents = ApplyBlockMouseMoveModifier(filter.blockMouseMoveEvents);
	filter.gamepadSuppress      = ApplyGamepadKeyModifier(filter.gamepadSuppress);
	filter.mouseSuppress        = ApplyMouseKeyModifier(filter.mouseSuppress);
	filter.kbdSuppress          = ApplyKeyModifier(filter.kbdSuppress);
	return filter;
}

bool Input::InputModifier::operator<(const InputModifier& rhs) const
{
	return slot < rhs.slot;
}

int Input::InputModifier::GetSlot() const
{
	return slot;
}
