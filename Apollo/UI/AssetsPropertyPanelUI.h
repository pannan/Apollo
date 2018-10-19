#pragma once

#include "Singleton.h"

namespace Apollo
{
	class ISelectPropertyUI;

	class AssetsPropertyPanelUI : public SingletonEx<AssetsPropertyPanelUI>
	{
	public:

		void registerAssetsPropertyUI(std::string type,ISelectPropertyUI* ui);

		void unRegisterAssetsPropertyUI(std::string type, ISelectPropertyUI* ui);

		void	render(int w,int h);

	protected:

		ISelectPropertyUI*		getAssetsPropertyUI(const std::string& type);

	protected:

#ifdef DEBUG
		typedef std::hash_map<std::string, ISelectPropertyUI*>		PropertyUIMap;
#else
		typedef std::unordered_map<std::string, ISelectPropertyUI*>		PropertyUIMap;
#endif // DEBUG

		
		PropertyUIMap		m_perportyMap;

	private:
	};
}
