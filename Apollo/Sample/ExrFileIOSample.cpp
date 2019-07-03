#include "stdafx.h"
#include "ExrFileIOSample.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
#include "DirectXTexEXR.h"
#include "CharacterTools.h"
#include <ImfRgbaFile.h>
#include <ImfIO.h>
#include <ImfTiledRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>
//#include <drawImage.h>

using namespace Apollo;
using namespace DirectX;
using namespace std;
using namespace Imf;
//using namespace IMATH_NAMESPACE;

ExrFileIOSample::ExrFileIOSample()
{
	m_exrTextureHandle = 0;
}

ExrFileIOSample::~ExrFileIOSample()
{

}

void ExrFileIOSample::init()
{
	ScratchImage image;
	TexMetadata metadata;

	std::string exrPath = "..\\bin\\Assets\\Texture\\studio_03.exr";
	std::wstring wExrPath;
	StringToWString(exrPath, wExrPath);
	HRESULT hr = LoadFromEXRFile(wExrPath.c_str(), &metadata, image);

	Texture2dConfigDX11 tex2dConfig;
	tex2dConfig.SetWidth(image.GetImages()->width);
	tex2dConfig.SetHeight(image.GetImages()->height);
	tex2dConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE);
	tex2dConfig.SetFormat(image.GetImages()->format);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = image.GetImages()->pixels;
	data.SysMemPitch = image.GetImages()->rowPitch;
	data.SysMemSlicePitch = image.GetImages()->slicePitch;
	//创建cs写入的uav
	m_exrTextureHandle = TextureDX11ResourceFactory::getInstance().createTexture2D("studio_03", tex2dConfig,&data);

	combineMipmapToTexture();
}

void writeRgba1(const char fileName[],
	const Imf::Rgba *pixels,
	int width,
	int height)
{
	Imf::RgbaOutputFile file(fileName, width, height, Imf::WRITE_RGBA); // 1
	file.setFrameBuffer(pixels, 1, width); // 2
	file.writePixels(height); // 3
}

void writeTiledRgbaONE1(const char fileName[],
	const Rgba *pixels,
	int width, int height,
	int tileWidth, int tileHeight)
{
	TiledRgbaOutputFile out(fileName,
		width, height, // image size
		tileWidth, tileHeight, // tile size
		ONE_LEVEL, // level mode
		ROUND_DOWN, // rounding mode
		WRITE_RGBA); // channels in file // 1
	out.setFrameBuffer(pixels, 1, width); // 2
	out.writeTiles(0, out.numXTiles() - 1, 0, out.numYTiles() - 1); // 3
}
void
writeTiledRgbaMIP1(const char fileName[],
	int width, int height,
	int tileWidth, int tileHeight, ScratchImage* imageArray,int mipmapCount)
{
	//
	// Write a tiled image with mipmap levels using an image-sized framebuffer.
	//

	TiledRgbaOutputFile out(fileName,
		width, height,		// image size
		tileWidth, tileHeight,	// tile size
		MIPMAP_LEVELS,		// level mode
		ROUND_DOWN,		// rounding mode
		WRITE_RGBA);		// channels in file

	//Array2D<Rgba> pixels(height, width);
	Rgba* tempBuffer = new Rgba[height * width];

	out.setFrameBuffer(tempBuffer, 1, width);
	int mc = out.numLevels();
	for (int level = 0; level < out.numLevels(); ++level)
	{
		/*drawImage4(pixels,
			out.levelWidth(level), out.levelHeight(level),
			0, out.levelWidth(level),
			0, out.levelHeight(level),
			level, level);*/
		ScratchImage& srcImage = imageArray[level];
		const uint8_t* srcData = srcImage.GetImages()->pixels;

		memcpy(tempBuffer, srcData, srcImage.GetImages()->height * srcImage.GetImages()->rowPitch);

		int xt = out.numXTiles(level);
		int yt = out.numYTiles(level);

		out.writeTiles(0, out.numXTiles(level) - 1,
			0, out.numYTiles(level) - 1,
			level);
	}
}

void ExrFileIOSample::combineMipmapToTexture()
{
	//0-11 mipmap
	ScratchImage imageArray[12];
	TexMetadata metadataArray[12];

	for (int i = 0; i < 12; ++i)
	{
		wstringstream ss;
		ss << "..\\bin\\Assets\\Texture\\evnTex_" << i << ".exr";
		/*std::string exrPath = "..\\bin\\Assets\\Texture\\studio_03.exr";
		std::wstring wExrPath;
		StringToWString(exrPath, wExrPath);*/
		HRESULT hr = LoadFromEXRFile(ss.str().c_str(), &metadataArray[i], imageArray[i]);
		if (hr != S_OK)
			return;
	}


	ScratchImage desTex;
	const Image* level0Image = imageArray[0].GetImages();
	desTex.Initialize2D(level0Image->format, level0Image->width, level0Image->height, 1, 12);

	int level0Width = level0Image->width;
	int tileWidth = 0;
	//fill des exr
	for (int i = 0; i < 12; ++i)
	{
		tileWidth += (level0Width >> i);
		Image& desImage = (Image&)(desTex.GetImages()[i]);
		const Image* srcImage = imageArray[i].GetImages();

		if(desImage.height * desImage.rowPitch != srcImage->height * srcImage->rowPitch)
			continue;

		memcpy(desImage.pixels, srcImage->pixels, desImage.height * desImage.rowPitch);
	}

	//这个函数只能存image？不能存mipmap数据?
	/*wstringstream ss;
	ss << "h:\\textExr.exr";
	SaveToEXRFile(desTex.GetImages()[1],ss.str().c_str());*/

	//
	Image& saveImage = (Image&)desTex.GetImages()[1];
	//writeRgba1("h:\\textExr2.exr", (Imf::Rgba *)saveImage.pixels, saveImage.width, saveImage.height);
	//writeTiledRgbaONE1("h:\\textExr3.exr", (Imf::Rgba *)saveImage.pixels, saveImage.width, saveImage.height, saveImage.width, saveImage.height);

	//tile width = level0 width + level1 width ......
	writeTiledRgbaMIP1("h:\\textExr4.exr", level0Image->width, level0Image->height, 100, 75, imageArray, 12);
}



//void loadExr(ScratchImage& image, TexMetadata& data,std::wstring path)
//{
//
//}

void ExrFileIOSample::onGUI()
{
	ImGui::SetNextWindowPos(ImVec2(100, 300));
	static bool g_overLayShow = true;
	ImGui::SetNextWindowPos(ImVec2(100, 300));
	if (!ImGui::Begin("ExrTexture", &g_overLayShow, ImVec2(500, 800), 0.7f, ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::End();
		return;
	}
	
	{
		Texture2dDX11* srcTex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_exrTextureHandle);

		ImGui::Image(srcTex2d->getShaderResourceView(), ImVec2(200, 200));
	}


	ImGui::End();
}