#pragma once

#include <fstream>
#include <iostream>
#include <string>

// TGA Specification - http://netghost.narod.ru/gff/vendspec/tga/tga.txt
struct TGAHEADER
{
	// Length of id string
	char	idLength;

	// Image storage info
	char	colourMapType;
	char	imageType;

	// Colour Map
	short	firstEntry;
	short	numEntries;
	char	bitsPerEntry;

	// Image description
	short	xOrigin;
	short	yOrigin;
	short	width;
	short	height;
	char	bitsPerPixel;
	char	descriptor;
};

enum
{
	TGA_UNSUPPORTED = 1,
	TGA_NO_IMAGE = 2,
	TGA_MAP = 4,
	TGA_RGB = 8,
	TGA_BW = 16,
	TGA_RLE = 32
};

// Generic header that image data will be stored in
// regardless of image format
// Fields will be added as required
struct ImageHeader
{
	int width;
	int height;
	int depth;
	char compentCountPrePixel;//每个像素有几个分量，rgba,rga....
	char bytesPreCompent;//每个compent的byes数,1(rgba32),2(half),4(float)
	//char bitsPerPixel; //bitsPerPixel = compentCountPrePixel * bytesPreCompent
};

enum TGA_LOADER_RES
{
	IMAGE_LOADED,
	_ERROR,
	UNSUPPORTED_IMAGE_FORMAT
};

// Currently only supports TGA
class TGAImageExt
{
public:

	TGAImageExt()
	{
		pixelData = nullptr;
	}

	~TGAImageExt()
	{
		SAFE_DELETE_ARRAY(pixelData);
	}

	ImageHeader header;
	char*	pixelData;

	int loadImage(const std::string& fileName);

	int loadImageArray(const std::vector<std::string> texArrayName);

	void loadByDirectTex(std::string path);

	void	save(const std::string& savePath);

protected:

	int getCurrentLoadByteCount();

private:
	// loadImage() will handle the call to format specifc methods
	int loadTGA(const std::string& fileName);
};