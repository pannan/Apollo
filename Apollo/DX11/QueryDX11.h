#pragma once

#include "Query.h"

namespace Apollo
{
	class QueryDX11 : public Query
	{
	public:

		QueryDX11(QueryType queryType, uint8_t numBuffers);
		virtual ~QueryDX11();

		virtual void Begin(int64_t frame = 0L);
		virtual void End(int64_t frame = 0L);
		virtual bool QueryResultAvailable(int64_t frame = 0L);
		virtual QueryResult GetQueryResult(int64_t frame = 0L);
		virtual uint8_t GetBufferCount() const;

	protected:

	private:
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		QueryBufferComPtr m_DisjointQueries;
		// For timer queries, we need 2 sets of buffered queries.
		QueryBufferComPtr m_Queries[2];

		QueryType m_QueryType;
		// How many queries will be used to prevent stalling the GPU.
		uint8_t m_NumBuffers;
	};
}
