#include "stdafx.h"
#include "DDSLoaderSample.h"
#include "Camera.h"
#include "TextureDX11ResourceFactory.h"
#include "Texture2dDX11.h"
#include "DirectXTex.h"

using namespace Apollo;
using namespace DirectX;
using namespace std;


DDSLoaderSample::DDSLoaderSample()
{
	m_camera = nullptr;
}

DDSLoaderSample::~DDSLoaderSample()
{
	SAFE_DELETE(m_camera);
}


void DDSLoaderSample::init()
{
	TexMetadata meta_data;
	ScratchImage scratch_image;
	HRESULT hr = LoadFromDDSFile(L"D:\\GitHub\\Apollo\\bin\\Assets\\Texture\\MultipleSctr.dds", 0, &meta_data, scratch_image);
	const DirectX::Image* images = scratch_image.GetImages();
	int image_size = scratch_image.GetPixelsSize();
	int width = meta_data.width;
	int height = meta_data.height;
	int depth = meta_data.depth;

	//下面是内存大小，字节
	int pixel_count_bytes = width * height * depth * 4;

	FILE* fp = fopen("single_multiscattering_rf32.bytes", "wb");

	fwrite(&width, sizeof(int), 1, fp);
	fwrite(&height, sizeof(int), 1, fp);
	fwrite(&depth, sizeof(int), 1, fp);

	fwrite(images[0].pixels, 1, image_size, fp);

	fclose(fp);
}

void DDSLoaderSample::onGUI()
{
	ImGui::SetNextWindowPos(ImVec2(100, 300));
	static bool g_overLayShow = true;
	ImGui::SetNextWindowPos(ImVec2(100, 300));
	if (!ImGui::Begin("DDSLoader", &g_overLayShow, ImVec2(500, 800), 0.7f, ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::End();
		return;
	}
	if (ImGui::Button("Loader", ImVec2(200, 50)))
	{
		
	}


	ImGui::End();
}

void DDSLoaderSample::render()
{
	
}