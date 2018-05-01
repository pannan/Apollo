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

	if (ImGui::IsItemClicked())
		m_selectNode = node;

	ImGuiTreeNodeFlags node_flags = ((m_selectNode == node) ? ImGuiTreeNodeFlags_Selected : 0);

	bool isLeaf = node->m_subDirectoryList.size() == 0 && node->m_fileList.size() == 0;

	if(isLeaf)
		node_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	else
	{
		node_flags |=ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	}

	bool node_open = false;
	//if (isLeaf)
	//{
	//	node_open = ImGui::TreeNode(node->m_path.c_str());
	//	//ImGui::Text(node->m_path.c_str());
	//	return;
	//}
	//else
	{
		node_open = ImGui::TreeNodeEx((void*)(intptr_t)node->m_id, node_flags, node->m_path.c_str(), node->m_id);
	}
	
	if (node_open == false)
		return;

	for each (DirectoryNode * var in node->m_subDirectoryList)
	{
		drawNode(var);
	}

	for each (std::string var in node->m_fileList)
	{
		ImGui::TreeNode(var.c_str());
		//ImGui::Text(var.c_str());
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