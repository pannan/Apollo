#include "stdafx.h"
#include "TextureSelectPropertyUI.h"
#include "AssetsPropertyPanelUI.h"
#include "AssetsDirectoryViewUI.h"
#include "AssetsDirectoryManager.h"
#include "ResourceManager.h"

using namespace Apollo;

TextureSelectPropertyUI::TextureSelectPropertyUI()
{
	AssetsPropertyPanelUI::getInstance().registerAssetsPropertyUI("dds", this);
	AssetsPropertyPanelUI::getInstance().registerAssetsPropertyUI("tga", this);
	AssetsPropertyPanelUI::getInstance().registerAssetsPropertyUI("exr", this);
}

TextureSelectPropertyUI::~TextureSelectPropertyUI()
{
	AssetsPropertyPanelUI::getInstance().unRegisterAssetsPropertyUI("dds", this);
	AssetsPropertyPanelUI::getInstance().unRegisterAssetsPropertyUI("tga", this);
	AssetsPropertyPanelUI::getInstance().unRegisterAssetsPropertyUI("exr", this);
}

void TextureSelectPropertyUI::render()
{
	DirectoryBaseNode* selectNode = AssetsDirectoryViewUI::getInstance().getSelectNode();

	if (selectNode == nullptr)
		return;

	uint32_t& handle = selectNode->m_handle;

	if (handle == 0)
	{
		handle = ResourceManager::getInstance().getResource(selectNode->m_name);
	}
}