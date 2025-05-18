#pragma once

namespace ImGui
{
	template <typename T>
	class ImGuiSelector
	{
	public:
		ImGuiSelector(const char* selectionID) : label(selectionID){};
		ImGuiSelector(const char* selectionID, T current) : label(selectionID), selection(current)
		{}

	public:
		virtual bool DrawValueEditor() = 0;
		const T*     GetSelection() const
		{
			if (selection.has_value())
			{
				return &selection.value();
			}
			return nullptr;
		}

	protected:
		std::string      label;
		std::optional<T> selection;
	};
}
