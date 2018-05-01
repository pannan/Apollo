#include "stdafx.h"
#include <io.h>
#include <iostream>
#include "AssetsDirectoryManager.h"

using namespace std;
using namespace Apollo;

size_t DirectoryNode::s_nodeID = 0;

DirectoryNode::DirectoryNode(const std::string& path) : m_path(path)
{
	m_id = s_nodeID++;
}

DirectoryNode::~DirectoryNode()
{
	for each (DirectoryNode* var in m_subDirectoryList)
	{
		if (var)
			delete var;
	}
}

void DirectoryNode::addFile(const std::string& filePath)
{
	m_fileList.push_back(filePath);
}

DirectoryNode* DirectoryNode::addDirectory(const std::string& filePath)
{
	DirectoryNode* node = new DirectoryNode(filePath);
	m_subDirectoryList.push_back(node);
	return node;
}

//////////////////////////////////////////////////////////////////////////

AssetsDirectoryManager::AssetsDirectoryManager() : m_directoryRootNode(nullptr)
{

}

AssetsDirectoryManager::~AssetsDirectoryManager()
{
	SAFE_DELETE(m_directoryRootNode);
}

void AssetsDirectoryManager::getDirectoryInfo(const std::string& path, DirectoryNode* node)
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
					string pathName = p.assign(path).append("\\").append(fileinfo.name);
					DirectoryNode* subNode = node->addDirectory(pathName);
					getDirectoryInfo(pathName, subNode);
				}
			}
			else
			{
				string filePath = p.assign(path).append("\\").append(fileinfo.name);
				node->addFile(filePath);
			}
		} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1  
		_findclose(hFile);
	}
}

void AssetsDirectoryManager::init(const std::string& path)
{
	if (m_directoryRootNode == nullptr)
	{
		m_directoryRootNode = new DirectoryNode(path);
	}
	else
	{
		delete m_directoryRootNode;
		m_directoryRootNode = new DirectoryNode(path);
	}
	
	getDirectoryInfo(path,m_directoryRootNode);
}