#pragma once
#include "ImGuiTabBarAbstract.h"

namespace ImGui
{
	class ImGuiTabBarItemRemovable : public ImGuiTabBarItem
	{
	public:
		virtual bool DrawTabItem(bool& isActive) = 0;
		virtual void Remove()                    = 0;
	};

	template <
		typename T,
		typename TPtr = std::unique_ptr<T>,
		typename It   = std::vector<TPtr>::iterator,
		typename CIt  = std::vector<TPtr>::const_iterator>
	class ImGuiTabBar : public ImGuiTabBarAbstract<T, It>
	{
		ASSERT_BASE(T, ImGuiTabBarItemRemovable);

	public:
		typedef It  iterator;
		typedef CIt const_iterator;

	public:
		ImGuiTabBar(const char* id) :
			ImGuiTabBarAbstract<T, It>(
				id,
				(ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll))
		{}

	public:
		void AddItem(TPtr&& tabItem) { items.push_back(std::move(tabItem)); }

	protected:
		T* DrawTabBarItem(It& it) override
		{
			bool activeProp   = false;
			bool isNotRemoved = (*it)->DrawTabItem(activeProp);
			T*   currentTab   = nullptr;
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

		It  begin() override { return items.begin(); }
		CIt cbegin() const { return items.cbegin(); }
		It  end() override { return items.end(); };
		CIt cend() const { return items.cend(); }

	protected:
		std::vector<TPtr> items;
	};
}
