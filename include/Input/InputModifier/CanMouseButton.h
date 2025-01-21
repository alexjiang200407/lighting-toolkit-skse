#pragma once
#include "InputModifier.h"

namespace Input
{
	class CanMouseButton :
		virtual public InputModifier
	{
	public:
		CanMouseButton(bool blockMouseButtons);

	public:
		MouseSupressionMask ApplyMouseKeyModifier(MouseSupressionMask mouse) const override;

	private:
		bool blockMouseButtons;
	};
}
