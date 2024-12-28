#include "SceneCraft.h"


SceneCraft SceneCraft::singleton;

void SceneCraft::Init()
{
	singleton.renderer.RegisterRenderTarget(&singleton);
}

void SceneCraft::DoFrame()
{
	ImGui::ShowDemoWindow();
}
