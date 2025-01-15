#include "Chiaroscuro.h"
#include "ImGui/ImGuiInputAdapter.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiTabBar.h"
#include "Lighting.h"
#include "LightingPreset.h"
#include "SKSE/SerializationControl.h"

Chiaroscuro Chiaroscuro::singleton;

void Chiaroscuro::Init()
{
	ImGui::ImGuiRenderer::GetSingleton()->Init(Style());
	ImGui::ImGuiInputAdapter::GetSingleton()->Init();
	ImGui::ImGuiRenderer::GetSingleton()->RegisterRenderTarget(&singleton);
	presetSerializationControl.Deserialize(config);
	SKSE::SerializationControl::GetSingleton()->RegisterSerializer(this);
}

void Chiaroscuro::OnDataLoaded()
{
	RE::PlayerCharacter::GetSingleton()->AddEventSink(this);
}

void Chiaroscuro::OnSavePostLoaded()
{
	for (const auto& light : items)
	{
		light->Init3D();
	}
}

void Chiaroscuro::DoFrame()
{
	if (ImGui::IsKeyPressed(ImGuiKey_End, false))
	{
		ToggleMenu();
	}

	if (!doProcess)
		return;

	if (ImGui::IsKeyPressedA(ImGuiKey_H, false))
		hidden = !hidden;

	if (hidden)
		return;

	if (ImGui::IsKeyPressedA(ImGuiKey_F, false) && !lookingAround)
		RE::Main::GetSingleton()->freezeTime = !RE::Main::GetSingleton()->freezeTime;
	if (lookingAround != (ImGui::IsKeyDownA(ImGuiKey_LeftCtrl) || ImGui::IsKeyDownA(ImGuiKey_LeftAlt)))
	{
		lookingAround = !lookingAround;

		if (lookingAround)
		{
			previouslyFreezeTimeLookingAround    = RE::Main::GetSingleton()->freezeTime;
			RE::Main::GetSingleton()->freezeTime = true;
		}
		else
		{
			RE::Main::GetSingleton()->freezeTime = previouslyFreezeTimeLookingAround;
		}

		UpdateLookingAround();
	}
	ImGui::Begin("##SCMain", nullptr, windowFlags);
	{
		ImGui::BeginDisabled(lookingAround);
		{
			DrawTabBar();
			DrawPropControlWindow();
			DrawCameraControlWindow();
			DrawSceneControlWindow();
		}
		ImGui::EndDisabled();
	}
	ImGui::End();
}

Chiaroscuro* Chiaroscuro::GetSingleton()
{
	return &singleton;
}

Chiaroscuro::Chiaroscuro() :
	ImGuiTabBar<Lighting>("##propstabbar")
{
}

void Chiaroscuro::ToggleMenu()
{
	doProcess = !doProcess;

	if (doProcess)
	{
		if (!CanOpenWindow())
		{
			RE::DebugNotification("Cannot open Chiaroscuro menu at this time...", "UIMenuOK");
			doProcess = false;
			return;
		}

		RE::PlaySound("UIMenuOK");
		SuppressDXInput();
		previouslyInFreeCameraMode = RE::PlayerCamera::GetSingleton()->IsInFreeCameraMode();
		if (!previouslyInFreeCameraMode)
		{
			RE::PlayerCamera::GetSingleton()->ToggleFreeCameraMode(true);
			RE::ControlMap::GetSingleton()->PushInputContext(RE::ControlMap::InputContextID::kTFCMode);
		}

		RE::UI::GetSingleton()->ShowMenus(false);
		previouslyFreezeTime = RE::Main::GetSingleton()->freezeTime;
	}
	else
	{
		RE::PlaySound("UIMenuCancel");
		ImGui::ImGuiInputAdapter::GetSingleton()->DisableSupression();
		RE::UI::GetSingleton()->ShowMenus(true);
		RE::Main::GetSingleton()->freezeTime = previouslyFreezeTime;
		if (!previouslyInFreeCameraMode)
		{
			RE::PlayerCamera::GetSingleton()->ToggleFreeCameraMode(false);
			RE::ControlMap::GetSingleton()->PopInputContext(RE::ControlMap::InputContextID::kTFCMode);
		}
		lookingAround = false;
		hidden        = false;
	}
}

