#pragma once

#include "Singleton.h"

namespace Apollo
{
	class DirectoryNode
	{
	public:
		friend class AssetsDirectoryViewUI;

		DirectoryNode(const std::string& path);
		~DirectoryNode();

		void	addFile(const std::string& filePath);

		DirectoryNode* addDirectory(const std::string& filePath);

	protected:

		std::list<std::string>			m_fileList;
		std::list<DirectoryNode*>	m_subDirectoryList;
		std::string		m_path;
		size_t			m_id;

		static size_t	s_nodeID;
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
