#include "stdafx.h"
#include "OutDoorScene.h"
#include "Graphics/Camera.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "RendererDX11.h"
#include "Texture2dConfigDX11.h"
#include "TextureDX11ResourceFactory.h"
#include "ClearRenderTargetPass.h"
#include "DeferredRenderLightPass.h"
#include "VertexStruct.h"
using namespace Apollo;

struct MatrixBuffer
{
	Matrix4x4	worldMatrix;
	Matrix4x4	MVPMatrix;
};

OutDoorScene::OutDoorScene()
{
	m_camera = nullptr;
}

OutDoorScene::~OutDoorScene()
{
	SAFE_DELETE(m_camera);
}

void OutDoorScene::init()
{
	m_camera = new Camera(Vector3(400, 100, -150), Vector3(0, 0, 0), Vector3(0, 1, 0), 0.001, 5000, 90 * PI / 180.0f);
	m_camera->setViewportWidth(1280);
	m_camera->setViewportHeight(800);

	ShaderDX11Ptr vs = ShaderDX11Ptr(new ShaderDX11());
	vs->loadShaderFromFile(ShaderType::VertexShader,
	"../bin/Assets/Shader/Mesh.hlsl",
	ShaderMacros(),
	"VSMAIN",
	"vs_5_0");

	m_matrixBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(MatrixBuffer), true, true, nullptr));
	vs->setConstantBuffer("PerObject", m_matrixBuffer);
	m_modelScene.createFromSDKMeshFile(L"Powerplant", vs);

	m_renderState.m_depthStencilDesc.DepthEnable = true;
	m_renderState.createState();

	//forward pass这里不需要设置rtt

	//create scene
	m_scenePtr = ScenePtr(new Scene);

	//add renderalbe
	const uint32_t subMeshCount = m_modelScene.getSubMeshCount();
	for (uint32_t i = 0; i < subMeshCount; ++i)
	{
		m_scenePtr->addRenderable(m_modelScene.getSubMesh(i));
	}

	const uint32_t materialCount = m_modelScene.getMaterialCount();
	for (uint32_t i = 0; i < materialCount; ++i)
	{
		m_scenePtr->addMaterial(m_modelScene.getMaterial(i));
	}

	m_renderPassPtr = RenderPassPtr(new RenderPass(m_scenePtr, (RenderState*)(&m_renderState)));

	m_forwardPipelinePtr = RenderPipelinePtr(new RenderPipeline);
	m_forwardPipelinePtr->addRenderPass(m_renderPassPtr);

	initDeferredPipeline();
}

void OutDoorScene::initDeferredPipeline()
{
	Texture2dConfigDX11 texConfig;
	texConfig.SetWidth(RendererDX11::getInstance().getBackBufferWidth());
	texConfig.SetHeight(RendererDX11::getInstance().getBackBufferHeight());
	texConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
	texConfig.SetFormat(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	//创建gbuffer rtt
	//adelbo map
	uint32_t adelboMapHandle = TextureDX11ResourceFactory::getInstance().createTexture2D("GBuffer_Adelbo", texConfig);
	Texture2dDX11* adelboTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(adelboMapHandle);
	//normal map
	texConfig.SetFormat(DXGI_FORMAT_R16G16B16A16_FLOAT);
	uint32_t normalMapHandle = TextureDX11ResourceFactory::getInstance().createTexture2D("GBuffer_Adelbo", texConfig);
	Texture2dDX11* normalTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(normalMapHandle);

	RenderTargetDX11Ptr GBufferRTTPtr = RenderTargetDX11Ptr(new RenderTargetDX11);
	GBufferRTTPtr->attachTexture(AttachmentPoint::Color0, adelboTex);
	GBufferRTTPtr->attachTexture(AttachmentPoint::Color1, normalTex);

	m_deferredPiplinePtr = RenderPipelinePtr(new RenderPipeline);
	//add clear rtt pass
	m_deferredPiplinePtr->addRenderPass(std::make_shared<ClearRenderTargetPass>(GBufferRTTPtr));

	m_deferredGBufferRenderState.setRenderPipelineType(RenderPipelineType::DeferredRender);
	m_deferredGBufferPassPtr = RenderPassPtr(new RenderPass(m_scenePtr, (RenderState*)(&m_deferredGBufferRenderState)));
	m_deferredPiplinePtr->addRenderPass(m_deferredGBufferPassPtr);
	//add lightpass
	m_deferredLightingPassPtr = RenderPassPtr(new DeferredRenderLightPass(m_scenePtr, (RenderState*)(&m_deferredGBufferRenderState)));
	m_deferredPiplinePtr->addRenderPass(m_deferredLightingPassPtr);
}

void OutDoorScene::initQuadMesh()
{
	MaterialDX11* material = new MaterialDX11;
	material->m_vs = ShaderDX11Ptr(new ShaderDX11());
	material->m_vs->loadShaderFromFile(ShaderType::VertexShader,
		"../bin/Assets/Shader/DeferredLighting.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");
	material->m_ps[(uint8_t)RenderPipelineType::DeferredRender] = ShaderDX11Ptr(new ShaderDX11());
	material->m_ps[(uint8_t)RenderPipelineType::DeferredRender]->loadShaderFromFile(ShaderType::PixelShader,
		"../bin/Assets/Shader/DeferredLighting.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"vs_5_0");

	Vertex_Pos_UV0 data[4];
	data[0].pos = Vector3(-1, 1, 0);
	data[0].uv0 = Vector2(0, 0);
	data[1].pos = Vector3(1, 1, 0);
	data[1].uv0 = Vector2(1, 0);
	data[2].pos = Vector3(1, -1, 0);
	data[2].uv0 = Vector2(1, 1);
	data[3].pos = Vector3(-1, -1, 0);
	data[3].uv0 = Vector2(0, 1);
	uint16_t index[6] = { 0,1,2,2,3,0 };
	m_deferredLightingQuadModel.createFromMemory(data, sizeof(Vertex_Pos_UV0), 4, index, 6);

	m_deferredLightingQuadModel.addMaterial(MaterialPtr(material));
}

void OutDoorScene::render()
{
	m_camera->updateViewProjMatrix();
	MatrixBuffer matrixBuffer;
	matrixBuffer.MVPMatrix = m_camera->getViewProjMat().transposition();

	m_matrixBuffer->set(&matrixBuffer, sizeof(MatrixBuffer));

	//m_renderState.bind();
	//似乎应该把renderstate放到renderpass里
	RendererDX11::getInstance().clearDebugInfo();
	//m_modelScene.draw();
	m_forwardPipelinePtr->render((RenderState*)(&m_renderState));
	//m_deferredPiplinePtr->render((RenderState*)(&m_renderState));
}