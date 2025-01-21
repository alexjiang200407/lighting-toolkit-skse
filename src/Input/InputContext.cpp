#include "Input/InputContext.h"
#include "ImGui/ImGuiInputAdapter.h"

Input::InputContext::InputContext(InputFilter filter) :
	starting(filter)
{
}

void Input::InputContext::AddModifier(int slot, InputModifier* modifier)
{
	modifiers[slot] = std::move(InputModifierPtr(modifier));
}

void Input::InputContext::RemoveModifier(int slot)
{
	modifiers.erase(slot);
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
	for (const auto& [_, modifier] : modifiers)
	{
		data = modifier->ApplyModifier(data);
	}
	return data;
}
