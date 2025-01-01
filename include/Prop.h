#pragma once

class Prop
{
public:
	Prop(RE::TESObjectREFRPtr ref);

public:
	virtual bool        DrawTabItem(bool& active);
	virtual void        DrawControlPanel();
	virtual void        Remove();
	virtual void        MoveToCameraLookingAt(float distanceFromCamera);
	virtual void        Hide();
	static RE::NiPoint3 GetCameraLookingAt(float distanceFromCamera);

private:
	static RE::NiPoint3 GetCameraPosition();

protected:
	RE::TESObjectREFRPtr ref;
};
