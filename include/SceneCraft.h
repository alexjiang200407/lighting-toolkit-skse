#pragma once
#include "ImGui/ImGuiComponent.h"
#include "Prop.h"

class SceneCraft :
	ImGui::ImGuiComponent,
	public RE::BSTEventSink<RE::BGSActorCellEvent>
{
public:
	void               Init();
	void               OnDataLoaded();
	void               DoFrame();
	ImGuiStyle         Style();
	static SceneCraft* GetSingleton();

private:
	SceneCraft() = default;

private:
	void                     ToggleMenu();
	bool                     CanOpenWindow();
	bool                     ShouldDrawCursor() override;
	static float*            GetCameraMoveSpeed();
	void                     SuppressDXInput();
	void                     UpdateLookingAround();
	int                      DrawTabBar();
	void                     DrawPropControlWindow(int activePropIndex);
	void                     DrawCameraControlWindow();
	RE::BSEventNotifyControl ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>* a_eventSource) override;

private:
	bool                               lookingAround              = false;
	bool                               previouslyInFreeCameraMode = false;
	bool                               previouslyFreezeTime       = false;
	bool                               showWindow                 = false;
	std::vector<std::unique_ptr<Prop>> props;
	static SceneCraft                  singleton;
	static constexpr ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
};
