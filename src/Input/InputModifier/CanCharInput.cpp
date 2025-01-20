#include "Input/InputModifier/CanCharInput.h"

Input::CanCharInput::CanCharInput(bool blockCharModifer, int slot) :
	InputModifier(slot), blockCharModifer(blockCharModifer)
{
}

bool Input::CanCharInput::ApplyBlockCharModifier(bool) const
{
	return blockCharModifer;
}