bool Chiaroscuro::CanOpenWindow()
{
	static constexpr std::array badMenus{
		RE::MainMenu::MENU_NAME,
		RE::MistMenu::MENU_NAME,
		RE::LoadingMenu::MENU_NAME,
		RE::FaderMenu::MENU_NAME,
		"LootMenu"sv,
		"CustomMenu"sv
	};

	if (const auto UI = RE::UI::GetSingleton();
	    !UI || std::ranges::any_of(badMenus, [&](const auto& menuName) { return UI->IsMenuOpen(menuName); }) || UI->IsItemMenuOpen())
	{
		return false;
	}

	const auto* controlMap = RE::ControlMap::GetSingleton();
	if (!controlMap)
	{
		return false;
	}

	auto context_id = controlMap->contextPriorityStack.back();
	if (context_id != RE::UserEvents::INPUT_CONTEXT_ID::kGameplay && context_id != RE::UserEvents::INPUT_CONTEXT_ID::kTFCMode && context_id != RE::UserEvents::INPUT_CONTEXT_ID::kConsole)
	{
		return false;
	}

	if (RE::MenuControls::GetSingleton()->InBeastForm() || RE::VATS::GetSingleton()->mode == RE::VATS::VATS_MODE::kKillCam)
	{
		return false;
	}

	return true;
}

bool Chiaroscuro::ShouldDrawCursor()
{
	return doProcess && !lookingAround && !hidden;
}

float* Chiaroscuro::GetCameraMoveSpeed()
{
	return REL::Relocation<float*>{ RELOCATION_ID(509808, 382522) }.get();
}

void Chiaroscuro::SuppressDXInput()
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

void Chiaroscuro::UpdateLookingAround()
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

void Chiaroscuro::DrawPropControlWindow()
{
	ImGui::PushID("###PropControlWindow");
	if (currentTab)
	{
		if (ImGui::IsKeyDownA(ImGuiKey_LeftAlt))
			currentTab->MoveToCameraLookingAt(true);
		if (ImGui::IsKeyDownA(ImGuiKey_R))
			currentTab->Rotate(-0.1f);
		if (ImGui::IsKeyDownA(ImGuiKey_T))
			currentTab->Rotate(0.1f);

		ImGui::BeginChild("##PropControlPanel", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
		{
			currentTab->DrawControlPanel();
		}
		ImGui::EndChild();

		// Reset currentTab and wait for next draw cycle
		currentTab = nullptr;
	}
	ImGui::PopID();
}

void Chiaroscuro::DrawCameraControlWindow()
{
	ImGui::BeginChild("###CameraControlWindow", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
	{
		ImGui::Text("Camera Settings:");
		ImGui::Checkbox("Freeze Time", &RE::Main::GetSingleton()->freezeTime);
		ImGui::SliderAutoFill("Camera Speed", GetCameraMoveSpeed(), 0.1f, 50.0f);
	}
	ImGui::EndChild();
}

void Chiaroscuro::DrawSceneControlWindow()
{
	ImGui::BeginChild("###SceneControlWindow", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
	{
		ImGui::Text("Scene Settings:");
		if (ImGui::Button("Save Presets"))
		{
			presetSerializationControl.Serialize(config);
		}

		lightSelector.DrawValueEditor();
		if (ImGui::Button("Add Light"))
		{
			// TODO Add a PropFactory creation method here
			const auto                dataHandler = RE::TESDataHandler::GetSingleton();
			const RE::FormID          id          = dataHandler->LookupFormID(0x800, "Chiaroscuro.esp");
			const auto                ref         = dataHandler->CreateReferenceAtLocation(RE::TESForm::LookupByID(id)->As<RE::TESBoundObject>(), Lighting::GetCameraLookingAt(50.0f), RE::NiPoint3(), RE::PlayerCharacter::GetSingleton()->GetParentCell(), RE::PlayerCharacter::GetSingleton()->GetWorldspace(), nullptr, nullptr, RE::ObjectRefHandle(), true, true).get();
			std::unique_ptr<Lighting> newProp     = std::make_unique<Lighting>(ref, &config, *lightSelector.GetSelection());

			newProp->Init3D();
			AddItem(std::move(newProp));
		}
	}
	ImGui::EndChild();
}

RE::BSEventNotifyControl Chiaroscuro::ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>*)
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

void Chiaroscuro::SerializeItems(SKSE::SerializationInterface* a_intfc) const
{
	a_intfc->WriteRecordData(items.size());
	for (const auto& light : items)
	{
		light->Serialize(a_intfc);
	}
}

void Chiaroscuro::DeserializeItems(SKSE::SerializationInterface* a_intfc)
{
	size_t itemsCount;
	if (!a_intfc->ReadRecordData(itemsCount))
		return;

	for (size_t i = 0; i < itemsCount; i++)
	{
		auto light = Lighting::Deserialize(a_intfc, &config);
		AddItem(std::move(light));
	}
}

void Chiaroscuro::Revert(SKSE::SerializationInterface* a_intfc)
{
	items.clear();
}

constexpr uint32_t Chiaroscuro::GetKey()
{
	return serializationKey;
}

ImGuiStyle Chiaroscuro::Style()
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
