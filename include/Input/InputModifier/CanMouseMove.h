#pragma once
#include "InputModifier.h"

namespace Input
{
	class CanMouseMove :
		public InputModifier
	{
	public:
		CanMouseMove(bool blockLookAround);
	public:
		bool ApplyBlockMouseMoveModifier(bool prevBlockMouseMove) const override;

	private:
		bool blockLookAround;
	};
}
