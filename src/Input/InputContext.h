#pragma once
#include "InputFilter.h"
#include "InputModifier/InputModifier.h"

namespace Input
{
	class InputContext
	{
	public:
		void AddModifier(int slot, InputModifier* modifier);
		void RemoveModifier(int slot);
		void ClearAllModifiers();
		void Update();
		void TransformInputFilter(InputFilter& data) const;

	private:
		std::map<int, InputModifierPtr> modifiers;
	};
}
