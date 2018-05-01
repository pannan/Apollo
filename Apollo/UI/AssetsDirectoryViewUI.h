#pragma once

namespace Apollo
{
	class DirectoryNode;
	class AssetsDirectoryViewUI
	{
	public:

		AssetsDirectoryViewUI();
		~AssetsDirectoryViewUI();

		void			render(int w,int h);

	protected:

		void			drawNode(DirectoryNode* node);

	private:

		DirectoryNode*		m_selectNode;
	};
}
