#pragma once

namespace Apollo
{
	bool StringToWString(const std::string &str, std::wstring &wstr);

	bool WStringToString(const std::wstring &wstr, std::string &str);

	std::string		stringToUTF8(std::string str);
}
