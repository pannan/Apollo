#include "stdafx.h"
#include "GPUParticleSample.h"
#include "RendererDX11.h"
#include "Vector4f.h"

using namespace Apollo;

GPUParticleSample::GPUParticleSample()
{
	m_particleCount = 100000;
}

void GPUParticleSample::init()
{
	//创建insert shader
	m_insertParticleCS = ShaderDX11Ptr(new ShaderDX11());
	m_insertParticleCS->loadShaderFromFile(ComputeShader,
		"../bin/Assets/Shader/ParticleSystemInsertCS.hlsl",
		ShaderMacros(),
		"CSMAIN",
		"cs_5_0");

	//设置insert粒子位置，方向参数
	m_particleInsertData.EmitterLocation = Vector4f(0, 0, 0, 1);
	m_particleInsertData.RandomVector = Vector4f(1, 0, 0, 0);
	D3D11_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem = &m_particleInsertData;
	InitialData.SysMemPitch = 0;
	InitialData.SysMemSlicePitch = 0;

	m_insertParticleBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(ParticleInsertParameters), true, true, &InitialData));
	m_insertParticleCS->setConstantBuffer("ParticleInsertParameters", m_insertParticleBuffer);

	//设置struct uav参数
	Particle* pData = new Particle[m_particleCount];
	for (int i = 0; i < m_particleCount; i++)
	{
		pData[i].position.MakeZero();
		pData[i].direction = Vector3f(0.0f, 0.0f, 1.0f);
		pData[i].time = 0.0f;
	}

	InitialData.pSysMem = pData;
	InitialData.SysMemPitch = 0;
	InitialData.SysMemSlicePitch = 0;
	m_currentParticelBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_particleCount, sizeof(Particle), false, false, &InitialData));
	m_insertParticleCS->setStructuredBuffer("CurrentSimulationState", m_currentParticelBuffer);

	//创建并初始化 particle update shader
	m_updateParticleCS = ShaderDX11Ptr(new ShaderDX11());
	m_updateParticleCS->loadShaderFromFile(ComputeShader,
		"../bin/Assets/Shader/ParticleSystemUpdateCS.hlsl",
		ShaderMacros(),
		"CSMAIN",
		"cs_5_0");

	m_nextParticleBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_particleCount, sizeof(Particle), false, false, &InitialData));
	m_insertParticleCS->setStructuredBuffer("CurrentSimulationState", m_currentParticelBuffer);
	m_updateParticleCS->setStructuredBuffer("NewSimulationState", m_nextParticleBuffer);


	m_updateParticleData.ConsumerLocation = Vector4f(5, 5, 0, 1);
	m_updateParticleData.TimeFactors = Vector4f(0, 0, 0, 0);
	D3D11_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem = &m_updateParticleData;
	InitialData.SysMemPitch = 0;
	InitialData.SysMemSlicePitch = 0;

	m_updateParticleBuffer =  ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(SimulationParameters), true, true, &InitialData));
	m_updateParticleCS->setConstantBuffer("SimulationParameters", m_updateParticleBuffer);

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = 4 * sizeof(UINT);
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.StructureByteStride = 0;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = 0;

	UINT pInitArgs[4];

	pInitArgs[0] = 0;
	pInitArgs[1] = 0;
	pInitArgs[2] = 0;
	pInitArgs[3] = 0;

	D3D11_SUBRESOURCE_DATA InitArgsData;
	InitArgsData.pSysMem = pInitArgs;
	InitArgsData.SysMemPitch = 0;
	InitArgsData.SysMemSlicePitch = 0;
	m_currentParticleBufferCount = ConstantBufferDX11Ptr(new ConstantBufferDX11(desc, &InitArgsData));
	m_updateParticleCS->setConstantBuffer("ParticleCount", m_currentParticleBufferCount);

	//创建并初始化粒子渲染shader
	//VS
	m_renderParticleVS = ShaderDX11Ptr(new ShaderDX11());
	m_renderParticleVS->loadShaderFromFile(VertexShader,
		"../bin/Assets/Shader/ParticleSystemRender.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	m_renderParticleVS->setStructuredBuffer("SimulationState", m_nextParticleBuffer);

	//GS
	m_renderParticleGS = ShaderDX11Ptr(new ShaderDX11());
	m_renderParticleGS->loadShaderFromFile(GeometryShader,
		"../bin/Assets/Shader/ParticleSystemRender.hlsl",
		ShaderMacros(),
		"GSMAIN",
		"gs_5_0");

	m_transformConstantBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(Transforms), true, true, nullptr));
	m_renderParticleGS->setConstantBuffer("Transforms", m_transformConstantBuffer);

	m_particleRenderParameter.ConsumerLocation = Vector4f(5, 0, 0, 0);

	//////////////////////////////////////////////////////////////////////////

	m_particleACBuffer0 = RendererDX11::getInstance().createAppendConsumeBuffer(m_particleCount, sizeof(Particle), &InitialData);
	m_particleACBuffer1 = RendererDX11::getInstance().createAppendConsumeBuffer(m_particleCount, sizeof(Particle), &InitialData);

	SAFE_DELETE_ARRAY(pData);

	ID3D11Device* device = RendererDX11::getInstance().getDevice();

	//对两个struct buffer创建unoredered aeecss view，在insert和update阶段cs访问
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

	//给updatedParticle创建srv,在渲染阶段vs访问
	ID3D11ShaderResourceView* updatedSRV = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = m_particleCount;
	srvDesc.Buffer.ElementWidth = sizeof(Particle);
	srvDesc.Buffer.FirstElement = 0;
	device->CreateShaderResourceView(m_particleACBuffer1.Get(), &srvDesc, &updatedSRV);
	m_updatedParticleSRVComPtr = updatedSRV;

	//给current particle创建一个constant buffer，用来保存current struct buffer的count，避免Consume越界
	uint32_t initCount = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &initCount;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	m_currentParticleCountBuffer = RendererDX11::getInstance().createConstantBuffer(sizeof(uint32_t), true, false, &initData);

	//创建一个IndirectArgsBuffe,用来让GPU自己生成粒子point
	initData;
	initData.pSysMem = &initCount;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	m_particleIndirectArgsBuffer = RendererDX11::getInstance().createIndirectArgsBuffer(sizeof(uint32_t), &initData);

	
	
	

	

	m_renderParticlePS = ShaderDX11Ptr(new ShaderDX11());
	m_renderParticlePS->loadShaderFromFile(PixelShader,
		"../bin/Assets/Shader/ParticleSystemRender.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");
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

	//对两个struct buffer创建unoredered aeecss view，在insert和update阶段cs访问
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

	//给updatedParticle创建srv,在渲染阶段vs访问
	ID3D11ShaderResourceView* updatedSRV = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = m_particleCount;
	srvDesc.Buffer.ElementWidth = sizeof(Particle);
	srvDesc.Buffer.FirstElement = 0;
	device->CreateShaderResourceView(m_particleACBuffer1.Get(), &srvDesc, &updatedSRV);
	m_updatedParticleSRVComPtr = updatedSRV;

	//给current particle创建一个constant buffer，用来保存current struct buffer的count，避免Consume越界
	uint32_t initCount = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &initCount;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	m_currentParticleCountBuffer = RendererDX11::getInstance().createConstantBuffer(sizeof(uint32_t), true, false, &initData);

	//创建一个IndirectArgsBuffe,用来让GPU自己生成粒子point
	initData;
	initData.pSysMem = &initCount;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	m_particleIndirectArgsBuffer = RendererDX11::getInstance().createIndirectArgsBuffer(sizeof(uint32_t),&initData);

	//创建insert shader
	m_insertParticleCS = ShaderDX11Ptr(new ShaderDX11());
	m_insertParticleCS->loadShaderFromFile(	ComputeShader,
																		"../bin/Assets/Shader/ParticleSystemInsertCS.hlsl", 
																		ShaderMacros(), 
																		"CSMAIN", 
																		"cs_5_0");
	//update shader
	m_updateParticleCS = ShaderDX11Ptr(new ShaderDX11());
	m_updateParticleCS->loadShaderFromFile(	ComputeShader,
																			"../bin/Assets/Shader/ParticleSystemUpdateCS.hlsl", 
																			ShaderMacros(), 
																			"CSMAIN", 
																			"cs_5_0");
	//render shader
	m_renderParticleVS = ShaderDX11Ptr(new ShaderDX11());
	m_renderParticleVS->loadShaderFromFile(	VertexShader, 
																			"../bin/Assets/Shader/ParticleSystemRender.hlsl",
																			ShaderMacros(),
																			"VSMAIN",
																			"vs_5_0");

	m_renderParticleGS = ShaderDX11Ptr(new ShaderDX11());
	m_renderParticleGS->loadShaderFromFile(	GeometryShader,
																			"../bin/Assets/Shader/ParticleSystemRender.hlsl",
																			ShaderMacros(),
																			"GSMAIN",
																			"gs_5_0");

	m_renderParticlePS = ShaderDX11Ptr(new ShaderDX11());
	m_renderParticlePS->loadShaderFromFile(	PixelShader,
																			"../bin/Assets/Shader/ParticleSystemRender.hlsl",
																			ShaderMacros(),
																			"PSMAIN",
																			"ps_5_0");
}

void GPUParticleSample::insertParticle()
{
	
}

void GPUParticleSample::updateParticle()
{
	//
}