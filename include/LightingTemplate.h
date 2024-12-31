#pragma once
#include "ImGui/ImGuiSelector.h"

struct LightingTemplateData
{
	enum class Flags
	{
		kPortalStrict = 1,
		kDynamic      = 1 << 1,
		kCastsShadow  = (1 << 2) | kDynamic,
		kAffectLand   = 1 << 3,
		kAffectWater  = 1 << 4,
		kAffectAll    = kAffectLand | kAffectWater,
		kNeverFades   = 1 << 5
	};

	typedef stl::enumeration<Flags, uint32_t> LightFlags;

	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS ToLightCreateParams() const;

	LightFlags  flags;
	float       fov;
	float       falloff;
	float       depthBias;
	float       nearDistance;
	std::string nameID;
};

class LightingTemplate :
	public ImGui::ImGuiSelector<LightingTemplateData>
{
public:
	LightingTemplate(int lightTemplateIdx);

public:
	const char*                 GetSelectionID() override;
	const char*                 SelectionName(int idx) const override;
	int                         GetSelectionCount() const override;
	static void                 LoadLightingTemplates();
	const LightingTemplateData& GetCurrentSelection() const override;

private:
	static inline std::vector<LightingTemplateData> templates;
};
