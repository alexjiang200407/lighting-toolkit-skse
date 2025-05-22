#include "Input/InputModifier/CanCharInput.h"

Input::CanCharInput::CanCharInput(bool blockCharModifer) : blockCharModifer(blockCharModifer) {}

bool Input::CanCharInput::ApplyBlockCharModifier(bool) const { return blockCharModifer; }
