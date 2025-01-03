#pragma once
#include "Preset/PresetDatabase.h"

class Prop
{
public:
	Prop(RE::TESObjectREFRPtr ref);

public:
	virtual bool        DrawTabItem(bool& active);
	virtual void        DrawControlPanel(preset::PresetDatabase& config);
	virtual void        Remove();
	virtual void        MoveToCameraLookingAt(float distanceFromCamera);
	virtual void        Hide();
	virtual void        OnEnterCell();
	RE::FormID          GetCellID();
	static RE::NiPoint3 GetCameraLookingAt(float distanceFromCamera);

private:
	static RE::NiPoint3 GetCameraPosition();

protected:
	RE::TESObjectREFRPtr ref;
};
