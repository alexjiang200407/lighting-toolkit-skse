#include "SceneLighting.h"
#include "ImGui/ImGuiInputAdapter.h"

void SceneLighting::DrawTabBar()
{
	if (ImGui::BeginTabBar(
			"###Lights",
			(ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll)))
	{
		for (auto it = lights.begin(); it != lights.end();)
		{
			bool active = false;
			bool open   = it->DrawTabItem(active);

			if (open)
			{
				if (active)
					currentTab = std::distance(lights.begin(), it);
				it++;
			}
			else if (!open)
			{
				it->Remove();
				it = lights.erase(it);
			}
		}
		ImGui::EndTabBar();
	}
}

void SceneLighting::PositionLight()
{
	if (auto* light = GetCurrentLight())
		light->MoveToCameraLookingAt(true);
}

void SceneLighting::OnSavePostLoaded()
{
	for (auto& light : lights)
	{
		light.Init3D();
	}
}

void SceneLighting::EndFrame() { currentTab = std::nullopt; }

void SceneLighting::SerializeItems(SKSE::CoSaveIO io) const
{
	logger::info("Serializing Lights...");
	io.Write(lights.size());
	for (const auto& light : lights)
	{
		light.Serialize(io);
	}
}

void SceneLighting::DeserializeItems(SKSE::CoSaveIO io, preset::PresetDatabase* config)
{
	size_t itemsCount;
	io.Read(itemsCount);

	for (size_t i = 0; i < itemsCount; i++)
	{
		if (auto light = SceneLight::Deserialize(io, config))
		{
			lights.push_back(std::move(*light));
		}
	}
}

void SceneLighting::Revert(SKSE::CoSaveIO)
{
	logger::info("Reverting Save");

	for (auto& light : lights)
	{
		light.Remove();
	}

	lights.clear();
}

static RE::TESObjectREFRPtr PlaceLight()
{
	const auto dataHandler = RE::TESDataHandler::GetSingleton();

	if (!dataHandler)
	{
		logger::error("Data Handler singleton not found");
		return nullptr;
	}

	const RE::FormID id = dataHandler->LookupFormID(0x801, "InGameLightingToolkit.esp");

	if (!id)
	{
		logger::error("Data Handler could not find ID");
		return nullptr;
	}

	if (auto* boundObj = RE::TESForm::LookupByID(id)->As<RE::TESBoundObject>())
	{
		const auto ref = dataHandler
		                     ->CreateReferenceAtLocation(
								 boundObj,
								 SceneLight::GetCameraLookingAt(50.0f),
								 RE::NiPoint3(),
								 RE::PlayerCharacter::GetSingleton()->GetParentCell(),
								 RE::PlayerCharacter::GetSingleton()->GetWorldspace(),
								 nullptr,
								 nullptr,
								 RE::ObjectRefHandle(),
								 true,
								 true)
		                     .get();
		return ref;
	}

	logger::error("Could not find light reference in InGameLightingToolkit.esp");
	return nullptr;
}

void SceneLighting::DrawPropControlWindow()
{
	ImGui::PushID("###PropControlWindow");
	if (auto* currentLight = GetCurrentLight())
	{
		if (ImGui::ImGuiInputAdapter::IsKeyDown("iRotateLeftKey"))
			currentLight->Rotate(-0.1f);
		if (ImGui::ImGuiInputAdapter::IsKeyDown("iRotateRightKey"))
			currentLight->Rotate(0.1f);

		currentLight->DrawControlPanel();
	}
	ImGui::PopID();
}

void SceneLighting::DrawSceneControlWindow(preset::PresetDatabase* config)
{
	if (ImGui::BeginPanel("Scene Settings###SceneControlWindow"))
	{
		lightSelector.DrawValueEditor();
		if (ImGui::Button("Add Light"))
		{
			if (auto ref = PlaceLight())
			{
				SceneLight newProp(ref, config, *lightSelector.GetSelection());
				newProp.Init3D();
				lights.push_back(std::move(newProp));
			}
		}
	}
	ImGui::EndPanel();
}

void SceneLighting::DrawWindow(preset::PresetDatabase* config)
{
	DrawTabBar();
	DrawPropControlWindow();
	DrawSceneControlWindow(config);
}

RE::BSEventNotifyControl SceneLighting::ProcessEvent(
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

	for (auto& light : lights)
	{
		if (light.GetCellID() == a_event->cellID)
		{
			light.OnEnterCell();
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}

SceneLight* SceneLighting::GetCurrentLight()
{
	if (!currentTab)
		return nullptr;

	if (*currentTab >= lights.size())
	{
		logger::error("CurrentTab index out of bounds");
		return nullptr;
	}

	return &lights[*currentTab];
}
