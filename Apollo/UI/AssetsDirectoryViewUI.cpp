#include "stdafx.h"
#include "AssetsDirectoryViewUI.h"
#include "AssetsDirectoryManager.h"

using namespace Apollo;

AssetsDirectoryViewUI::AssetsDirectoryViewUI() : m_selectNode(nullptr)
{

}

AssetsDirectoryViewUI::~AssetsDirectoryViewUI()
{

}

void AssetsDirectoryViewUI::drawNode(DirectoryNode* node)
{
	if (node == nullptr)
		return;

	ImGuiTreeNodeFlags node_flags = ((m_selectNode == node) ? ImGuiTreeNodeFlags_Selected : 0);

	node_flags |=ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	bool node_open = false;

	node_open = ImGui::TreeNodeEx((void*)(intptr_t)node->m_id, node_flags, node->m_name.c_str(), node->m_id);
	if (ImGui::IsItemClicked())
		m_selectNode = node;
	
	if (node_open == false)
		return;

	for each (DirectoryNode * var in node->m_subDirectoryList)
	{
		drawNode(var);
	}

	for each (FileNode* var in node->m_fileList)
	{
		node_flags = ((m_selectNode == var) ? ImGuiTreeNodeFlags_Selected : 0);
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)var, node_flags, var->m_name.c_str(), var->m_id);
		if (ImGui::IsItemClicked())
			m_selectNode = var;
	}

	if(node_open)
		ImGui::TreePop();
}

void AssetsDirectoryViewUI::render(int w, int h)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(200, h));

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	static bool s_open = true;
	ImGui::Begin("Assets", &s_open, window_flags);

	AssetsDirectoryManager* assetsDirectory = AssetsDirectoryManager::getInstancePtr();
	if (assetsDirectory)
	{
		drawNode(assetsDirectory->getRootNode());				
		//ImGui::TreePop();
	}
		
	ImGui::End();
}