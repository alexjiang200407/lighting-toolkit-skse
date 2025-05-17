#include "Input/InputModifier/InputModifier.h"

bool Input::InputModifier::ApplyBlockCharModifier(bool prevBlockChars) const
{
	return prevBlockChars;
}

bool Input::InputModifier::ApplyBlockMouseMoveModifier(bool prevBlockMouseMove) const
{
	return prevBlockMouseMove;
}

void Input::InputModifier::ApplyKeySuppressionModifier(InputFilter&) const {}

void Input::InputModifier::ApplyModifier(InputFilter& filter) const
{
	filter.SetBlockingChar(ApplyBlockCharModifier(filter.IsBlockingChar()));
	filter.SetBlockingMouseMove(ApplyBlockMouseMoveModifier(filter.IsBlockingMouseMove()));

	ApplyKeySuppressionModifier(filter);
}
