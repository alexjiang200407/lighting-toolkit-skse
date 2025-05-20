#pragma once

namespace ImGui
{
	class ImGuiRenderTarget
	{
	public:
		virtual void DoFrame() = 0;
		virtual bool ShouldSkip();
		virtual bool ShouldDrawCursor();
	};
}
