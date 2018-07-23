
struct Triangle
{
	uint index[3];		//三个顶点索引
	float3 normal;
};

struct ShareVertex
{
	float3 normal;
	uint    shareCount;
};

Texture2D<float4>					HeightMap : register(t0);

StructuredBuffer<float3>		TerrainVertexBuffer;

StructuredBuffer<uint>				IndexBuffer;

RWStructuredBuffer<Triangle> TriangleBuffer;


/*
globallycoherent保证这个数据在整个gpu的同步，如果没有，只会在group里同步
*/
globallycoherent RWStructuredBuffer<ShareVertex>		ShareVertexBuffer;

RWStructuredBuffer<float3>					VertexNormalBuffer;

float fetchHeightMap(uint index)
{
	uint x = index % 1024;
	uint y = index / 1024;
	return HeightMap.Load(float3(x,y,0)).x;
}

#define triangle_size_x 2
#define triangle_size_y 1
[numthreads(triangle_size_x, triangle_size_y, 1)]

/*
这里是计算三角形的normal，写入TriangleBuffer，所以这里的索引是三角形的triangleindex
通过triangleindex * 3得到indexbuffer里对应的顶点索引，然后去vertexbuffer里找顶点数据
这里纹理为128x128，所以顶点为128x128，三角形为127 * 127 * 2
所以我们发出127x127个线程group
Dispatch(127,127,1)
每个group里有两2x1=2个线程
triangle_size_x 2
triangle_size_y 1
*/
void CS_ComputeTriangleNormal( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, 
														uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	uint indexX = GroupID.x * triangle_size_x + GroupThreadID.x;
	uint indexY = GroupID.y * triangle_size_y + GroupThreadID.y;
	uint triangleIndex = indexY * (1023 * 2) + indexX;

	//得到顶点索引
	uint baseIndex = triangleIndex * 3;
	uint index0 = IndexBuffer[baseIndex];
	uint index1 = IndexBuffer[baseIndex + 1];
	uint index2 = IndexBuffer[baseIndex + 2];

	//下面需要把index映射到纹理坐标上来读取heightmap
	float y0 = fetchHeightMap(index0) * 50;
	float y1 = fetchHeightMap(index1) * 50;
	float y2 = fetchHeightMap(index2) * 50;

	float3 vertex0 = TerrainVertexBuffer[index0];
	float3 vertex1 = TerrainVertexBuffer[index1];
	float3 vertex2 = TerrainVertexBuffer[index2];
	vertex0.y = y0;
	vertex1.y = y1;
	vertex2.y = y2;

	float3 vec0 = normalize(vertex1 - vertex0);
	float3 vec1 = normalize(vertex2 - vertex0);
	float3 normal = normalize(cross(vec0, vec1));

	Triangle tri;
	tri.normal = normal;
	tri.index[0] = index0;
	tri.index[1] = index1;
	tri.index[2] = index2;

	TriangleBuffer[triangleIndex] = tri;
}

/*
这里是计算sharevertex信息，顶点数量为128x128
但是我们需要利用trianglebuffer的数据来计算，所以我们还是要以
trianglebuffer的尺度来分发线程
三角形为127 * 127 * 2
所以我们发出127x127个线程group
Dispatch(127,127,1)
每个group里有两2x1=2个线程
triangle_size_x 2
triangle_size_y 1
*/

//初始化sharevertex
[numthreads(32, 32, 1)]
void CS_InitShareVertex(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID,
	uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex)
{
	uint indexX = GroupID.x * 32 + GroupThreadID.x;
	uint indexY = GroupID.y * 32 + GroupThreadID.y;
	uint vertexIndex = indexY * 1024 + indexX;

	ShareVertexBuffer[vertexIndex].normal = float3(0,0,0);
	ShareVertexBuffer[vertexIndex].shareCount = 0;
}

#define triangle_size_x 2
#define triangle_size_y 1
[numthreads(triangle_size_x, triangle_size_y, 1)]
void CS_ComputeShareVertex(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID,
	uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex)
{
	uint indexX = GroupID.x * triangle_size_x + GroupThreadID.x;
	uint indexY = GroupID.y * triangle_size_y + GroupThreadID.y;
	uint triangleIndex = indexY * (1023 * 2) + indexX;

	Triangle tri = TriangleBuffer[triangleIndex];

	for (int i = 0; i < 3; ++i)
	{
		uint vertexIndex = tri.index[i];
		ShareVertexBuffer[vertexIndex].normal += tri.normal;
		ShareVertexBuffer[vertexIndex].shareCount += 1;
	}
}

/*
这里是计算vertexnormal信息，顶点数量为128x128
128可以分解为16x8 = 128
所以我们发出16x16个线程group
Dispatch(16,16,1)
每个group里有两8x8=64个线程
vertex_size_x 8
vertex_size_y 8
*/

#define vertex_size_x 32
#define vertex_size_y 32
[numthreads(vertex_size_x, vertex_size_y, 1)]
void CS_ComputeVertexNormal(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID,
	uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex)
{
	uint indexX = GroupID.x * vertex_size_x + GroupThreadID.x;
	uint indexY = GroupID.y * vertex_size_y + GroupThreadID.y;
	uint vertexIndex = indexY * 1024 + indexX;

	ShareVertex shareVertex = ShareVertexBuffer[vertexIndex];

	GroupMemoryBarrierWithGroupSync();
	VertexNormalBuffer[vertexIndex] = normalize(shareVertex.normal / shareVertex.shareCount);
}