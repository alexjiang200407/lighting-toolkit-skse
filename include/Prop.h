#pragma once
#include "ImGui/ImGuiTabBar.h"
#include "Preset/PresetDatabase.h"

class Prop :
	public ImGui::ImGuiTabBarItemRemovable
{
public:
	Prop(RE::TESObjectREFRPtr ref);

public:
	virtual bool            DrawTabItem(bool& active) override;
	virtual void            DrawControlPanel();
	virtual void            Remove() override;
	virtual void            MoveToCameraLookingAt(float distanceFromCamera);
	virtual void            MoveTo(RE::NiPoint3 newPos);
	virtual void            Hide();
	bool                    isHidden() const;
	void                    MoveToCurrentPosition();
	virtual void            Show();
	virtual void            OnEnterCell();
	RE::FormID              GetCellID();
	virtual void            Rotate(float delta);
	static RE::NiPoint3     GetCameraLookingAt(float distanceFromCamera);
	virtual RE::BSFadeNode* Attach3D();
	virtual void            Init3D();

private:
	static RE::NiPoint3 GetCameraPosition();

protected:
	RE::TESObjectREFRPtr ref;

private:
	bool         hidden = false;
	RE::NiPoint3 worldTranslate;
};
