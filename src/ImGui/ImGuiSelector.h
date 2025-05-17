#pragma once
#include "ImGui/ImGuiValueEditor.h"

namespace ImGui
{
	template <typename T>
	class ImGuiSelector : public ImGuiValueEditorMode<T>
	{
	public:
		ImGuiSelector(const char* selectionID) : ImGuiValueEditorMode<T>(selectionID){};
		ImGuiSelector(const char* selectionID, T current) :
			ImGuiValueEditorMode<T>(selectionID), selection(current)
		{}

	public:
		bool DrawValueEditor() override { return true; }
		T*   GetSelection() override
		{
			if (selection.has_value())
			{
				return &selection.value();
			}
			return nullptr;
		}

	protected:
		std::optional<T> selection;
	};
}
