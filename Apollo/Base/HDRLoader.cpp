#include "stdafx.h"
#include "HDRLoader.h"

NAME_SPACE_BEGIN_APOLLO

HDRLoader::HDRLoader() : m_width(0),m_height(0),m_hdrBuffer(nullptr)
{

}

HDRLoader::~HDRLoader()
{
	SAFE_DELETE_ARRAY(m_hdrBuffer);
}

bool HDRLoader::load(std::string filename)
{
	std::ifstream reading_hdr(filename, std::ios::in);
	if (!reading_hdr) 
	{
		std::cerr << "failed reading hdr!(file is not found)" << std::endl;
		return false;
	}

	std::string header_line;

	std::cerr << "reading " << filename << "..." << std::endl;

	int width = 0;
	int height = 0;
	int count_LF = 0;//计算换行符
	while (!reading_hdr.eof()) 
	{
		std::getline(reading_hdr, header_line);
		count_LF++;

		if (header_line[0] == '-' && header_line[1] == 'Y') 
		{
			std::vector<std::string> v;/////////////////
			std::stringstream ss(header_line);//按空间分割
			std::string buf;
			while (std::getline(ss, buf, ' ')) 
			{
				v.push_back(buf);
			}/////////////////////

			width = std::stoi(v[3]);
			height = std::stoi(v[1]);
			break;
		}
	}

	reading_hdr.close();
	if (width <= 0 || height <= 0)
	{
		std::cerr << "failed reading hdr!(header is wrong)" << std::endl;
		return false;
	}
	std::cerr << "width = " << width << " height = " << height << std::endl;

	//如果buffer大小需要改变
	if (width * height != m_width * m_height)
	{
		SAFE_DELETE_ARRAY(m_hdrBuffer);
		m_hdrBuffer = new float[width * height * 4]; //rgba
		m_width = width;
		m_height = height;
	}

	reading_hdr.open(filename, std::ios::binary);//以二进制形式打开

	if (!reading_hdr) 
	{
		std::cerr << "failed reading hdr!(file is not found)" << std::endl;
		return false;
	}

	int count_LF_ = 0;
	while (count_LF_ < count_LF) 
	{
		char c;
		reading_hdr.read(&c, 1);
		if (c == '\n') {
			count_LF_++;
		}
	}//忽略标题

	for (int i = 0; i < height; i++) 
	{
		std::vector<unsigned char> r, g, b, e;

		char c;
		reading_hdr.read(&c, 1);
		reading_hdr.read(&c, 1);//忽略神奇的数字

		reading_hdr.read(&c, 1);
		//int w = (c & 0xff);
		//w <<= 8;
		reading_hdr.read(&c, 1);//忽略宽度的规格
		//w += (c & 0xff);

		if (!load_bin(reading_hdr, r)) std::cerr << "faied" << std::endl;
		if (!load_bin(reading_hdr, g)) std::cerr << "faied" << std::endl;
		if (!load_bin(reading_hdr, b)) std::cerr << "faied" << std::endl;
		if (!load_bin(reading_hdr, e)) std::cerr << "faied" << std::endl;

		for (int j = 0; j < width; j++) 
		{
			float fr = 0.005 * r[j] * std::pow(2, e[j] - 128);
			float fg = 0.005 * g[j] * std::pow(2, e[j] - 128);
			float fb = 0.005 * b[j] * std::pow(2, e[j] - 128);

			m_hdrBuffer[i * width * 4 + j * 4] = fr;
			m_hdrBuffer[i * width * 4 + j * 4 + 1] = fg;
			m_hdrBuffer[i * width * 4 + j * 4 + 2] = fb;
			m_hdrBuffer[i * width * 4 + j * 4 + 3] = 1.0f;
		}
	}

	return true;
}

bool HDRLoader::load_bin(std::ifstream &reading_hdr, std::vector<unsigned char> &v) 
{

	while (v.size() < m_width) 
	{
		if (reading_hdr.eof()) 
			return false;

		char c;
		reading_hdr.read(&c, 1);

		unsigned char uc = (c & 0xff);//将c解释为无符号

		if (uc <= 128)
		{
			for (int j = 0; j < uc; j++)
			{
				if (reading_hdr.eof()) return false;
				char c;
				reading_hdr.read(&c, 1);
				unsigned char num = (c & 0xff);
				v.push_back(num);
			}
		}
		else 
		{
			if (reading_hdr.eof()) return false;
			char c;
			reading_hdr.read(&c, 1);
			unsigned char num = (c & 0xff);//将c解释为无符号
			for (int j = 0; j < uc - 128; j++)
			{
				v.push_back(num);
			}
		}
	}
	return true;
}

NAME_SPACE_END