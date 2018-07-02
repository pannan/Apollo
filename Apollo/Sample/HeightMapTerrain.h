#pragma once
#include "MeshDX11.h"
namespace Apollo
{
	class HeightMapTerrain
	{
	public:

		void  init();
	protected:

		void		createMesh();

	private:

		int		m_terrainSize;

		MeshDX11Ptr		m_terrainMesh;
	};
}
