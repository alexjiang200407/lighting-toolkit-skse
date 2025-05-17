#pragma once

namespace ImGui
{
	typedef uuids::uuid TabID;

	class ImGuiTabBarItem
	{
	public:
		bool  operator==(const ImGuiTabBarItem& rhs) const;
		TabID GetTabID() const;
		void  SetActive();

	protected:
		bool setActive = false;

	private:
		TabID id = uuids::uuid_system_generator{}();
	};

	template <class T, typename It>
	class ImGuiTabBarAbstract
	{
		ASSERT_BASE(T, ImGuiTabBarItem);

	public:
		ImGuiTabBarAbstract(const char* id, ImGuiTabBarFlags flags) : tabBarID(id), flags(flags) {}

	public:
		bool DrawTabBar()
		{
			bool changed = false;
			ImGui::BeginTabBar(tabBarID.c_str(), flags);
			{
				changed = DrawTabBarItems();
			}
			ImGui::EndTabBar();
			return changed;
		}

		const char* GetLabel() const { return tabBarID.c_str(); }

	protected:
		virtual bool DrawTabBarItems()
		{
			bool changed = false;
			currentTab   = nullptr;

			for (auto it = begin(); it != end();)
			{
				T* newTab = DrawTabBarItem(it);
				if (newTab)
				{
					currentTab = newTab;
					if (lastID != newTab->GetTabID())
					{
						lastID  = currentTab->GetTabID();
						changed = true;
					}
				}
			}
			return changed;
		}

		virtual T* DrawTabBarItem(It& it) = 0;
		virtual It begin()                = 0;
		virtual It end()                  = 0;

	protected:
		T* currentTab = nullptr;

	private:
		uuids::uuid            lastID;
		const ImGuiTabBarFlags flags;
		std::string            tabBarID;
	};
}
