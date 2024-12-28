#pragma once

namespace ImGui
{
	class ImGuiComponent
	{
	public:
		virtual void DoFrame() = 0;
		virtual bool ShouldSkip();
	};
}
