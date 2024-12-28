#pragma once
#include "ImGuiComponent.h"
#include <fmt/compile.h>

namespace ImGui
{
	class ImGuiRenderer
	{
	private:
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

	public:
		ImGuiRenderer()                                = default;
		ImGuiRenderer(const ImGuiRenderer&)            = delete;
		ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;
		ImGuiRenderer(ImGuiRenderer&&)                 = delete;
		ImGuiRenderer& operator=(ImGuiRenderer&&)      = delete;

	public:
		static void Init();
		void        RegisterRenderTarget(ImGuiComponent* target);
		void        UnregisterRenderTarget(ImGuiComponent* target);

	private:
		static std::set<ImGuiComponent*> targets;
		static inline std::atomic<bool>  installedHooks;
		static const inline std::string  iniFile = fmt::format("{}ImGui.ini", Version::PROJECT);
	};
}
