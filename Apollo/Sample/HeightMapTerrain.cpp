#include "stdafx.h"
#include "HeightMapTerrain.h"
#include "VertexStruct.h"

using namespace  Apollo;

void HeightMapTerrain::init()
{
	m_terrainSize = 129;
	createMesh();
}

void HeightMapTerrain::createMesh()
{
	Vertex_Pos* data = new Vertex_Pos[m_terrainSize * m_terrainSize];

	for (int z = 0; z < m_terrainSize; ++z)
	{
		for (int x = 0; x < m_terrainSize; ++x)
		{
			data[z * m_terrainSize + x].pos = Vector4f(x, 0, z,1);
		}
	}

	uint16_t* meshIndex = new uint16_t[(m_terrainSize - 1) * (m_terrainSize - 1) * 6];

	for (int z = 0; z < m_terrainSize - 1; ++z)
	{
		for (int x = 0; x < m_terrainSize - 1; ++x)
		{
			uint32_t vertexIndex = z * m_terrainSize + x;
			uint32_t quadIndex = z * (m_terrainSize - 1) + x;
			uint32_t baseIndex = quadIndex * 6;		//每个quad两个triangles，6个index

			meshIndex[quadIndex] = z * m_terrainSize + x;
			meshIndex[quadIndex + 1] = (z + 1) * m_terrainSize + x;
			meshIndex[quadIndex + 2] = (z + 1) * m_terrainSize + x + 1;

			meshIndex[quadIndex + 3] = (z + 1) * m_terrainSize + x + 1;
			meshIndex[quadIndex + 4] = z * m_terrainSize + x + 1;
			meshIndex[quadIndex + 5] = z * m_terrainSize + x;
		}
	}

	m_terrainMesh->createVertexBuffer(data, sizeof(Vertex_Pos), m_terrainSize * m_terrainSize * sizeof(Vertex_Pos));
	m_terrainMesh->createIndexBuffer(meshIndex, sizeof(uint16_t), (m_terrainSize - 1) * (m_terrainSize - 1) * 6 * sizeof(uint16_t));


	SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(meshIndex);
}