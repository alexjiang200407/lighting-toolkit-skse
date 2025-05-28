#include "SceneLight.h"

SceneLight::SceneLight(
	RE::TESObjectREFRPtr    ref,
	preset::PresetDatabase* presetDB,
	preset::LightingPreset  lightPreset) :
	SceneLight(ref, presetDB, lightPreset, lightPreset.intensity, lightPreset.radius)
{}

SceneLight::SceneLight(
	RE::TESObjectREFRPtr    ref,
	preset::Color           color,
	preset::PresetDatabase* presetDB,
	preset::LightingPreset  lightPreset) :
	SceneLight(ref, color, presetDB, lightPreset, lightPreset.intensity, lightPreset.radius)
{}

SceneLight::SceneLight(
	RE::TESObjectREFRPtr                     ref,
	preset::PresetDatabase*                  presetDB,
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightPreset,
	float                                    fade,
	float                                    radius,
	bool                                     hideLight,
	bool                                     hideMarker) :
	ref(ref),
	colorPalette(presetDB), lightCreateParams(lightPreset), fade(fade),
	radius(radius, radius, radius), worldTranslate(ref->GetPosition()), hideLight(hideLight),
	hideMarker(hideMarker)
{}

SceneLight::SceneLight(
	RE::TESObjectREFRPtr                     ref,
	preset::Color                            color,
	preset::PresetDatabase*                  presetDB,
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightPreset,
	float                                    fade,
	float                                    radius,
	bool                                     hideLight,
	bool                                     hideMarker) :
	ref(ref),
	colorPalette(presetDB, color), lightCreateParams(lightPreset), fade(fade),
	radius(radius, radius, radius), worldTranslate(ref->GetPosition()), hideLight(hideLight),
	hideMarker(hideMarker)
{}

bool SceneLight::DrawTabItem(bool& active)
{
	bool isNotRemoved = true;
	bool selected     = false;

	if ((selected = ImGui::BeginTabItem(tabLabel.c_str(), &isNotRemoved)))
	{
		active = selected;
		ImGui::EndTabItem();
	}

	return isNotRemoved;
}

void SceneLight::DrawControlPanel()
{
	ImGui::PushID(std::bit_cast<int>(ref->GetFormID()));
	{
		bool disabled = !ref->Is3DLoaded() || !niLight || hideLight;
		ImGui::BeginDisabled(disabled);
		{
			if (ImGui::BeginPanel("##ColorSelector"))
			{
				if (colorPalette.DrawEditor())
				{
					UpdateLightColor();
				}
			}
			ImGui::EndPanel();
		}
		ImGui::EndDisabled();

		if (ImGui::BeginPanel("##LightProperties"))
		{
			ImGui::BeginDisabled(disabled);
			{
				ImGui::SliderAutoFill("Light Radius", &niLight->radius.x, 32.0f, 1024.0f);
				ImGui::SliderAutoFill("Light Intensity", &niLight->fade, 0.0f, 10.0f);

				radius = niLight->radius;
				fade   = niLight->fade;
				DrawCameraOffsetSlider();
			}
			ImGui::EndDisabled();
			if (ImGui::ConditionalCheckbox("Hide Light", niLight.get(), &hideLight))
			{
				niLight->SetAppCulled(hideLight);
			}
			ImGui::SameLine();
			if (auto* model = ref->Get3D();
			    ImGui::ConditionalCheckbox("Hide Marker", model, &hideMarker))
			{
				model->GetObjectByName("Marker")->SetAppCulled(hideMarker);
			}
		}
		ImGui::EndPanel();
	}
	ImGui::PopID();
}

void SceneLight::UpdateLightColor()
{
	if (auto selection = colorPalette.GetSelection())
	{
		niLight->diffuse = *selection;
		tabLabel         = std::format("{}###{}", selection->GetName(), ref->GetFormID());
	}
}

