#pragma once
#include "../InputFilter.h"

namespace Input
{
	class InputModifier;
	typedef std::unique_ptr<InputModifier> InputModifierPtr;

	class InputModifier
	{
	public:
		virtual ~InputModifier() = default;

	public:
		virtual bool ApplyBlockCharModifier(bool prevBlockChars) const;
		virtual bool ApplyBlockMouseMoveModifier(bool prevBlockMouseMove) const;
		virtual void ApplyKeySuppressionModifier(InputFilter& filter) const;
		virtual void ApplyModifier(InputFilter& filter) const;
	};
}
