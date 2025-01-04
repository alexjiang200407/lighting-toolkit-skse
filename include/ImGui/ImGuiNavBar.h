#pragma once
#include "ImGuiTabBarAbstract.h"

namespace ImGui
{
	class ImGuiNavBarItem;
	typedef std::unique_ptr<ImGuiNavBarItem> ImGuiNavBarItemPtr;

	class ImGuiNavBarItem
	{
	public:
		ImGuiNavBarItem(const char* label);
		bool DrawTabItem();
		operator ImGuiNavBarItemPtr() const; 
	private:
		std::string label;
	};


	template <size_t SZ, typename TPtr = std::unique_ptr<ImGuiNavBarItem>, typename It = std::array<TPtr, SZ>::iterator>
	class ImGuiNavBar :
		private ImGuiTabBarAbstract<ImGuiNavBarItem, It>
	{
	public:
		ImGuiNavBar(const char* id, std::array<const char*, SZ> nav) :
			ImGuiTabBarAbstract<ImGuiNavBarItem, It>(id, 0)
		{
			for (size_t i = 0; i < SZ; i++)
			{
				navBarItems[i] = ImGuiNavBarItem(nav[i]);
			}
		}

		void DrawNavBar()
		{
			ImGuiTabBarAbstract<ImGuiNavBarItem, It>::DrawTabBar();
		}

	protected:
		ImGuiNavBarItem* DrawTabBarItem(It& it) override
		{
			ImGuiNavBarItem* ret = nullptr;
			if ((*it)->DrawTabItem())
			{
				ret = (*it).get();
			}

			it++;
			return ret;
		}

		It begin() { return navBarItems.begin(); }
		It end() { return navBarItems.end(); }

	private:
		std::array<TPtr, SZ> navBarItems;
	};

}
