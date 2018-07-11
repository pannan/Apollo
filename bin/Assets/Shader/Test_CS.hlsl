


RWTexture2D<float4>		OutputMap : register(u0);

StructuredBuffer<float3>		TerrainVertexBuffer;

/*
这里是计算vertexnormal信息，顶点数量为128x128
128可以分解为16x8 = 128
所以我们发出16x16个线程group
Dispatch(16,16,1)
每个group里有两8x8=64个线程
vertex_size_x 8
vertex_size_y 8
*/

#define vertex_size_x 8
#define vertex_size_y 8
[numthreads(vertex_size_x, vertex_size_y, 1)]
void CS_Color(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID,
	uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex)
{
	uint indexX = GroupID.x * vertex_size_x + GroupThreadID.x;
	uint indexY = GroupID.y * vertex_size_y + GroupThreadID.y;
	uint vertexIndex = indexY * 128 + indexX;

	float3 color = TerrainVertexBuffer[indexY * 128 + indexX];

	OutputMap[float2(indexX, indexY)] = float4(color, 1);
}