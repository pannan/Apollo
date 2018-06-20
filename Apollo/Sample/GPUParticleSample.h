#pragma once

#include "Vector3f.h"
#include "ShaderDX11.h"

namespace Apollo
{
	/*
	1 ��������struct buffer,������������view type��һ����shader resource view��srv����Ϊ�������Ⱦ��ʱ����vs���ȡ���ӵ�λ��
	һ����unordered access view,������flag��D3D11_BUFFER_UAV_FLAG_APPEND������Ϊ��
	1��ÿ֡��ʼʱ����ǰstruct buffer�������ӣ���Append��ʽ
	2��������struct buffer��������ʱ����Consume�ӵ�ǰstruct bufferȡ�����ӣ����º󣬰�������Append��һ��struct buffer

	2 ��������const buffer��bind flag��D3D11_BIND_CONSTANT_BUFFER��������¼��ǰstruct buffer�Ĵ�С�����������Ч���ݣ���һ��
	�����¼���º����ӵ�struct buffer������������ʹ��DrawInstancedIndirect����GPU�ڲ�����input assarlar�Ķ��������ǣ���Ҫ֪��point������

	���ݴ����������ÿ֡��ǰstruct buffer��������
	��cs��������ӣ���sbuffer0->subffer1
	ͨ��DrawInstancedIndirect����point��Ȼ����vs��õ�λ�ã���gs������һ��������������ɵ��ı���
	��ps���������
	*/

	struct Particle
	{
		Vector3f position;
		Vector3f direction;
		float    time;
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

		BufferComPtr			m_particleACBuffer0;
		BufferComPtr			m_particleACBuffer1;

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

	private:
	};
}
