#include "Lighting.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset) :
	ref(ref), colorPalette(presetDB), lightCreateParams(lightPreset),
	fade(lightPreset.intensity), radius(lightPreset.radius, lightPreset.radius, lightPreset.radius), worldTranslate(ref->GetPosition())
{
}

Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::Color color, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset) :
	ref(ref), colorPalette(presetDB, color), lightCreateParams(lightPreset),
	fade(lightPreset.intensity), radius(lightPreset.radius, lightPreset.radius, lightPreset.radius), worldTranslate(ref->GetPosition())
{
}

bool Lighting::DrawTabItem(bool& active)
{
	bool isNotRemoved = true;
	bool selected     = false;
	if ((selected = ImGui::BeginTabItem(std::format("{} 0x{:X}", ref->GetFormEditorID(), ref->GetFormID()).c_str(), &isNotRemoved)))
	{
		active = selected;
		ImGui::EndTabItem();
	}
	return isNotRemoved;
}

void Lighting::DrawControlPanel()
{
	ImGui::BeginDisabled(!ref->Is3DLoaded() || !niLight || hideLight);
	{
		if (colorPalette.DrawEditor())
		{
			UpdateLightColor();
		}
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
	if (auto* model = ref->Get3D(); ImGui::ConditionalCheckbox("Hide Marker", model, &hideMarker))
	{
		model->GetObjectByName("Marker")->SetAppCulled(hideMarker);
	}
}

void Lighting::UpdateLightColor()
{
	if (auto selection = colorPalette.GetSelection())
		niLight->diffuse = *selection;
}

void Lighting::UpdateLightTemplate()
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	bsLight.reset(shadowSceneNode->AddLight(niLight.get(), lightCreateParams));
}

void Lighting::MoveToCameraLookingAt(bool resetOffset)
{
	assert(RE::PlayerCharacter::GetSingleton());
	assert(RE::PlayerCharacter::GetSingleton()->GetParentCell());

	if (GetCellID() != RE::PlayerCharacter::GetSingleton()->GetParentCell()->formID)
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
		shadowSceneNode->RemoveLight(bsLight);
		shadowSceneNode->RemoveLight(niLight.get());
		Attach3D();
	}
	if (resetOffset)
	{
		cameraOffset.y = 0;
		cameraOffset.z = 0;
	}

	auto cameraNode = RE::PlayerCamera::GetSingleton()->cameraRoot.get()->AsNode();
	auto cameraNI   = reinterpret_cast<RE::NiCamera*>((cameraNode->children.size() == 0) ? nullptr : cameraNode->children[0].get());

	if (cameraNI)
	{
		MoveTo(GetCameraPosition() + (cameraNI->world.rotate * cameraOffset));
	}
}

void Lighting::MoveTo(RE::NiPoint3 point)
{
	worldTranslate = point;
	ref->SetPosition(worldTranslate);
	niLight->world.translate = point + offset;
}

void Lighting::MoveToCurrentPosition()
{
	MoveTo(worldTranslate);
}

void Lighting::OnEnterCell()
{
	// Skyrim creates a new niLight when we enter a cell
	// so we have to update our state
	Attach3D();
	UpdateLightColor();
	UpdateLightTemplate();
	MoveToCurrentPosition();
}

RE::FormID Lighting::GetCellID()
{
	assert(ref->GetParentCell());
	return ref->GetParentCell()->formID;
}

void Lighting::Rotate(float delta)
{
	RE::NiMatrix3 rotation = {
		{ 1, 0, 0 },
		{ 0, cos(delta), -sin(delta) },
		{ 0, sin(delta), cos(delta) }
	};

	Rotate(rotation);
}

void Lighting::Remove()
{
	auto* shadowSceneNode                   = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
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

void Lighting::Rotate(RE::NiMatrix3 rotation)
{
	if (niLight)
	{
		niLight->world.rotate = niLight->world.rotate * rotation;
	}
}

RE::NiPoint3 Lighting::GetCameraLookingAt(float distanceFromCamera)
{
	auto cameraNode = RE::PlayerCamera::GetSingleton()->cameraRoot.get()->AsNode();
	auto cameraNI   = reinterpret_cast<RE::NiCamera*>((cameraNode->children.size() == 0) ? nullptr : cameraNode->children[0].get());

	if (cameraNI)
		return GetCameraPosition() + (cameraNI->world.rotate * RE::NiPoint3{ distanceFromCamera, 0.0f, 0.0f });

	return RE::NiPoint3();
}

RE::BSFadeNode* Lighting::Attach3D()
{
	if (!ref->Is3DLoaded())
		ref->Load3D(false);

	auto* niRoot = ref->Get3D()->AsFadeNode();

	if (!niRoot)
	{
		logger::error("Base Root Node not found!");
	}

	if (!niRoot)
		return nullptr;

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
		offset = niNode->local.translate;

		// TODO put these into a settings class
		niLight->ambient = RE::NiColor();
		niLight->radius  = radius;
		niLight->SetLightAttenuation(500);
		niLight->fade = fade;
	}

	return niRoot;
}

void Lighting::Init3D()
{
	hideLight = false;
	hideMarker = false;
	Attach3D();
	MoveTo(worldTranslate);
	UpdateLightColor();
	UpdateLightTemplate();
}

void Lighting::DrawCameraOffsetSlider()
{
	bool changed = false;
	changed      = ImGui::SliderAutoFill("Offset Forward/Backward", &cameraOffset.x, 0.1f, 500.0f);
	changed      = ImGui::SliderAutoFill("Offset Up/Down", &cameraOffset.y, -50.0f, 50.0f) || changed;
	changed      = ImGui::SliderAutoFill("Offset Left/Right", &cameraOffset.z, -50.0f, 50.0f) || changed;

	if (changed)
	{
		MoveToCameraLookingAt();
	}
}

RE::NiPoint3 Lighting::GetCameraPosition()
{
	RE::NiPoint3 origin;
	if (RE::PlayerCamera::GetSingleton()->IsInFreeCameraMode())
		reinterpret_cast<RE::FreeCameraState*>(RE::PlayerCamera::GetSingleton()->currentState.get())->GetTranslation(origin);
	else
		origin = RE::PlayerCamera::GetSingleton()->pos;

	return origin;
}
