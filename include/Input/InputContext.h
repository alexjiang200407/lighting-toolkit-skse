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
		void ClearAllModifiers();
		void Update();

	//private:
	//	struct InputModifierComparator
	//	{
	//		using is_transparent = void;
	//		bool operator()(const InputModifierPtr& lhs, const InputModifierPtr& rhs) const;
	//		bool operator()(const InputModifierPtr& lhs, int rhs) const;
	//		bool operator()(int slot, const InputModifierPtr& modifier) const;
	//	};

	private:
		InputFilter AggregateModifiers(InputFilter data) const;

	private:
		InputFilter                                              starting;
		std::map<int, InputModifierPtr> modifiers;
	};
}
