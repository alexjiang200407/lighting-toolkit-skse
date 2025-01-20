#include "Input/InputModifier/CanMoveAround.h"

Input::CanMoveAround::CanMoveAround(bool blockMove, int slot) :
	BSControlModifier({ "Forward", "Back", "Strafe Left", "Strafe Right" }, blockMove, slot)
{
}
