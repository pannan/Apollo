#pragma once
#include "ModelDX11.h"
#include "ShaderDX11.h"
#include "ConstantBufferDX11.h"
#include "Matrix4x4.h"
#include "Graphics/Camera.h"
#include "RenderStateDX11.h"
#include "IEventListener.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "SampleBase.h"
namespace Apollo
{
	class Texture2dDX11;
	class FirstPersonCamera;

	struct ComputeNormalChunk
	{
		Vector3f	normal;
		int			shareCount; 
	};

	class HeightMapTerrain : public SampleBase
	{
	public:

		HeightMapTerrain();
		~HeightMapTerrain();

		virtual void  init();

		virtual void	render();

		virtual Camera* getCamera() { return m_camera; }

	protected:

		void		createMesh();

		void		createShader();

		void		computeNormal(byte* vertexBuffer,uint32_t* indexBuffer,int vertexSize,int positionOffset,int vertexCount,uint32_t indexCount,Vector3f* outNormalBuffer);

		void		computeNormalWithGPU();

		void		updateCamera();

	private:

		int		m_terrainSize;

		uint32_t	m_vertexCount;
		uint32_t	m_indexCount;

		ModelDX11				m_terrainModel;

		ShaderDX11Ptr		m_vsShader;
		ShaderDX11Ptr		m_psShader;

		ShaderDX11Ptr		m_computerTriangleNormalShader;
		ShaderDX11Ptr		m_initShareVertexShader;
		ShaderDX11Ptr		m_computerShareVertexNormal;
		ShaderDX11Ptr		m_computerVertexNormal;

		Matrix4x4				m_mvpMat;
		ConstantBufferDX11Ptr m_mvpBuffer;

		Camera*					m_camera;

		RenderStateDX11	m_renderState;

		Vector3f*					m_terrainPosBuffer;
		uint32_t*					m_terrainIndexBuffer;

		StructuredBufferDX11Ptr	m_terrainVertexStructBuffer;
		StructuredBufferDX11Ptr	m_terrainIndexStructBuffer;
		StructuredBufferDX11Ptr	m_TriangleRWStructBuffer;
		StructuredBufferDX11Ptr	m_shareVertexRWStructBuffer;
		StructuredBufferDX11Ptr	m_vertexNormalRWStructBuffer;
	};
}
