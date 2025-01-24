#pragma once
#include "BSKeyboardGamepadModifier.h"

namespace Input
{
	class CanRun :
		public BSKeyboardGamepadModifier
	{
	public:
		CanRun(bool blockRun);

	public:
		static inline constexpr const char* inputIDs[1] = { "Run" };
	};
}
