#include "stdafx.h"
#include "MaterialParse.h"
#include <iostream>
#include <fstream>
#include "LogManager.h"

using namespace Apollo;
using namespace std;


Parse_Result	MaterialParse::parse(std::string path,std::string name)
{
	ifstream fread;
	fread.open(path);

	if (!fread.is_open())
	{
		LogManager::getInstance().log("Material:" + path + "open false!");
		return PR_FILE_OPEN_FALSE;
	}

	string buffer;
	string allBuffer;

	while (!fread.eof())
	{
		fread >> buffer;
		allBuffer += buffer;
	}
	const char* ptr = allBuffer.c_str();
	m_endPos = allBuffer.size();
	m_readPos = 0;
	m_ptr = (char*)allBuffer.c_str();
	m_chunkList.clear();

	while (!isEnd())
	{
		Parse_Result res = skipWhiteSpace();
		if (res != PR_OK)
			return res;

		ChunkData chunkData;
		res = findStringChunk(chunkData.name);
		if (res != PR_OK)
			return res;
		
		parseChunk(chunkData);

		m_chunkList.push_back(chunkData);
	}

	return PR_OK;
}

Parse_Result MaterialParse::parseChunk(ChunkData& chunkData)
{
	//find '{'
	Parse_Result res = hitChar('{');
	if (res != PR_OK)
		return res;

	while (true)
	{
		string keyStr;
		//find key
		res = findStringChunk(keyStr);
		if (res != PR_OK)
			return res;

		//find ":"
		res = hitChar(':');
		if (res != PR_OK)
			return res;

		//fin value
		string valueStr;
		res = findStringChunk(valueStr);
		if (res != PR_OK)
			return res;

		chunkData.elementList.push_back(KeyValueChunk(keyStr, valueStr));

		//找到下一个字符，如果是'}' parse结束
		skipWhiteSpace();
		char c = getChar();
		if (c == '}')
		{
			if(!isEnd())
				moveNext();
			return PR_OK;
		}				
	}	
}

Parse_Result MaterialParse::skipWhiteSpace()
{
	do 
	{
		char c = getChar();
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
			moveNext();
		else
			return PR_OK;

	} while (!isEnd());	

	return PR_OK;
}

Parse_Result MaterialParse::hitChar(char hitChar)
{
	while (true)
	{
		if (isEnd())
			return PR_NOT_HIT_CHAR;

		char c = getChar();
		moveNext();

		if (c == hitChar)
			return PR_OK;
	}
}

Parse_Result MaterialParse::findStringChunk(std::string& chunkName)
{
	enum ParseChunkState
	{
		PCS_LOOK_FOR_CHUNK,
		PCD_GET_CHUNK_NAME,
		PCD_END
	};

	ParseChunkState state = PCS_LOOK_FOR_CHUNK;

	while (state != PCD_END)
	{
		if (isEnd())
			return PR_ERROR_END;

		char c = getChar();
		moveNext();

		if (state == PCS_LOOK_FOR_CHUNK)
		{
			if (c == '"')
				state = PCD_GET_CHUNK_NAME;
		}
		else if (state == PCD_GET_CHUNK_NAME)
		{
			if (c != '"')
				chunkName += c;
			else
				state = PCD_END;
		}		
	}

	return PR_OK;
}

