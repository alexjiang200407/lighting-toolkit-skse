#pragma once
#include "BSControlModifier.h"

namespace Input
{
	class CanMoveAround :
		public BSControlModifier
	{
	public:
		CanMoveAround(bool blockMove, int slot);
	};
}
