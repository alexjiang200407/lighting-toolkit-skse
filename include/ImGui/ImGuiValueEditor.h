#pragma once
#include "ImGuiNavBar.h"
#include <type_traits>

namespace ImGui
{
	template <typename V>
	class ImGuiValueEditorMode :
		public ImGuiNavBarItem
	{
	public:
		ImGuiValueEditorMode(const char* label) :
			ImGuiNavBarItem(label)
		{
		}

		operator std::unique_ptr<ImGuiValueEditorMode<V>>() const
		{
			return std::make_unique<ImGuiValueEditorMode<V>>(*this);
		}

		virtual bool     DrawValueEditor()    = 0;
		virtual std::optional<V> GetSelection() const = 0;
	};

	template <
		typename T,
		typename V,  // Value type that is being edited
		size_t SZ,
		typename = typename std::enable_if_t<std::is_base_of<ImGuiValueEditorMode<V>, T>::value>>
	class ImGuiValueEditor :
		private ImGuiNavBar<T, SZ>
	{
	public:
		ImGuiValueEditor(const char* label, std::array<T, SZ> elements) :
			ImGuiNavBar<T, SZ>(label, elements)
		{}

	public:
		bool DrawEditor()
		{
			ImGuiNavBar<T, SZ>::DrawNavBar();
			return ImGuiNavBar<T, SZ>::GetSelected()->DrawValueEditor();
		}

		std::optional<V> GetSelection() const
		{
			if (auto* selectedTab = ImGuiNavBar<T, SZ>::GetSelected())
				return selectedTab->GetSelection();

			return std::nullopt;
		}
	};
}
