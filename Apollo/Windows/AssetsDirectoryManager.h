#pragma once

#include "Singleton.h"

namespace Apollo
{
	class DirectoryBaseNode
	{
	public:

		DirectoryBaseNode(const std::string& path);
		size_t			m_id;
		std::string		m_name;
		std::string m_path;

		static size_t	s_nodeID;
	};

	class FileNode : public DirectoryBaseNode
	{
	public:
		FileNode(const std::string& path);
	};

	class DirectoryNode : public DirectoryBaseNode
	{
	public:
		friend class AssetsDirectoryViewUI;

		DirectoryNode(const std::string& path);
		~DirectoryNode();

		void	addFile(const std::string& filePath);

		DirectoryNode* addDirectory(const std::string& filePath);

	protected:

		std::list<FileNode*>			m_fileList;
		std::list<DirectoryNode*>	m_subDirectoryList;		
	};

//	typedef std::shared_ptr<DirectoryNode*> DirectoryNodePtr;

	class AssetsDirectoryManager : public SingletonEx<AssetsDirectoryManager>
	{
	public:

		AssetsDirectoryManager();
		~AssetsDirectoryManager();

		void init(const std::string& path);

		DirectoryNode*		getRootNode() { return m_directoryRootNode;}

	protected:

		void getDirectoryInfo(const std::string& path, DirectoryNode* node);

	protected:

		DirectoryNode*		m_directoryRootNode;
	private:
	};
}
