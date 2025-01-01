#include "SceneCraft.h"
#include "ImGui/ImGuiInputAdapter.h"
#include "ImGui/ImGuiRenderer.h"
#include "Lighting.h"
#include "LightingTemplate.h"
#include "Palette.h"

SceneCraft SceneCraft::singleton;

void SceneCraft::Init()
{
	ImGui::ImGuiRenderer::GetSingleton()->Init(Style());
	ImGui::ImGuiInputAdapter::GetSingleton()->Init();
	ImGui::ImGuiRenderer::GetSingleton()->RegisterRenderTarget(&singleton);
	Palette::LoadPaletteFile();
	LightingTemplate::LoadLightingTemplates();
}

void SceneCraft::OnDataLoaded()
{
	RE::PlayerCharacter::GetSingleton()->AddEventSink(this);
}

void SceneCraft::DoFrame()
{
	if (ImGui::IsKeyPressed(ImGuiKey_End, false))
	{
		ToggleMenu();
	}

	if (!showWindow)
		return;

	if (ImGui::IsKeyPressed(ImGuiKey_F, false))
		RE::Main::GetSingleton()->freezeTime = !RE::Main::GetSingleton()->freezeTime;
	if (lookingAround != (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftAlt)))
	{
		lookingAround = !lookingAround;
		UpdateLookingAround();
	}
	ImGui::Begin("##SCMain", nullptr, windowFlags);
	{
		ImGui::BeginDisabled(lookingAround);
		{
			int activePropIndex = DrawTabBar();
			DrawPropControlWindow(activePropIndex);
			DrawCameraControlWindow();
		}
		ImGui::EndDisabled();
	}
	ImGui::End();
}

SceneCraft* SceneCraft::GetSingleton()
{
	return &singleton;
}

void SceneCraft::ToggleMenu()
{
	showWindow = !showWindow;

	if (showWindow)
	{
		SuppressDXInput();
		previouslyInFreeCameraMode = RE::PlayerCamera::GetSingleton()->IsInFreeCameraMode();
		if (!previouslyInFreeCameraMode)
		{
			RE::PlayerCamera::GetSingleton()->ToggleFreeCameraMode(false);
			RE::ControlMap::GetSingleton()->PushInputContext(RE::ControlMap::InputContextID::kTFCMode);
		}

		previouslyFreezeTime = RE::Main::GetSingleton()->freezeTime;
	}
	else
	{
		ImGui::ImGuiInputAdapter::GetSingleton()->DisableSupression();
		RE::Main::GetSingleton()->freezeTime = previouslyFreezeTime;
		if (!previouslyInFreeCameraMode)
		{
			RE::PlayerCamera::GetSingleton()->ToggleFreeCameraMode(false);
			RE::ControlMap::GetSingleton()->PopInputContext(RE::ControlMap::InputContextID::kTFCMode);
		}
	}
}

bool SceneCraft::ShouldDrawCursor()
{
	return showWindow && !lookingAround;
}

float* SceneCraft::GetCameraMoveSpeed()
{
	return REL::Relocation<float*>{ RELOCATION_ID(509808, 382522) }.get();
}

void SceneCraft::SuppressDXInput()
{
	using INPUT_CONTEXT = RE::UserEvents::INPUT_CONTEXT_IDS;
	ImGui::ImGuiInputAdapter::KeyboardSupressionMask kbd;
	ImGui::ImGuiInputAdapter::MouseSupressionMask    mouse;
	ImGui::ImGuiInputAdapter::GamePadSupressionMask  gamepad;

	const auto* controlMap = RE::ControlMap::GetSingleton();
	kbd.set();

	kbd.flip(controlMap->GetMappedKey("Forward", RE::INPUT_DEVICE::kKeyboard, INPUT_CONTEXT::kGameplay));
	kbd.flip(controlMap->GetMappedKey("Back", RE::INPUT_DEVICE::kKeyboard, INPUT_CONTEXT::kGameplay));
	kbd.flip(controlMap->GetMappedKey("Strafe Left", RE::INPUT_DEVICE::kKeyboard, INPUT_CONTEXT::kGameplay));
	kbd.flip(controlMap->GetMappedKey("Strafe Right", RE::INPUT_DEVICE::kKeyboard, INPUT_CONTEXT::kGameplay));

	mouse.set();

	gamepad.set();
	gamepad.flip(controlMap->GetMappedKey("Forward", RE::INPUT_DEVICE::kGamepad, INPUT_CONTEXT::kGameplay));
	gamepad.flip(controlMap->GetMappedKey("Back", RE::INPUT_DEVICE::kGamepad, INPUT_CONTEXT::kGameplay));
	gamepad.flip(controlMap->GetMappedKey("Strafe Left", RE::INPUT_DEVICE::kGamepad, INPUT_CONTEXT::kGameplay));
	gamepad.flip(controlMap->GetMappedKey("Strafe Right", RE::INPUT_DEVICE::kGamepad, INPUT_CONTEXT::kGameplay));

	ImGui::ImGuiInputAdapter::GetSingleton()->EnableSupression(kbd, mouse, gamepad, true, true);
}

