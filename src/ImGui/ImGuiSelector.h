#pragma once

namespace ImGui
{
	template <typename T>
	class ImGuiSelector
	{
	public:
		ImGuiSelector(const char* selectionID, std::optional<T> current = std::nullopt) : label(selectionID), selection(current)
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
