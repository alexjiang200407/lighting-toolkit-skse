#pragma once
#include "LightSelector.h"
#include "MCM/Settings.h"
#include "Preset/PresetDatabase.h"
#include "Preset/PresetSerializationControl.h"
#include "SKSE/SerializableCollection.h"
#include "SceneLight.h"

class SceneLighting : public RE::BSTEventSink<RE::BGSActorCellEvent>
{
public:
	SceneLighting(preset::PresetDatabase* config) : lightSelector(config){};

public:
	void SerializeItems(SKSE::CoSaveIO io) const;
	void DeserializeItems(SKSE::CoSaveIO io, preset::PresetDatabase* config);
	void Revert(SKSE::CoSaveIO io);
	void DrawWindow(preset::PresetDatabase* config);
	void PositionLight();
	void OnSavePostLoaded();
	void EndFrame();

	RE::BSEventNotifyControl ProcessEvent(
		const RE::BGSActorCellEvent*               a_event,
		RE::BSTEventSource<RE::BGSActorCellEvent>* a_eventSource) override;

private:
	void        DrawTabBar();
	void        DrawPropControlWindow();
	void        DrawSceneControlWindow(preset::PresetDatabase* config);
	SceneLight* GetCurrentLight();

private:
	std::vector<SceneLight>   lights;
	LightSelector             lightSelector;
	std::optional<size_t>     currentTab;
	std::optional<RE::FormID> currentFormID;
};
