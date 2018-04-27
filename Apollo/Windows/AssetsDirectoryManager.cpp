#include "stdafx.h"
#include <io.h>
#include <iostream>
#include "AssetsDirectoryManager.h"

using namespace std;
using namespace Apollo;

void AssetsDirectoryManager::getDirectoryInfo(const std::string& path, DirectoryNodePtr& node)
{
	//文件句柄    
	long   hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;  //很少用的文件信息读取结构  
	string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本  
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //比较文件类型是否是文件夹  
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					getDirectoryInfo(p.assign(path).append("\\").append(fileinfo.name));
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1  
		_findclose(hFile);
	}
}

void AssetsDirectoryManager::init(const std::string& path)
{
	if (m_directoryRootNode == nullptr)
	{
		m_directoryRootNode = new DirectoryNode;
	}
	else
	{
		m_directoryRootNode;
	}
	
	getDirectoryInfo(path,m_directoryRootNode);
}