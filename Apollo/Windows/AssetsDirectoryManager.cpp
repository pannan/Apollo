#include "stdafx.h"
#include <io.h>
#include <iostream>
#include "AssetsDirectoryManager.h"
#include "LogManager.h"
#include "MaterialParse.h"
#include "ResourceManager.h"
using namespace std;
using namespace Apollo;

size_t DirectoryBaseNode::s_nodeID = 0;

DirectoryBaseNode::DirectoryBaseNode(const std::string& path) : m_path(path)
{
	m_id = s_nodeID++;

	//找到文件名
	int pos = m_path.find_last_of("\\");
	if (pos != std::string::npos)
	{
		m_name = m_path.substr(pos + 1, m_path.length() - pos);
	}
	else
		m_name = m_path;

	m_suffix = "";
	if (m_name.empty())
		return;
	
	//找后缀名
	pos = m_name.find_last_of(".");
	if (pos != std::string::npos)
	{
		m_suffix = m_name.substr(pos + 1, m_name.length() - pos);
		//LogManager::getInstance().log("Load file:" + m_name);

		if (m_suffix == "material")
		{
			MaterialParse::getInstance().parse(path, m_name);
		}

		//统一到assets目录分析完再创建
//		ResourceManager::getInstance().createResource(path, m_name, m_suffix);
	}

}

FileNode::FileNode(const std::string& path) : DirectoryBaseNode(path)
{

}

DirectoryNode::DirectoryNode(const std::string& path) : DirectoryBaseNode(path)
{
}

DirectoryNode::~DirectoryNode()
{
	for each (FileNode* var in m_fileList)
	{
		if (var)
			delete var;
	}

	for each (DirectoryNode* var in m_subDirectoryList)
	{
		if (var)
			delete var;
	}
}

void DirectoryNode::addFile(const std::string& filePath)
{
	FileNode* node = new FileNode(filePath);
	m_fileList.push_back(node);
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
		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1  
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