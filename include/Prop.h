#pragma once

class Prop
{
public:
	Prop(RE::TESObjectREFRPtr ref);

public:
	virtual bool DrawTabItem(bool& active);
	virtual void DrawControlPanel();
	void         Remove();
	void         Hide();
	void         MoveToCameraLookingAt(float distanceFromCamera);

private:
	static RE::NiPoint3 GetCameraPosition();

private:
	RE::TESObjectREFRPtr ref;
};
