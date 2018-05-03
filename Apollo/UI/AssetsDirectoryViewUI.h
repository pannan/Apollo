#pragma once

namespace Apollo
{
	class DirectoryNode;
	class DirectoryBaseNode;

	class AssetsDirectoryViewUI
	{
	public:

		AssetsDirectoryViewUI();
		~AssetsDirectoryViewUI();

		void			render(int w,int h);

	protected:

		void			drawNode(DirectoryNode* node);

	private:

			DirectoryBaseNode*		m_selectNode;
	};
}
