#include "Input/InputModifier/BSControlModifier.h"

Input::BSControlModifier::BSControlModifier(std::initializer_list<std::string> controlIDs, bool blockControls, int slot) :
	InputModifier(slot), blockControls(blockControls), controls(controlIDs)
{
}

Input::KeyboardSupressionMask Input::BSControlModifier::ApplyKeyModifier(KeyboardSupressionMask kbd) const
{
	return ApplyBSControlModifier(kbd, RE::INPUT_DEVICE::kKeyboard);
}

Input::GamePadSupressionMask Input::BSControlModifier::ApplyGamepadKeyModifier(GamePadSupressionMask gamepad) const
{
	return ApplyBSControlModifier(gamepad, RE::INPUT_DEVICE::kGamepad);
}