void SceneCraft::UpdateLookingAround()
{
	if (lookingAround)
	{
		ImGui::ImGuiInputAdapter::GetSingleton()->SetSuppressMouse(0);
		ImGui::ImGuiInputAdapter::GetSingleton()->SetSuppressMouseMove(false);
		return;
	}

	ImGui::ImGuiInputAdapter::MouseSupressionMask mouse;
	mouse.set();

	ImGui::ImGuiInputAdapter::GetSingleton()->SetSuppressMouse(mouse);
	ImGui::ImGuiInputAdapter::GetSingleton()->SetSuppressMouseMove(true);
}

int SceneCraft::DrawTabBar()
{
	int activePropIndex = -1;
	ImGui::BeginTabBar("##propstabbar", ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll);
	{
		int i = 0;
		for (auto prop = props.begin(); prop != props.end();)
		{
			bool activeProp = false;
			if (!(*prop)->DrawTabItem(activeProp))
			{
				(*prop)->Remove();
				prop = props.erase(props.begin() + i);
				continue;
			}

			prop++;

			if (activeProp)
				activePropIndex = i;

			i++;
		}

		if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
		{
			// TODO Add a PropFactory creation method here
			const auto       dataHandler = RE::TESDataHandler::GetSingleton();
			const RE::FormID id          = dataHandler->LookupFormID(0x801, "SceneCraft.esp");
			const auto       ref         = RE::PlayerCharacter::GetSingleton()->PlaceObjectAtMe(RE::TESForm::LookupByID(id)->As<RE::TESBoundObject>(), true);

			props.push_back(std::make_unique<Lighting>(Lighting(ref, 0, 0)));
			(*props.rbegin())->MoveToCameraLookingAt(50.0f);
		}
	}
	ImGui::EndTabBar();
	return activePropIndex;
}

void SceneCraft::DrawPropControlWindow(int activePropIndex)
{
	ImGui::BeginChild("###PropControlWindow", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
	{
		ImGui::Text("Current Prop:");
		if (activePropIndex != -1)
		{
			if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
				props[activePropIndex]->MoveToCameraLookingAt(50.0f);
			props[activePropIndex]->DrawControlPanel();
		}
	}
	ImGui::EndChild();
}

void SceneCraft::DrawCameraControlWindow()
{
	ImGui::BeginChild("###CameraControlWindow", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
	{
		ImGui::Text("Camera Settings:");
		ImGui::Checkbox("Freeze Time", &RE::Main::GetSingleton()->freezeTime);
		ImGui::SliderAutoFill("Camera Speed", GetCameraMoveSpeed(), 0.1f, 50.0f);
	}
	ImGui::EndChild();
}

RE::BSEventNotifyControl SceneCraft::ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>*)
{
	if (!a_event || a_event->flags == RE::BGSActorCellEvent::CellFlag::kLeave)
	{
		return RE::BSEventNotifyControl::kContinue;
	}

	auto cell = RE::TESForm::LookupByID<RE::TESObjectCELL>(a_event->cellID);
	if (!cell)
	{
		return RE::BSEventNotifyControl::kContinue;
	}

	for (const auto& prop : props)
	{
		if (prop->GetCellID() == a_event->cellID)
		{
			prop->OnEnterCell();
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}

ImGuiStyle SceneCraft::Style()
{
	ImGuiStyle style;

	style.WindowPadding     = ImVec2(15, 15);
	style.WindowRounding    = 5.0f;
	style.ChildRounding     = 5.0f;
	style.FramePadding      = ImVec2(5, 5);
	style.FrameRounding     = 4.0f;
	style.ItemSpacing       = ImVec2(12, 8);
	style.ItemInnerSpacing  = ImVec2(8, 6);
	style.IndentSpacing     = 25.0f;
	style.ScrollbarSize     = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize       = 5.0f;
	style.GrabRounding      = 3.0f;
	style.DisabledAlpha     = 0.3f;

	style.Colors[ImGuiCol_Text]                 = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_Border]               = ImVec4(0.80f, 0.80f, 0.83f, 0.35f);
	style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Button]               = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Header]               = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);

	return style;
}
