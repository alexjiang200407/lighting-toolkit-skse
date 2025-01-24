#include "Input/InputContext.h"
#include "ImGui/ImGuiInputAdapter.h"

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
	adapter->EnableSupression(*this);
}

void Input::InputContext::TransformInputFilter(InputFilter& data) const
{
	data.Clear();
	for (const auto& [_, modifier] : modifiers)
	{
		modifier->ApplyModifier(data);
	}
}
