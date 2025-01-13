#include "Prop.h"

Prop::Prop(RE::TESObjectREFRPtr ref) :
	ref(ref), worldTranslate(ref->GetPosition())
{
}

bool Prop::DrawTabItem(bool& active)
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

void Prop::Remove()
{
	ref->Disable();
	ref->SetDelete(true);
}

void Prop::MoveToCurrentPosition()
{
	MoveTo(worldTranslate);
}

void Prop::OnEnterCell()
{
	// TODO Add default behavior
}

RE::FormID Prop::GetCellID()
{
	assert(ref->GetParentCell());
	return ref->GetParentCell()->formID;
}

void Prop::Rotate(float delta)
{
	RE::NiMatrix3 rotation = {
		{ 1, 0, 0 },
		{ 0, cos(delta), -sin(delta) },
		{ 0, sin(delta), cos(delta) }
	};

	Rotate(rotation);
}

void Prop::Rotate(RE::NiMatrix3 rotation)
{
	if (!ref->Is3DLoaded())
		return;

	RE::BSFadeNode* fadeNode = ref->Get3D()->AsFadeNode();

	if (!fadeNode)
		return;

	fadeNode->world.rotate = fadeNode->world.rotate * rotation;
}

RE::NiPoint3 Prop::GetCameraLookingAt(float distanceFromCamera)
{
	auto cameraNode = RE::PlayerCamera::GetSingleton()->cameraRoot.get()->AsNode();
	auto cameraNI   = reinterpret_cast<RE::NiCamera*>((cameraNode->children.size() == 0) ? nullptr : cameraNode->children[0].get());

	if (cameraNI)
		return GetCameraPosition() + (cameraNI->world.rotate * RE::NiPoint3{ distanceFromCamera, 0.0f, 0.0f });

	return RE::NiPoint3();
}

RE::BSFadeNode* Prop::Attach3D()
{
	if (!ref->Is3DLoaded())
		ref->Load3D(false);

	auto* niRoot = ref->Get3D()->AsFadeNode();

	if (!niRoot)
	{
		logger::error("Base Root Node not found!");
	}

	return niRoot;
}

void Prop::Init3D()
{
	Attach3D();
	MoveTo(worldTranslate);
}

void Prop::DrawCameraOffsetSlider()
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

void Prop::MoveToCameraLookingAt(bool resetOffset)
{
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

void Prop::MoveTo(RE::NiPoint3 newPos)
{
	worldTranslate = newPos;	
	ref->SetPosition(worldTranslate);
}

RE::NiPoint3 Prop::GetCameraPosition()
{
	RE::NiPoint3 origin;
	if (RE::PlayerCamera::GetSingleton()->IsInFreeCameraMode())
		reinterpret_cast<RE::FreeCameraState*>(RE::PlayerCamera::GetSingleton()->currentState.get())->GetTranslation(origin);
	else
		origin = RE::PlayerCamera::GetSingleton()->pos;

	return origin;
}
