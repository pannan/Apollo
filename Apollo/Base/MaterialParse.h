#pragma once

#include "Singleton.h"

namespace Apollo
{
	struct ShaderChunk
	{
		std::string		source;
		std::string		entry;
	};

	struct  KeyValueChunk 
	{
		KeyValueChunk(const std::string& k,const std::string& v) : key(k),value(v){}
		std::string key;
		std::string	value;
	};

	struct ChunkData
	{
		std::string									name;
		std::vector<KeyValueChunk>	elementList;
	};

	struct MaterialChunk
	{
		/*ShaderChunk		m_vs;
		ShaderChunk		m_ps;		*/
		std::vector<ChunkData>		m_chunkList;
	};

	enum Parse_Result
	{
		PR_OK,
		PR_FILE_OPEN_FALSE,
		PR_ERROR_END,
		PR_NOT_HIT_CHAR
	};

	class MaterialParse : public SingletonEx<MaterialParse>
	{
	public:

		Parse_Result				parse(std::string path,std::string name);

	protected:

		//跳过空白
		Parse_Result				skipWhiteSpace();

		Parse_Result				findStringChunk(std::string& chunkName);

		Parse_Result				hitChar(char hitChar);

		Parse_Result				parseChunk(ChunkData& chunkData);

		inline	bool isEnd() {return m_readPos >= m_endPos; }

		//读取当前位置字符
		char		getChar() { return m_ptr[m_readPos]; }

		//移动readpos
		void		moveNext() { ++m_readPos; }

	private:

		size_t			m_endPos;
		size_t			m_readPos;
		char*			m_ptr;

		std::vector<MaterialChunk>	m_materialList;
	};
}
