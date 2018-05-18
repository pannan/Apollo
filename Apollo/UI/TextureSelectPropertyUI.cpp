#include "stdafx.h"
#include "TextureSelectPropertyUI.h"
#include "AssetsPropertyPanelUI.h"
#include "AssetsDirectoryViewUI.h"
#include "AssetsDirectoryManager.h"
#include "ResourceManager.h"
#include "CharacterTools.h"
#include "TextureResource.h"
#include "Texture2dDX11.h"

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
		handle = ResourceManager::getInstance().getResourceHandle(selectNode->m_name);
		return;
	}
	
	TextureResource* texResource = (TextureResource*)ResourceManager::getInstance().getResource(handle);
	if (texResource == nullptr)
		return;

	ID3D11ShaderResourceView* srv = nullptr;
	TEXTURE_TYPE type = texResource->getTextureType();
	if (type == TextureType_2D)
	{
		Texture2dDX11* tex2d = (Texture2dDX11*)texResource;
		srv = tex2d->getSRV();
	}

	if (srv == nullptr)
		return;

	ImGui::Image((void*)srv, ImVec2(300, 300));

}