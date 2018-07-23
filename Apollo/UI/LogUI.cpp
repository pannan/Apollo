#include "stdafx.h"
#include "LogUI.h"
#include "LogManager.h"

using namespace Apollo;

void LogUI::render()
{
	ImGui::SetNextWindowPos(ImVec2(200, 700), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(600, 200));

	ImGuiWindowFlags window_flags = 0;
	//window_flags |= ImGuiWindowFlags_NoResize;
	//window_flags |= ImGuiWindowFlags_NoMove;
	//window_flags |= ImGuiWindowFlags_NoScrollbar;
	static bool s_open = true;
	ImGui::Begin("Log", &s_open, window_flags);

	char* data = (char*)LogManager::getInstance().m_logData.c_str();
	ImGui::InputTextMultiline("##source", data, IM_ARRAYSIZE(data), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly);

	ImGui::End();
}