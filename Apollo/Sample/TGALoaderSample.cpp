#include "stdafx.h"
#include "TGALoaderSample.h"
#include "Camera.h"
#include "TGAImageExt.h"
#include "TextureDX11ResourceFactory.h"
#include "Texture2dDX11.h"

using namespace Apollo;
using namespace DirectX;
using namespace std;

TGALoaderSample::TGALoaderSample()
{
	m_camera = nullptr;
}

TGALoaderSample::~TGALoaderSample()
{
	SAFE_DELETE(m_camera);
}

void saveArray2DTo3D()
{
	std::vector<std::string> arrayNameList;
	for (int z = 1; z <= 128; ++z)
	{
		TGAImageExt tgaImageExt;

		std::stringstream ss;

		if (z < 10)
			ss << "F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray.00" << z << ".tga";
		else if (z >= 10 && z < 100)
			ss << "F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray.0" << z << ".tga";
		else
			ss << "F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray." << z << ".tga";

		arrayNameList.push_back(ss.str());
	}

	TGAImageExt tgaImageExt;
	tgaImageExt.loadImageArray(arrayNameList);

	tgaImageExt.save("F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTexture.bytes");
}

void save2D()
{
	for (int z = 1; z <= 128; ++z)
	{
		TGAImageExt tgaImageExt;

		std::stringstream ss;

		if (z < 10)
			ss << "F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray.00" << z << ".tga";
		else if (z >= 10 && z < 100)
			ss << "F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray.0" << z << ".tga";
		else
			ss << "F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray." << z << ".tga";

		tgaImageExt.loadImage(ss.str());

		std::stringstream ss2;
		ss2 << "F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray." << z << ".bytes";
		tgaImageExt.save(ss2.str());
	}
}

void saveTextureArray()
{
	//save2D();

	saveArray2DTo3D();
}

uint32_t m_tgaTextureHandle = 0;

void loadTgaTexture()
{
	TGAImageExt tgaImageExt;
	tgaImageExt.loadByDirectTex("F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray.001.tga");
	tgaImageExt.save("F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray.1.bytes");
}

void TGALoaderSample::init()
{
	/*TGAImageExt tgaImageExt;

	tgaImageExt.loadImage("F:\\eBooks_code\\Cloud\\nubis_noise_generator\\my3DTextureArray.001.tga");
	tgaImageExt.save("F:\\eBooks_code\\Cloud\\nubis_noise_generator\\3DTexArrray_001.bytes");*/
	//saveTextureArray();

	loadTgaTexture();
}




void TGALoaderSample::onGUI()
{
	ImGui::SetNextWindowPos(ImVec2(100, 300));
	static bool g_overLayShow = true;
	ImGui::SetNextWindowPos(ImVec2(100, 300));
	if (!ImGui::Begin("TgaTex", &g_overLayShow, ImVec2(500, 800), 0.7f, ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::End();
		return;
	}

	{
		Texture2dDX11* srcTex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_tgaTextureHandle);

		ImGui::Image(srcTex2d->getShaderResourceView(), ImVec2(200, 200));
	}


	ImGui::End();
}

void TGALoaderSample::render()
{
	
}