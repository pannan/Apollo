#include "stdafx.h"
#include "SampleManager.h"
#include "SampleBase.h"
#include "HeightMapTerrain.h"
#include "Graphics/Camera.h"
#include "Sample/OutDoorScene.h"
#include "Timer.h"
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
	m_currentSample = new OutDoorScene;
	m_currentSample->init();
	m_sampleList.push_back(m_currentSample);
}

void SampleManager::render()
{ 
	m_currentSample->render();
}

void SampleManager::debugOverlay()
{ 
	
	int frameRate = Timer::getInstance().framerate();

	static bool g_overLayShow = true;
	ImGui::SetNextWindowPos(ImVec2(400, 0));
	if (!ImGui::Begin("Example: Fixed Overlay", &g_overLayShow, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::End();
		return;
	}

	Camera* cam = m_currentSample->getCamera();
	Vector3 camPos = cam->getPosition();
	Vector3 forward = cam->getDirection();
	Vector3 rightDir = cam->getRightDir();
	ImGui::Text("FPS: (%.1i)", frameRate);
	ImGui::Separator();
	ImGui::Text("Camera Position: (%.1f,%.1f,%.1f)", camPos.m_x, camPos.m_y, camPos.m_z);
	ImGui::Text("Camera Forward: (%.1f,%.1f,%.1f)", forward.m_x, forward.m_y, forward.m_z);
	ImGui::Text("Camera RightDir: (%.1f,%.1f,%.1f)", rightDir.m_x, rightDir.m_y, rightDir.m_z);
	ImGui::End();
}
