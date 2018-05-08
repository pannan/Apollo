#include "stdafx.h"
#include "LogManager.h"
#include "AssetsPropertyPanelUI.h"
#include "ISelectPropertyUI.h"
#include "AssetsDirectoryViewUI.h"
#include "AssetsDirectoryManager.h"

using namespace Apollo;
using namespace std;
using namespace stdext;

void AssetsPropertyPanelUI::registerAssetsPropertyUI(std::string type,ISelectPropertyUI* ui)
{
	if (m_perportyMap.find(type) != m_perportyMap.end())
	{
		LogManager::getInstance().log("[AssetsPropertyPanelUI::registerAssetsPropertyUI] Tpye:" + type + " already register!");
		return;
	}

	m_perportyMap[type] = ui;
}

void AssetsPropertyPanelUI::unRegisterAssetsPropertyUI(std::string type, ISelectPropertyUI* ui)
{
	PropertyUIMap::iterator it = m_perportyMap.find(type);
	if (it == m_perportyMap.end())
	{
		LogManager::getInstance().log("[AssetsPropertyPanelUI::unRegisterAssetsPropertyUI] Tpye:" + type + " not register!");
		return;
	}

	if (it->second != ui)
	{
		LogManager::getInstance().log("[AssetsPropertyPanelUI::unRegisterAssetsPropertyUI] Tpye:" + type + " it->second != ui!");
		return;
	}

	m_perportyMap.erase(it);
}

ISelectPropertyUI* AssetsPropertyPanelUI::getAssetsPropertyUI(const std::string& type)
{
	if (m_perportyMap.find(type) == m_perportyMap.end())
	{
		LogManager::getInstance().log("[AssetsPropertyPanelUI::getAssetsPropertyUI] Tpye:" + type + "not register!");
		return nullptr;
	}

	return m_perportyMap[type];
}

void AssetsPropertyPanelUI::render(int w,int h)
{
	ImGui::SetNextWindowPos(ImVec2(w - 300, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, h));

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	//window_flags |= ImGuiWindowFlags_NoScrollbar;
	static bool s_open = true;
	ImGui::Begin("Property", &s_open, window_flags);

	DirectoryBaseNode*  selectNode = AssetsDirectoryViewUI::getInstance().getSelectNode();
	if (selectNode)
	{
		ISelectPropertyUI* propertyUI = getAssetsPropertyUI(selectNode->m_suffix);
		if (propertyUI)
			propertyUI->render();
	}	

	ImGui::End();
}