#include "stdafx.h"
#include "TextureSelectPropertyUI.h"
#include "AssetsPropertyPanelUI.h"
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

}