#include "stdafx.h"
#include "UIRoot.h"
#include "AssetsDirectoryViewUI.h"
#include "AssetsPropertyPanelUI.h"
#include "TextureSelectPropertyUI.h"

using namespace Apollo;

UIRoot::UIRoot()
{
	m_assetsDirectoryViewUI = new AssetsDirectoryViewUI;

	//ע��˳������TextureSelectPropertyUI�ڹ��캯��Ҫȥ����AssetsPropertyPanelUI��ע�ắ��������AssetsPropertyPanelUIҪ���ȴ���
	m_assetsPropertyPanelUI = new AssetsPropertyPanelUI;
	m_textureSelectPropertyUI = new TextureSelectPropertyUI;
}

UIRoot::~UIRoot()
{
	SAFE_DELETE(m_assetsDirectoryViewUI);

	//ע��˳������TextureSelectPropertyUI����������Ҫȥ����AssetsPropertyPanelUI�ķ�ע�ắ��������AssetsPropertyPanelUIҪ�������
	SAFE_DELETE(m_textureSelectPropertyUI);
	SAFE_DELETE(m_assetsPropertyPanelUI);
}

void UIRoot::render(int w,int h)
{
	m_assetsDirectoryViewUI->render(w,h);
	m_assetsPropertyPanelUI->render(w, h);
}