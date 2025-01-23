#include "Input/InputModifier/CanMouseButton.h"

Input::CanMouseButton::CanMouseButton(bool blockMouseButtons) :
	BSControlModifier(inputIDs, blockMouseButtons)
{
}
