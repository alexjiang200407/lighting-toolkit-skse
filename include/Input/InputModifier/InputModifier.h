#pragma once
#include "Input/InputFilter.h"

namespace Input
{
	class InputModifier;
	typedef std::unique_ptr<InputModifier> InputModifierPtr;

	class InputModifier
	{
	public:
		InputModifier(int slot);

	public:
		virtual KeyboardSupressionMask ApplyKeyModifier(KeyboardSupressionMask kbd) const;
		virtual MouseSupressionMask    ApplyMouseKeyModifier(MouseSupressionMask mouse) const;
		virtual GamePadSupressionMask  ApplyGamepadKeyModifier(GamePadSupressionMask gamepad) const;
		virtual bool                   ApplyBlockCharModifier(bool prevBlockChars) const;
		virtual bool                   ApplyBlockMouseMoveModifier(bool prevBlockMouseMove) const;
		virtual InputFilter            ApplyModifier(InputFilter filter) const;
		bool                           operator<(const InputModifier& rhs) const;
		int                            GetSlot() const;
		operator InputModifierPtr();
	private:
		int slot = 0;
	};
}
