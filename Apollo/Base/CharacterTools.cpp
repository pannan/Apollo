#include "stdafx.h"
#include "CharacterTools.h"

using namespace std;

namespace Apollo
{
	bool StringToWString(const std::string &str, std::wstring &wstr)
	{
		int nLen = (int)str.length();
		wstr.resize(nLen, L' ');
		int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), nLen, (LPWSTR)wstr.c_str(), nLen);

		if (nResult == 0)
		{
			return false;
		}
		return true;
	}

	bool WStringToString(const std::wstring &wstr, std::string &str)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		str.resize(size_needed);
		int nResult = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
		if (nResult == 0)
			return false;

		return true;
	}

	static inline int textCharToUtf8(char* buf, int buf_size, unsigned int c)
	{
		if (c < 0x80)
		{
			buf[0] = (char)c;
			return 1;
		}
		if (c < 0x800)
		{
			if (buf_size < 2) return 0;
			buf[0] = (char)(0xc0 + (c >> 6));
			buf[1] = (char)(0x80 + (c & 0x3f));
			return 2;
		}
		if (c >= 0xdc00 && c < 0xe000)
		{
			return 0;
		}
		if (c >= 0xd800 && c < 0xdc00)
		{
			if (buf_size < 4) return 0;
			buf[0] = (char)(0xf0 + (c >> 18));
			buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
			buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
			buf[3] = (char)(0x80 + ((c) & 0x3f));
			return 4;
		}
		//else if (c < 0x10000)
		{
			if (buf_size < 3) return 0;
			buf[0] = (char)(0xe0 + (c >> 12));
			buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
			buf[2] = (char)(0x80 + ((c) & 0x3f));
			return 3;
		}
	}

	int textStrToUtf8(char* buf, int buf_size, const ImWchar* in_text, const ImWchar* in_text_end)
	{
		char* buf_out = buf;
		const char* buf_end = buf + buf_size;
		while (buf_out < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text)
		{
			unsigned int c = (unsigned int)(*in_text++);
			if (c < 0x80)
				*buf_out++ = (char)c;
			else
				buf_out += textCharToUtf8(buf_out, (int)(buf_end - buf_out - 1), c);
		}
		*buf_out = 0;
		return (int)(buf_out - buf);
	}

	std::string stringToUTF8(std::string str)
	{
		std::wstring wss;
		StringToWString(str, wss);
		std::string finalStr;
		finalStr.resize(wss.length() * 2);
		textStrToUtf8((char*)finalStr.c_str(), (int)str.length() * 2, (ImWchar*)wss.c_str(), NULL);

		return finalStr;
	}
}


