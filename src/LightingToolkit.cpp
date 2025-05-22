#include "LightingToolkit.h"
#include "ImGui/ImGuiInputAdapter.h"
#include "ImGui/ImGuiRenderer.h"
#include "LightingPreset.h"
#include "MenuState/MenuOpen.h"
#include "SKSE/SerializationControl.h"

LightingToolkit LightingToolkit::singleton;

void LightingToolkit::Init()
{
	ImGui::ImGuiRenderer::GetSingleton()->Init(
		Style(),
		"./Data/Interface/Fonts/Futura-Condensed-Normal.ttf",
		15.0f);
	ImGui::ImGuiInputAdapter::GetSingleton()->Init();
	ImGui::ImGuiRenderer::GetSingleton()->RegisterRenderTarget(&singleton);
	presetSerializationControl.Deserialize(config);
	SKSE::SerializationControl::GetSingleton()->RegisterSerializer(this);
}

void LightingToolkit::OnDataLoaded()
{
	RE::PlayerCharacter::GetSingleton()->AddEventSink(&sceneLighting);
}

void LightingToolkit::OnSavePostLoaded() { sceneLighting.OnSavePostLoaded(); }

void LightingToolkit::DoFrame()
{
	if (ImGui::ImGuiInputAdapter::IsKeyPressed("iOpenCloseMenuKey", false))
	{
		ToggleMenu();
	}

	if (!doProcess)
		return;

	if (auto newState = menuState->Transition(&inputCtx))
	{
		inputCtx.Update();
		menuState = std::move(newState);
	}

	if (firstRender && !ImGui::ImGuiRenderer::GetSingleton()->HasPreexistingIni())
	{
		const auto& io = ImGui::GetIO();
		ImGui::SetNextWindowSize({ io.DisplaySize.x * 0.35f, io.DisplaySize.y * 0.97f });
	}
	firstRender = false;
	menuState->DoFrame(this);

	sceneLighting.EndFrame();
}

LightingToolkit* LightingToolkit::GetSingleton() { return &singleton; }

void LightingToolkit::DrawMenu()
{
	static const char* labels[] = {
		"Scene Light",
		"Character Light",
		"Camera",
		"Environment",
	};
	static bool enabled[] = { true, true, true, true };
	bool        changedSelected;

	ImGui::Toolbar(
		"##ControlTabs",
		labels,
		enabled,
		sizeof(enabled) / sizeof(bool),
		reinterpret_cast<size_t*>(&currentTool),
		&changedSelected);

	switch (currentTool)
	{
	case Tool::kSceneLight:
		sceneLighting.DrawWindow(&config);
		break;
	case Tool::kCharacterLight:
		break;
	case Tool::kCamera:
		DrawCameraControlWindow();
		break;
	case Tool::kEnvironment:
		break;
	}
}

void LightingToolkit::Position()
{
	if (currentTool == Tool::kSceneLight)
	{
		sceneLighting.PositionLight();
	}
}

LightingToolkit::LightingToolkit() : sceneLighting(&config), currentTool(Tool::kSceneLight) {}

void LightingToolkit::ToggleMenu()
{
	doProcess = !doProcess;

	if (doProcess)
	{
		if (!CanOpenWindow())
		{
			RE::DebugNotification("Cannot open LightingToolkit menu at this time...", "UIMenuOK");
			doProcess = false;
			return;
		}

		RE::PlaySound("UIMenuOK");
		logger::info("Opening Menu...");
		inputCtx.MenuOpen();
		inputCtx.Update();
		menuState                  = std::make_unique<MenuOpen>(&inputCtx);
		previouslyInFreeCameraMode = RE::PlayerCamera::GetSingleton()->IsInFreeCameraMode();
		if (!previouslyInFreeCameraMode)
		{
			RE::PlayerCamera::GetSingleton()->ToggleFreeCameraMode(true);
			RE::ControlMap::GetSingleton()->PushInputContext(
				RE::ControlMap::InputContextID::kTFCMode);
		}

		RE::UI::GetSingleton()->ShowMenus(false);
		previouslyFreezeTime = RE::Main::GetSingleton()->freezeTime;
	}
	else
	{
		RE::PlaySound("UIMenuCancel");
		logger::info("Closing Menu...");
		menuState.reset();
		inputCtx.MenuClose();
		RE::UI::GetSingleton()->ShowMenus(true);
		RE::Main::GetSingleton()->freezeTime = previouslyFreezeTime;
		if (!previouslyInFreeCameraMode)
		{
			RE::PlayerCamera::GetSingleton()->ToggleFreeCameraMode(false);
			RE::ControlMap::GetSingleton()->PopInputContext(
				RE::ControlMap::InputContextID::kTFCMode);
		}
	}
}

bool LightingToolkit::CanOpenWindow()
{
	static constexpr std::array badMenus{ RE::MainMenu::MENU_NAME,
		                                  RE::MistMenu::MENU_NAME,
		                                  RE::LoadingMenu::MENU_NAME,
		                                  RE::FaderMenu::MENU_NAME,
		                                  "LootMenu"sv,
		                                  "CustomMenu"sv };

	if (const auto UI = RE::UI::GetSingleton();
	    !UI ||
	    std::ranges::any_of(
			badMenus,
			[&](const auto& menuName) { return UI->IsMenuOpen(menuName); }) ||
	    UI->IsItemMenuOpen())
	{
		return false;
	}

	const auto* controlMap = RE::ControlMap::GetSingleton();
	if (!controlMap)
	{
		return false;
	}

	auto context_id = controlMap->contextPriorityStack.back();
	if (context_id != RE::UserEvents::INPUT_CONTEXT_ID::kGameplay &&
	    context_id != RE::UserEvents::INPUT_CONTEXT_ID::kTFCMode &&
	    context_id != RE::UserEvents::INPUT_CONTEXT_ID::kConsole)
	{
		return false;
	}

	if (RE::MenuControls::GetSingleton()->InBeastForm() ||
	    RE::VATS::GetSingleton()->mode == RE::VATS::VATS_MODE::kKillCam)
	{
		return false;
	}

	return true;
}

bool LightingToolkit::ShouldDrawCursor() { return doProcess && menuState->ShouldDrawCursor(); }

float* LightingToolkit::GetCameraMoveSpeed()
{
	return REL::Relocation<float*>{ RELOCATION_ID(509808, 382522) }.get();
}

void LightingToolkit::DrawCameraControlWindow()
{
	if (ImGui::BeginPanel("Camera Settings###CameraControlWindow"))
	{
		ImGui::Checkbox("Freeze Time", &RE::Main::GetSingleton()->freezeTime);
		ImGui::SliderAutoFill("Camera Speed", GetCameraMoveSpeed(), 0.1f, 50.0f);
		ImGui::EndPanel();
	}
}

void LightingToolkit::SerializeItems(SKSE::CoSaveIO io) const { sceneLighting.SerializeItems(io); }

void LightingToolkit::DeserializeItems(SKSE::CoSaveIO io)
{
	sceneLighting.DeserializeItems(io, &config);
}

void LightingToolkit::Revert(SKSE::CoSaveIO io) { sceneLighting.Revert(io); }

constexpr uint32_t LightingToolkit::GetKey() { return serializationKey; }

ImGuiStyle LightingToolkit::Style()
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
