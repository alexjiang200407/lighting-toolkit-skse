#pragma once
#include "ImGuiComponent.h"
#include <fmt/compile.h>

namespace ImGui
{
	class ImGuiRenderer
	{
	private:
		struct WndProc
		{
			static LRESULT        thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static inline WNDPROC func;
		};

		struct CreateD3DAndSwapChain
		{
			static void                                    thunk();
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct StopTimer
		{
			static void                                    thunk(std::uint32_t timer);
			static inline REL::Relocation<decltype(thunk)> func;
		};

	private:
		ImGuiRenderer()                                = default;
		ImGuiRenderer(const ImGuiRenderer&)            = delete;
		ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;
		ImGuiRenderer(ImGuiRenderer&&)                 = delete;
		ImGuiRenderer& operator=(ImGuiRenderer&&)      = delete;

	public:
		void                  Init(ImGuiStyle style);
		void                  RegisterRenderTarget(ImGuiComponent* target);
		void                  UnregisterRenderTarget(ImGuiComponent* target);
		static ImGuiRenderer* GetSingleton();

	private:
		ImGuiStyle                style;
		std::set<ImGuiComponent*> targets;
		std::atomic<bool>         installedHooks;
		std::string               iniFile = fmt::format("{}ImGui.ini", Version::PROJECT);
		static ImGuiRenderer      singleton;
	};
}
