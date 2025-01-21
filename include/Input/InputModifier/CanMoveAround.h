#pragma once
#include "BSControlModifier.h"

namespace Input
{
	class CanMoveAround :
		virtual public BSControlModifier
	{
	public:
		CanMoveAround(bool blockMove);
	};
}
