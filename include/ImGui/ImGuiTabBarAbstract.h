#pragma once

namespace ImGui
{
	template <typename T, typename It>
	class ImGuiTabBarAbstract
	{
	public:
		ImGuiTabBarAbstract(const char* id, ImGuiTabBarFlags flags) :
			tabBarID(id), flags(flags)
		{
		}

	public:
		void DrawTabBar()
		{
			ImGui::BeginTabBar(tabBarID.c_str(), flags);
			{
				DrawTabBarItems();
			}
			ImGui::EndTabBar();
		}

		const char* GetLabel() const
		{
			return tabBarID.c_str();
		}

	protected:
		virtual void DrawTabBarItems()
		{
			currentTab = nullptr;
			for (auto it = begin(); it != end();)
			{
				T* newTab = DrawTabBarItem(it);
				if (newTab)
					currentTab = newTab;
			}
		}

		virtual T* DrawTabBarItem(It& it) = 0;
		virtual It begin()                = 0;
		virtual It end()                  = 0;

	protected:
		T* currentTab = nullptr;

	private:
		const ImGuiTabBarFlags flags;
		std::string            tabBarID;
	};
}
