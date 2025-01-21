#pragma once
#include "InputFilter.h"
#include "InputModifier/InputModifier.h"

namespace Input
{
	class InputContext
	{
	public:
		InputContext(InputFilter filter);

	public:
		void AddModifier(int slot, InputModifier* modifier);
		void RemoveModifier(int slot);
		void ClearAllModifiers();
		void Update();

	private:
		InputFilter AggregateModifiers(InputFilter data) const;

	private:
		InputFilter                     starting;
		std::map<int, InputModifierPtr> modifiers;
	};
}
