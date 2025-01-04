#pragma once
#include "ImGuiTabBarAbstract.h"

namespace ImGui
{
	class ImGuiTabBarItemRemovable :
		public ImGuiTabBarItem
	{
	public:
		virtual void Remove() = 0;
	};

	template <typename T, typename TPtr = std::unique_ptr<T>, typename It = std::vector<TPtr>::iterator>
	class ImGuiTabBar : public ImGuiTabBarAbstract<T, It>
	{
	public:
		ImGuiTabBar(const char* id) :
			ImGuiTabBarAbstract<T, It>(id, (ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll))
		{
		}

	public:
		void AddItem(TPtr&& tabItem)
		{
			items.push_back(std::move(tabItem));
		}

	protected:
		T* DrawTabBarItem(It& it) override
		{
			bool activeProp   = false;
			bool isNotRemoved = (*it)->DrawTabItem(activeProp);
			T*   currentTab = nullptr;
			if (isNotRemoved)
			{
				currentTab = (*it).get();
				it++;
			}
			else
			{
				(*it)->Remove();
				it = items.erase(it);
			}

			return ((activeProp && isNotRemoved) ? currentTab : nullptr);
		}

		It begin() { return items.begin(); }
		It end() { return items.end(); };

	protected:
		std::vector<TPtr> items;
	};
}
