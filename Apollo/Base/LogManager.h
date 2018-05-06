#pragma once

#include "Singleton.h"

namespace Apollo
{
	class LogManager : public SingletonEx<LogManager>
	{
	public:

		friend class LogUI;

		void		log(std::string logInfo);

	protected:

		std::string		m_logData;

	private:
	};
}
