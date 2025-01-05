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

		virtual bool DrawValueEditor() = 0;
		virtual V*   GetSelection()    = 0;
	};

	template <
		typename T,
		typename V,  // Value type that is being edited
		size_t SZ>
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

		V* GetSelection()
		{
			if (auto* selectedTab = ImGuiNavBar<T, SZ>::GetSelected())
				return selectedTab->GetSelection();

			return nullptr;
		}
	};
}
