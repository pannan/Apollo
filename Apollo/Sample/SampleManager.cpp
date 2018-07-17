#include "stdafx.h"
#include "SampleManager.h"
#include "SampleBase.h"
#include "HeightMapTerrain.h"

using namespace Apollo;

SampleManager::SampleManager()
{

}

SampleManager::~SampleManager()
{
	for each (SampleBase* var in m_sampleList)
	{
		SAFE_DELETE(var);
	}
}

void SampleManager::init()
{
	m_currentSample = new HeightMapTerrain;
	m_currentSample->init();
	m_sampleList.push_back(m_currentSample);
}

void SampleManager::render()
{
	m_currentSample->render();
}

void SampleManager::debugOverlay()
{
	static bool g_overLayShow = true;
	ImGui::SetNextWindowPos(ImVec2(400, 0));
	if (!ImGui::Begin("Example: Fixed Overlay", &g_overLayShow, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Simple overlay\non the top-left side of the screen.");
	ImGui::Separator();
	ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	ImGui::End();
}
