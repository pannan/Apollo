#include "stdafx.h"
#include "OutDoorScene.h"
#include "Graphics/Camera.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "RendererDX11.h"
#include "Texture2dConfigDX11.h"
#include "TextureDX11ResourceFactory.h"
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
	m_forwardPipelinePtr->addRenderPass(m_renderPassPtr.get());

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
	//normal map
	texConfig.SetFormat(DXGI_FORMAT_R16G16B16A16_FLOAT);
	uint32_t normalMapHandle = TextureDX11ResourceFactory::getInstance().createTexture2D("GBuffer_Adelbo", texConfig);
}

void OutDoorScene::render()
{
	m_camera->updateViewProjMatrix();
	MatrixBuffer matrixBuffer;
	matrixBuffer.MVPMatrix = m_camera->getViewProjMat();

	m_matrixBuffer->set(&matrixBuffer, sizeof(MatrixBuffer));

	//m_renderState.bind();

	RendererDX11::getInstance().clearDebugInfo();
	//m_modelScene.draw();
	m_forwardPipelinePtr->render((RenderState*)(&m_renderState));
}