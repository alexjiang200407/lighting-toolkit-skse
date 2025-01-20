#include "Input/InputModifier/CanMouseMove.h"

Input::CanMouseMove::CanMouseMove(bool blockLookAround, int slot) :
	InputModifier(slot), blockLookAround(blockLookAround)
{
}

bool Input::CanMouseMove::ApplyBlockMouseMoveModifier(bool) const
{
	return blockLookAround;
}
