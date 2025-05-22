#pragma once
#include "Input/InputFilter.h"
#include "Input/InputModifier/InputModifier.h"

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
		std::unordered_map<int, InputModifierPtr> modifiers;
	};
}
