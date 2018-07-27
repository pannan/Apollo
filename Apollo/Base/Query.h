#pragma once

namespace Apollo
{
	class Query
	{
	public:
		enum class QueryType
		{
			Timer,
			CountSamples,
			CountSamplesPredicate,
			CountPrimitives,
			CountTransformFeedbackPrimitives,
		};

		struct QueryResult
		{
			union
			{
				double   ElapsedTime;                   // Valid for QueryType::Timer queries. Returns the elapsed time in seconds between Query::Begin and Query::End.
				uint64_t PrimitivesGenerated;           // Valid for QueryType::CountPrimitives. Returns the number of primitives generated between Query::Begin and Query::End.
				uint64_t TransformFeedbackPrimitives;   // Valid for QueryType::CountTransformFeedbackPrimitives. Returns the number of primtives written to stream out or transform feedback buffers.
				uint64_t NumSamples;                    // Valid for QueryType::CountSamples. Returns the number of samples written by the fragment shader between Query::Begin and Query::End.
				bool     AnySamples;                    // Valid for QueryType::CountSamplesPredicate. Returns true if any samples were written by the fragment shader between Query::Begin and Query::End.
			};
			// Are the results of the query valid?
			// You should check this before using the value.
			bool IsValid;
		};

		/**
		* Begin a GPU query.
		*/
		virtual void Begin(int64_t frame = 0L) = 0;

		/**
		* End the GPU query.
		*/
		virtual void End(int64_t frame = 0L) = 0;

		/**
		* Check to see if the query results are available.
		*/
		virtual bool QueryResultAvailable(int64_t frame = 0L) = 0;

		/**
		* Retrieve the query results from the query object.
		* The type of result returned is determined by the query type.
		* Retrieving the results could result in a GPU stall if you request
		* the current frame's result because the CPU has to wait until all of the
		* GPU commands that were enqued on the GPU before Query::End was called.
		* You should instead request the query results from 2 or 3 frames previous
		* (if you can wait a few frames for the result). If you absolutely need the current frame's result,
		* you can query the current frames result but will almost always result in a GPU stall.
		* IMPORTANT: Be sure to check the QueryResult::IsValid flag before using the result.
		从查询对象接受查询结果
		返回类型由查询类型决定
		接受结果可能导致GPU停转，如果你请求当前帧的结果，因为CPU必须等待所有在Query::End之前的GPU命令全部执行，然后执行Query::End才能得到结果。
		你可以转而请求2,3帧之前的查询结果（如果你能等待结果几帧）。
		如果你必须要当前帧的结果，你可以查询当前帧的结果但是几乎总是导致GPU停转。
		总要：使用结果前要检查QueryResult::IsValid
		*/
		virtual QueryResult GetQueryResult(int64_t frame = 0L) = 0;

		/**
		* GPU queries can generally be multi-buffered to reduce
		* GPU stalls when getting the query result. If the implementation provides
		* multi-buffered queries this method will return the number of buffered query
		* results that are available.
		*/
		virtual uint8_t GetBufferCount() const = 0;
	};
}
