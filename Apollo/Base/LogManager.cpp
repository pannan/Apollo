#include "stdafx.h"
#include "LogManager.h"

using namespace Apollo;

void  LogManager::log(std::string logInfo)
{
	m_logData += logInfo + "\n";
}