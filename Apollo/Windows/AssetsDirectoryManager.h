#pragma once

#include "Singleton.h"

namespace Apollo
{
	struct DirectoryNode
	{
		std::list<std::string>			m_fileList;
		std::list<std::shared_ptr<DirectoryNode*>>	m_subDirectoryList;
		std::string		m_path;
	};

	typedef std::shared_ptr<DirectoryNode*> DirectoryNodePtr;

	class AssetsDirectoryManager : public SingletonEx<AssetsDirectoryManager>
	{
	public:

		void init(const std::string& path);

	protected:

		void getDirectoryInfo(const std::string& path, DirectoryNodePtr& node);

	protected:

		DirectoryNodePtr		m_directoryRootNode;
	private:
	};
}
