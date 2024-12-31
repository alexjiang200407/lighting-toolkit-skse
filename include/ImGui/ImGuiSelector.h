#pragma once

namespace ImGui
{
	template <typename T>
	class ImGuiSelector
	{
	public:
		ImGuiSelector(int idx) :
			selectionIdx(idx)
		{
		}

	public:
		virtual const char* GetSelectionID() = 0;
		int                 DrawSelectionComboBox();
		virtual const char* SelectionName(int idx) const = 0;
		virtual int         GetSelectionCount() const    = 0;
		virtual const T&    GetCurrentSelection() const  = 0;

	protected:
		int selectionIdx;
	};

	template <typename T>
	inline int ImGuiSelector<T>::DrawSelectionComboBox()
	{
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(GetSelectionID()).x - ImGui::GetStyle().ItemSpacing.x);

		int old = selectionIdx;
		ImGui::Combo(
			GetSelectionID(),
			&selectionIdx,
			[](void* a, int idx, const char** out_pcstr) { 
			*out_pcstr = reinterpret_cast<ImGuiSelector<T>*>(a)->SelectionName(idx);
			return true; },
			this,
			GetSelectionCount());

		ImGui::PopItemWidth();

		return selectionIdx == old ? -1 : selectionIdx;
	}
}
