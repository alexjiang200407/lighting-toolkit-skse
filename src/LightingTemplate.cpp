#include "LightingTemplate.h"

LightingTemplate::LightingTemplate(int lightTemplateIdx) :
	ImGui::ImGuiSelector<LightingTemplateData>(lightTemplateIdx)
{
}

const char* LightingTemplate::GetSelectionID()
{
	return "Lighting Template";
}

const char* LightingTemplate::SelectionName(int idx) const
{
	return templates[idx].nameID.c_str();
}

int LightingTemplate::GetSelectionCount() const
{
	return (int)templates.size();
}

void LightingTemplate::LoadLightingTemplates()
{
	using FLAGS = LightingTemplateData::Flags;
	// TODO read from JSON file
	LightingTemplateData data;

	data.depthBias    = 1.0f;
	data.falloff      = 1.0f;
	data.fov          = RE::NI_PI;
	data.nearDistance = 6.0;
	data.depthBias    = 1.0f;
	data.nameID       = "Shadow90";
	data.flags.set(FLAGS::kAffectAll, FLAGS::kCastsShadow, FLAGS::kPortalStrict, FLAGS::kNeverFades);

	templates.push_back(data);

	data.fov    = RE::NI_TWO_PI;
	data.nameID = "Shadow180";

	templates.push_back(data);

	data.flags = FLAGS();
	data.flags.set(FLAGS::kAffectAll, FLAGS::kDynamic, FLAGS::kPortalStrict, FLAGS::kNeverFades);
	data.nameID = "Dynamic180";
	templates.push_back(data);
}

const LightingTemplateData& LightingTemplate::GetCurrentSelection() const
{
	return templates[selectionIdx];
}

RE::ShadowSceneNode::LIGHT_CREATE_PARAMS LightingTemplateData::ToLightCreateParams() const
{
	using FLAGS = LightingTemplateData::Flags;
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS params{};

	params.affectLand      = flags.all(FLAGS::kAffectLand);
	params.affectWater     = flags.all(FLAGS::kAffectWater);
	params.depthBias       = depthBias;
	params.dynamic         = flags.all(FLAGS::kDynamic);
	params.falloff         = falloff;
	params.fov             = fov;
	params.lensFlareData   = nullptr;
	params.nearDistance    = nearDistance;
	params.neverFades      = flags.all(FLAGS::kNeverFades);
	params.portalStrict    = flags.all(FLAGS::kPortalStrict);
	params.restrictedNode  = nullptr;
	params.shadowLight     = flags.all(FLAGS::kCastsShadow);
	params.sceneGraphIndex = 0;

	return params;
}
