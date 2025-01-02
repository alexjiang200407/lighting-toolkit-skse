#pragma once
#include "PresetDatabase.h"

namespace ImGui
{
	template <typename T, uint16_t CTID>
	class ImGuiSelector
	{
	public:
		ImGuiSelector(int idx) :
			selectionIdx(idx)
		{
		}

	public:
		virtual const char* GetSelectionID() = 0;
		int                 DrawSelectionComboBox(const PresetDatabase& config);
		virtual const char* SelectionName(int idx) const = 0;
		virtual int         GetSelectionCount() const    = 0;
		virtual const T&    GetCurrentSelection() const  = 0;

	protected:
		int selectionIdx;
	};

}
