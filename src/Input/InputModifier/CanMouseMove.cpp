#include "../InputModifier/CanMouseMove.h"

Input::CanMouseMove::CanMouseMove(bool blockLookAround) : blockLookAround(blockLookAround) {}

bool Input::CanMouseMove::ApplyBlockMouseMoveModifier(bool) const { return blockLookAround; }