void SceneLight::UpdateLightTemplate()
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	bsLight.reset(shadowSceneNode->AddLight(niLight.get(), lightCreateParams));
}

void SceneLight::MoveToCameraLookingAt(bool resetOffset)
{
	auto cameraNode = RE::PlayerCamera::GetSingleton()->cameraRoot.get()->AsNode();
	auto cameraNI   = reinterpret_cast<RE::NiCamera*>(
        (cameraNode->children.size() == 0) ? nullptr : cameraNode->children[0].get());

	if (auto* player = RE::PlayerCharacter::GetSingleton(); cameraNI && player)
	{
		bool doReset = GetCellID() != RE::PlayerCharacter::GetSingleton()->GetParentCell()->formID;
		ref->MoveTo(player->AsReference());
		MoveTo(GetCameraPosition() + (cameraNI->world.rotate * cameraOffset));

		if (doReset)
		{
			auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
			shadowSceneNode->RemoveLight(bsLight);
			shadowSceneNode->RemoveLight(niLight.get());
			Attach3D();
			UpdateLightColor();
			UpdateLightTemplate();
		}

		if (resetOffset)
		{
			cameraOffset.y = 0;
			cameraOffset.z = 0;
		}
	}
}

void SceneLight::MoveTo(RE::NiPoint3 point)
{
	hideMarker     = false;
	worldTranslate = point;
	ref->SetPosition(worldTranslate);
	niLight->world.translate = point;
}

void SceneLight::MoveToCurrentPosition() { MoveTo(worldTranslate); }

void SceneLight::OnEnterCell()
{
	// Skyrim creates a new niLight when we enter a cell
	// so we have to update our state
	Attach3D();
	UpdateLightColor();
	UpdateLightTemplate();
	MoveToCurrentPosition();
}

RE::FormID SceneLight::GetCellID() const
{
	assert(ref->GetParentCell());
	return ref->GetParentCell()->formID;
}

void SceneLight::Rotate(float delta)
{
	RE::NiMatrix3 rotation = { { 1, 0, 0 },
		                       { 0, cos(delta), -sin(delta) },
		                       { 0, sin(delta), cos(delta) } };

	Rotate(rotation);
}

void SceneLight::Remove()
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
	shadowSceneNode->allowLightRemoveQueues = false;

	if (niLight.get())
		shadowSceneNode->RemoveLight(niLight.get());

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	shadowSceneNode->allowLightRemoveQueues = true;

	niLight->SetAppCulled(true);
	ref->Disable();
	ref->SetDelete(true);
}

void SceneLight::Rotate(RE::NiMatrix3 rotation)
{
	if (niLight)
	{
		niLight->world.rotate = niLight->world.rotate * rotation;
	}
}

RE::NiPoint3 SceneLight::GetCameraLookingAt(float distanceFromCamera)
{
	auto* playerCamera = RE::PlayerCamera::GetSingleton();

	if (!playerCamera)
	{
		logger::error("Could not get player camera");
		return RE::NiPoint3();
	}

	auto* cameraRoot = playerCamera->cameraRoot.get();

	if (!cameraRoot)
	{
		logger::error("Could not get camera root");
		return RE::NiPoint3();
	}

	auto* cameraNode = cameraRoot->AsNode();

	if (!cameraNode)
	{
		logger::error("Could not get camera node");
		return RE::NiPoint3();
	}

	auto cameraNI = reinterpret_cast<RE::NiCamera*>(
		(cameraNode->children.size() == 0) ? nullptr : cameraNode->children[0].get());

	if (cameraNI)
		return GetCameraPosition() +
		       (cameraNI->world.rotate * RE::NiPoint3{ distanceFromCamera, 0.0f, 0.0f });

	logger::error("Could not get NiCamera");
	return RE::NiPoint3();
}

