#include "Input/InputModifier/CanMoveAround.h"

Input::CanMoveAround::CanMoveAround(bool blockMove) :
	BSControlModifier({ "Forward", "Back", "Strafe Left", "Strafe Right" }, blockMove)
{
}
