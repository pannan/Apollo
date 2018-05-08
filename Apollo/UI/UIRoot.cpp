#include "stdafx.h"
#include "UIRoot.h"
#include "AssetsDirectoryViewUI.h"
#include "AssetsPropertyPanelUI.h"
#include "TextureSelectPropertyUI.h"

using namespace Apollo;

UIRoot::UIRoot()
{
	m_assetsDirectoryViewUI = new AssetsDirectoryViewUI;

	//注意顺序，由于TextureSelectPropertyUI在构造函数要去调用AssetsPropertyPanelUI的注册函数，所以AssetsPropertyPanelUI要最先创建
	m_assetsPropertyPanelUI = new AssetsPropertyPanelUI;
	m_textureSelectPropertyUI = new TextureSelectPropertyUI;
}

UIRoot::~UIRoot()
{
	SAFE_DELETE(m_assetsDirectoryViewUI);

	//注意顺序，由于TextureSelectPropertyUI在析构函数要去调用AssetsPropertyPanelUI的反注册函数，所以AssetsPropertyPanelUI要最后销毁
	SAFE_DELETE(m_textureSelectPropertyUI);
	SAFE_DELETE(m_assetsPropertyPanelUI);
}

void UIRoot::render(int w,int h)
{
	m_assetsDirectoryViewUI->render(w,h);
	m_assetsPropertyPanelUI->render(w, h);
}