RE::BSFadeNode* SceneLight::Attach3D()
{
	if (!ref->Is3DLoaded())
		ref->Load3D(false);

	auto* ref3D = ref->Get3D();

	if (!ref3D)
	{
		logger::error("Cannot attach 3D. 3D is not loaded");
		return nullptr;
	}

	auto* niRoot = ref3D->AsFadeNode();

	if (!niRoot)
	{
		logger::error("Base Root Node not found!");
		return nullptr;
	}

	if (auto* newLight = niRoot->GetObjectByName("ChiaroscuroLight"))
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
		shadowSceneNode->RemoveLight(niLight.get());
		niLight.reset(skyrim_cast<RE::NiPointLight*>(newLight));
	}
	else
	{
		auto* niNode = niRoot->GetObjectByName("AttachLight")->AsNode();
		niLight.reset(RE::NiPointLight::Create());
		niLight->name = "ChiaroscuroLight";
		RE::AttachNode(niNode, niLight.get());

		niLight->ambient = RE::NiColor();
		niLight->radius  = radius;
		niLight->SetLightAttenuation(500);
		niLight->fade = fade;
	}

	if (hideLight)
		niLight->SetAppCulled(true);

	if (hideMarker)
		niRoot->GetObjectByName("Marker")->SetAppCulled(true);

	return niRoot;
}

void SceneLight::Init3D()
{
	Attach3D();
	//MoveTo(worldTranslate); // Moving causes the EXCEPTION_ACCESS_VIOLATION bug
	niLight->world.translate = worldTranslate;
	UpdateLightColor();
	UpdateLightTemplate();
}

std::optional<SceneLight>
	SceneLight::Deserialize(SKSE::CoSaveIO io, preset::PresetDatabase* presetDB)
{
	RE::FormID                               formID;
	float                                    fade, radius;
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightCreateParams;
	bool                                     hideLight, hideMarker;

	io.Read(hideLight);
	io.Read(hideMarker);
	io.Read(fade);
	io.Read(radius);
	preset::Color color = ColorPalette::Deserialize(io, presetDB);
	io.Read(lightCreateParams);
	io.Read(formID);

	auto* tesForm = RE::TESObjectREFR::LookupByID(formID);

	if (!tesForm)
		return std::nullopt;

	auto* tesObjectREFR = tesForm->As<RE::TESObjectREFR>();
	if (!tesObjectREFR)
		return std::nullopt;

	return SceneLight(
		tesObjectREFR->CreateRefHandle().get(),
		color,
		presetDB,
		lightCreateParams,
		fade,
		radius,
		hideLight,
		hideMarker);
}

void SceneLight::DrawCameraOffsetSlider()
{
	bool changed = false;
	changed      = ImGui::SliderAutoFill("Offset Forward/Backward", &cameraOffset.x, 0.1f, 500.0f);
	changed = ImGui::SliderAutoFill("Offset Up/Down", &cameraOffset.y, -50.0f, 50.0f) || changed;
	changed = ImGui::SliderAutoFill("Offset Left/Right", &cameraOffset.z, -50.0f, 50.0f) || changed;

	if (changed)
	{
		MoveToCameraLookingAt();
	}
}

void SceneLight::Serialize(SKSE::CoSaveIO io) const
{
	io.Write(hideLight);
	io.Write(hideMarker);
	io.Write(fade);
	io.Write(radius.x);
	colorPalette.Serialize(io);
	io.Write(lightCreateParams);
	io.Write(ref->GetFormID());
}

RE::NiPoint3 SceneLight::GetCameraPosition()
{
	RE::NiPoint3 origin;
	auto*        camera = RE::PlayerCamera::GetSingleton();

	if (!camera)
	{
		logger::error("Could not get player camera");
		return RE::NiPoint3();
	}

	if (camera->IsInFreeCameraMode())
		reinterpret_cast<RE::FreeCameraState*>(camera->currentState.get())->GetTranslation(origin);
	else
		origin = RE::PlayerCamera::GetSingleton()->pos;

	return origin;
}
