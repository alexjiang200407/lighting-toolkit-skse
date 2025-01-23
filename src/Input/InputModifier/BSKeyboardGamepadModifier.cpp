#include "Input/InputModifier/BSKeyboardGamepadModifier.h"

void Input::BSKeyboardGamepadModifier::ApplyKeySuppressionModifier(InputFilter& filter) const
{
	BSControlModifier<RE::INPUT_DEVICE::kKeyboard>::ApplyKeySuppressionModifier(filter);
	BSControlModifier<RE::INPUT_DEVICE::kGamepad>::ApplyKeySuppressionModifier(filter);
}
