#include "stdafx.h"
#include "GPUParticleSample.h"
#include "RendererDX11.h"

using namespace Apollo;

GPUParticleSample::GPUParticleSample()
{
	m_particleCount = 100000;
}

void GPUParticleSample::init()
{
	Particle* pData = new Particle[m_particleCount];

	for (int i = 0; i < m_particleCount; i++)
	{
		pData[i].position.MakeZero();
		pData[i].direction = Vector3f(0.0f, 0.0f, 1.0f);
		pData[i].time = 0.0f;
	}

	D3D11_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem = pData;
	InitialData.SysMemPitch = 0;
	InitialData.SysMemSlicePitch = 0;

	m_particleACBuffer0 = RendererDX11::getInstance().createAppendConsumeBuffer(m_particleCount, sizeof(Particle), &InitialData);
	m_particleACBuffer1 = RendererDX11::getInstance().createAppendConsumeBuffer(m_particleCount, sizeof(Particle), &InitialData);

	SAFE_DELETE_ARRAY(pData);

	ID3D11Device* device = RendererDX11::getInstance().getDevice();

	//������struct buffer����unoredered aeecss view����insert��update�׶�cs����
	D3D11_BUFFER_UAV uav;
	uav.FirstElement = 0;
	uav.NumElements = m_particleCount;
	uav.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;


	ID3D11UnorderedAccessView * currentParticleUAV = nullptr;
	ID3D11UnorderedAccessView * updatedParticleUAV = nullptr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_particleCount;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	device->CreateUnorderedAccessView(m_particleACBuffer0.Get(), &uavDesc, &currentParticleUAV);
	device->CreateUnorderedAccessView(m_particleACBuffer1.Get(), &uavDesc, &updatedParticleUAV);
	m_currentParticleUAVComPtr = currentParticleUAV;
	m_updatedParticleUAVComPtr = updatedParticleUAV;

	//��updatedParticle����srv,����Ⱦ�׶�vs����
	ID3D11ShaderResourceView* updatedSRV = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = m_particleCount;
	srvDesc.Buffer.ElementWidth = sizeof(Particle);
	srvDesc.Buffer.FirstElement = 0;
	device->CreateShaderResourceView(m_particleACBuffer1.Get(), &srvDesc, &updatedSRV);
	m_updatedParticleSRVComPtr = updatedSRV;

	//��current particle����һ��constant buffer����������current struct buffer��count������ConsumeԽ��
	uint32_t initCount = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &initCount;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	m_currentParticleCountBuffer = RendererDX11::getInstance().createConstantBuffer(sizeof(uint32_t), true, false, &initData);

	//����һ��IndirectArgsBuffe,������GPU�Լ���������point
	initData;
	initData.pSysMem = &initCount;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	m_particleIndirectArgsBuffer = RendererDX11::getInstance().createIndirectArgsBuffer(sizeof(uint32_t),&initData);

	//����shader
}