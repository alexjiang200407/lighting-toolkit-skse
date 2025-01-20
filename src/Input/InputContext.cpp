#include "Input/InputContext.h"
#include "ImGui/ImGuiInputAdapter.h"

Input::InputContext::InputContext(InputFilter filter) :
	starting(filter)
{
}

void Input::InputContext::AddModifier(InputModifier* modifier)
{
	modifiers.erase(modifier->GetSlot());
	modifiers.insert(std::move(InputModifierPtr(modifier)));
}

void Input::InputContext::ClearAllModifiers()
{
	modifiers.clear();
}

void Input::InputContext::Update()
{
	auto* adapter = ImGui::ImGuiInputAdapter::GetSingleton();
	adapter->EnableSupression(AggregateModifiers(starting));
}

Input::InputFilter Input::InputContext::AggregateModifiers(InputFilter data) const
{
	for (const auto& modifier : modifiers)
	{
		data = modifier->ApplyModifier(data);
	}
	return data;
}

bool Input::InputContext::InputModifierComparator::operator()(const InputModifierPtr& lhs, const InputModifierPtr& rhs) const
{
	return *lhs < *rhs;
}

bool Input::InputContext::InputModifierComparator::operator()(const InputModifierPtr& lhs, int rhs) const
{
	return lhs->GetSlot() < rhs;
}

bool Input::InputContext::InputModifierComparator::operator()(int lhs, const InputModifierPtr& rhs) const
{
	return lhs < rhs->GetSlot();
}
