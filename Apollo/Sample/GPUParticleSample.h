#pragma once

#include "Vector3.h"
#include "Vector4.h"
#include "ShaderDX11.h"
#include "Matrix4x4.h"
namespace Apollo
{
	/*
	1 创建两个struct buffer,并且它有两个view type，一个是shader resource view，srv，作为在最后渲染的时候，在vs里读取粒子的位置
	一个是unordered access view,创建的flag是D3D11_BUFFER_UAV_FLAG_APPEND，作用为：
	1）每帧开始时，向当前struct buffer插入粒子，用Append方式
	2）在两个struct buffer更新粒子时，用Consume从当前struct buffer取出例子，更新后，把例子用Append另一个struct buffer

	2 创建两个const buffer，bind flag是D3D11_BIND_CONSTANT_BUFFER，用来记录当前struct buffer的大小，避免访问无效数据，另一个
	保存记录更新后粒子的struct buffer，这里是用来使用DrawInstancedIndirect来用GPU内部生产input assarlar的顶点数据是，需要知道point的数量

	数据创建玩后，首先每帧向当前struct buffer插入例子
	在cs里更新例子，从sbuffer0->subffer1
	通过DrawInstancedIndirect生成point，然后在vs里得到位置，在gs里生成一个由两个三角组成的四边形
	在ps里输出像素
	*/

	struct Particle
	{
		Vector3 position;
		Vector3 direction;
		float    time;
	};

	__declspec(align(16)) struct ParticleInsertParameters
	{
		Vector4 EmitterLocation;
		Vector4 RandomVector;
	};

	__declspec(align(16)) struct SimulationParameters
	{
		Vector4 TimeFactors;
		//float4 EmitterLocation;
		Vector4 ConsumerLocation;
	};

	__declspec(align(16)) struct Transforms
	{
		Matrix4x4 WorldViewMatrix;
		Matrix4x4 ProjMatrix;
	};

	struct ParticleRenderParameters
	{
		//float4 EmitterLocation;
		Vector4 ConsumerLocation;
	};

	class GPUParticleSample
	{
	public:

		GPUParticleSample();
		~GPUParticleSample(){}

		void init();

		void render();

	protected:

		void	insertParticle();

		void updateParticle();

		void renderParticle();

	protected:

		size_t						m_particleCount;

		//BufferComPtr			m_particleACBuffer0;
		//BufferComPtr			m_particleACBuffer1;
		StructuredBufferDX11Ptr		m_currentParticelBuffer;
		StructuredBufferDX11Ptr		m_nextParticleBuffer;

		UnorderedAccessViewComPtr			m_currentParticleUAVComPtr;
		UnorderedAccessViewComPtr			m_updatedParticleUAVComPtr;

		ShaderResourceViewComPtr				m_updatedParticleSRVComPtr;

		BufferComPtr										m_currentParticleCountBuffer;

		BufferComPtr										m_particleIndirectArgsBuffer;

		ShaderDX11Ptr			m_insertParticleCS;
		ShaderDX11Ptr			m_updateParticleCS;

		ShaderDX11Ptr			m_renderParticleVS;
		ShaderDX11Ptr			m_renderParticleGS;
		ShaderDX11Ptr			m_renderParticlePS;

		//Insert 粒子的初始化数据
		ParticleInsertParameters		m_particleInsertData;

		//粒子更新时需要的数据
		SimulationParameters			m_updateParticleData;

		Transforms							m_cameraTransforms;

		ParticleRenderParameters	m_particleRenderParameter;

		ConstantBufferDX11Ptr		m_insertParticleBuffer;
		ConstantBufferDX11Ptr		m_updateParticleBuffer;
		ConstantBufferDX11Ptr		m_currentParticleBufferCount;
		ConstantBufferDX11Ptr		m_afterUpdateParticleBufferCount;
		ConstantBufferDX11Ptr		m_transformConstantBuffer;
		ConstantBufferDX11Ptr		m_particleRenderBuffer;

	private:
	};
}
