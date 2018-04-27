#include "stdafx.h"
#include <io.h>
#include <iostream>
#include "AssetsDirectoryManager.h"

using namespace std;
using namespace Apollo;

void AssetsDirectoryManager::getDirectoryInfo(const std::string& path, DirectoryNodePtr& node)
{
	//�ļ����    
	long   hFile = 0;
	//�ļ���Ϣ    
	struct _finddata_t fileinfo;  //�����õ��ļ���Ϣ��ȡ�ṹ  
	string p;  //string�������˼��һ����ֵ����:assign()���кܶ����ذ汾  
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //�Ƚ��ļ������Ƿ����ļ���  
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
		} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1  
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