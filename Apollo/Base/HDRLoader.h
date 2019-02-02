#pragma once

NAME_SPACE_BEGIN_APOLLO

class HDRLoader
{
public:

	HDRLoader();

	~HDRLoader();

	bool		load(std::string filename);

	int		getWidth() { return m_width; }

	int		getHeight() { return m_height; }

	float*	getRGBAFloatBuffer() { return m_hdrBuffer; }

protected:

	bool load_bin(std::ifstream &reading_hdr, std::vector<unsigned char> &v);

protected:

	int				m_width;
	int				m_height;

	float*			m_hdrBuffer;
private:
};

NAME_SPACE_END