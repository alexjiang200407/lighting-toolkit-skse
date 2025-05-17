#include "LightingToolkit.h"
#include "ImGui/ImGuiInputAdapter.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiTabBar.h"
#include "Lighting.h"
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

void LightingToolkit::OnDataLoaded() { RE::PlayerCharacter::GetSingleton()->AddEventSink(this); }

void LightingToolkit::OnSavePostLoaded()
{
	for (const auto& light : items)
	{
		light->Init3D();
	}
}

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

	// Reset currentTab and wait for next draw cycle
	currentTab = nullptr;
}

LightingToolkit* LightingToolkit::GetSingleton() { return &singleton; }

LightingToolkit::LightingToolkit() : ImGuiTabBar<Lighting>("##propstabbar") {}

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

void LightingToolkit::DrawPropControlWindow()
{
	ImGui::PushID("###PropControlWindow");
	if (currentTab)
	{
		if (ImGui::ImGuiInputAdapter::IsKeyDown("iRotateLeftKey"))
			currentTab->Rotate(-0.1f);
		if (ImGui::ImGuiInputAdapter::IsKeyDown("iRotateRightKey"))
			currentTab->Rotate(0.1f);

		ImGui::BeginChild(
			"##PropControlPanel",
			ImVec2(0, 0),
			ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
		{
			currentTab->DrawControlPanel();
		}
		ImGui::EndChild();
	}
	ImGui::PopID();
}

void LightingToolkit::DrawCameraControlWindow()
{
	ImGui::BeginChild(
		"###CameraControlWindow",
		ImVec2(0, 0),
		ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
	{
		ImGui::Text("Camera Settings:");
		ImGui::Checkbox("Freeze Time", &RE::Main::GetSingleton()->freezeTime);
		ImGui::SliderAutoFill("Camera Speed", GetCameraMoveSpeed(), 0.1f, 50.0f);
	}
	ImGui::EndChild();
}

void LightingToolkit::DrawSceneControlWindow()
{
	ImGui::BeginChild(
		"###SceneControlWindow",
		ImVec2(0, 0),
		ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
	{
		ImGui::Text("Scene Settings:");
		if (ImGui::Button("Save Presets"))
		{
			presetSerializationControl.Serialize(config);
		}

		lightSelector.DrawValueEditor();
		if (ImGui::Button("Add Light"))
		{
			const auto       dataHandler = RE::TESDataHandler::GetSingleton();
			const RE::FormID id = dataHandler->LookupFormID(0x801, "InGameLightingToolkit.esp");

			if (auto* boundObj = RE::TESForm::LookupByID(id)->As<RE::TESBoundObject>())
			{
				const auto ref = dataHandler
				                     ->CreateReferenceAtLocation(
										 boundObj,
										 Lighting::GetCameraLookingAt(50.0f),
										 RE::NiPoint3(),
										 RE::PlayerCharacter::GetSingleton()->GetParentCell(),
										 RE::PlayerCharacter::GetSingleton()->GetWorldspace(),
										 nullptr,
										 nullptr,
										 RE::ObjectRefHandle(),
										 true,
										 true)
				                     .get();
				std::unique_ptr<Lighting> newProp =
					std::make_unique<Lighting>(ref, &config, *lightSelector.GetSelection());

				newProp->Init3D();
				AddItem(std::move(newProp));
			}
			else
			{
				logger::error("Could not find light reference in InGameLightingToolkit.esp");
			}
		}
	}
	ImGui::EndChild();
}

RE::BSEventNotifyControl LightingToolkit::ProcessEvent(
	const RE::BGSActorCellEvent* a_event,
	RE::BSTEventSource<RE::BGSActorCellEvent>*)
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

	for (const auto& prop : items)
	{
		if (prop->GetCellID() == a_event->cellID)
		{
			prop->OnEnterCell();
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}

void LightingToolkit::SerializeItems(SKSE::CoSaveIO io) const
{
	logger::info("Serializing Lights...");
	io.Write(items.size());
	for (const auto& light : items)
	{
		light->Serialize(io);
	}
}

void LightingToolkit::DeserializeItems(SKSE::CoSaveIO io)
{
	size_t itemsCount;
	io.Read(itemsCount);

	for (size_t i = 0; i < itemsCount; i++)
	{
		auto light = Lighting::Deserialize(io, &config);
		AddItem(std::move(light));
	}
}

void LightingToolkit::Revert(SKSE::CoSaveIO)
{
	logger::info("Reverting Save");
	items.clear();
}

constexpr uint32_t LightingToolkit::GetKey() { return serializationKey; }

void LightingToolkit::PositionLight()
{
	if (currentTab)
		currentTab->MoveToCameraLookingAt(true);
}

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
