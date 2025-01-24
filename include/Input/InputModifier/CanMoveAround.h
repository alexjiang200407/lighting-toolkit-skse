#pragma once
#include "BSKeyboardGamepadModifier.h"

namespace Input
{
	class CanMoveAround :
		public BSKeyboardGamepadModifier
	{
	public:
		CanMoveAround(bool blockMove);

	public:
		static inline constexpr const char* inputIDs[4] = { "Forward", "Back", "Strafe Left", "Strafe Right" };
	};
}
