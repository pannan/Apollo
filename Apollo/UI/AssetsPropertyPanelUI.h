#pragma once

namespace Apollo
{
	class ISelectPropertyUI;

	class AssetsPropertyPanelUI
	{
	public:

		void registerAssetsPropertyUI(std::string type,ISelectPropertyUI* ui);

	protected:

		typedef std::hash_map<std::string, ISelectPropertyUI*>		PropertyUIMap;
		PropertyUIMap		m_perportyMap;

	private:
	};
}
