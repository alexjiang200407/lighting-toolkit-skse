#pragma once
#include "ImGuiTabBarAbstract.h"
#include <type_traits>

namespace ImGui
{
	class ImGuiNavBarItem;
	typedef std::unique_ptr<ImGuiNavBarItem> ImGuiNavBarItemPtr;

	class ImGuiNavBarItem :
		public ImGuiTabBarItem
	{
	public:
		ImGuiNavBarItem(const char* label);
		bool DrawTabItem();
		const char* GetLabel() const;

	private:
		std::string label;
	};

	template <
		typename T,
		size_t SZ,
		typename TPtr = typename std::unique_ptr<T>,
		typename It   = typename std::array<TPtr, SZ>::iterator,
		typename      = typename std::enable_if_t<std::is_base_of<ImGuiNavBarItem, T>::value>>
	class ImGuiNavBar :
		private ImGuiTabBarAbstract<T, It>
	{
	public:
		ImGuiNavBar(const char* id, std::array<T*, SZ> nav) :
			ImGuiTabBarAbstract<T, It>(id, 0)
		{
			for (size_t i = 0; i < SZ; i++)
			{
				navBarItems[i] = std::unique_ptr<T>(nav[i]);
			}
		}

		bool DrawNavBar()
		{
			return ImGuiTabBarAbstract<T, It>::DrawTabBar();
		}

		using ImGuiTabBarAbstract<T, It>::GetLabel;


	protected:
		T* DrawTabBarItem(It& it) override
		{
			T* ret = nullptr;
			if ((*it)->DrawTabItem())
			{
				ret = (*it).get();
			}

			it++;
			return ret;
		}

		It begin() { return navBarItems.begin(); }
		It end() { return navBarItems.end(); }

		T* GetSelected() const
		{
			return ImGuiTabBarAbstract<T, It>::currentTab;
		}

	protected:
		std::array<TPtr, SZ> navBarItems;
	};

}
