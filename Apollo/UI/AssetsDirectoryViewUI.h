#pragma once

#include "Singleton.h"

namespace Apollo
{
	class DirectoryNode;
	class DirectoryBaseNode;

	class AssetsDirectoryViewUI : public SingletonEx<AssetsDirectoryViewUI>
	{
	public:

		AssetsDirectoryViewUI();
		~AssetsDirectoryViewUI();

		void			render(int w,int h);

		DirectoryBaseNode*		getSelectNode() { return m_selectNode; }

	protected:

		void			drawNode(DirectoryNode* node);

	private:

			DirectoryBaseNode*		m_selectNode;
	};
}
