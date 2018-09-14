#include "stdafx.h"
#include "SkySample.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "VertexStruct.h"

using namespace Apollo;

struct MatrixBuffer
{
	Matrix4x4	inverseViewProjMatrix;
};

struct AtmosphereParameters
{
	//这里是地球半径
	float		bottom_radius;
	float		top_radius;
};

SkySample::SkySample()
{
	m_camera = nullptr;
}

SkySample::~SkySample()
{
	SAFE_DELETE(m_camera);
}

void SkySample::initQuadMesh()
{
	m_quadModelPtr = ModelDX11Ptr(ModelDX11::createFullScreenQuadModel());

	//create material
	m_vsShader = ShaderDX11Ptr(new ShaderDX11());
	m_vsShader->loadShaderFromFile(ShaderType::VertexShader,
		"../bin/Assets/Shader/Sky.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	m_psShader = ShaderDX11Ptr(new ShaderDX11());
	m_psShader->loadShaderFromFile(ShaderType::PixelShader,
		"../bin/Assets/Shader/Sky.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");

	MaterialDX11* material = new MaterialDX11;
	material->m_vs = m_vsShader;
	material->m_ps[(uint8_t)RenderPipelineType::ForwardRender] = m_psShader;
	m_quadModelPtr->addMaterial(MaterialPtr(material));
}

void SkySample::init()
{
	initQuadMesh();

	m_renderState.createState();

	m_camera = new Camera(Vector3(400, 100, -150), Vector3(0, 0, 0), Vector3(0, 1, 0), 0.001, 5000, 90 * PI / 180.0f);
	m_camera->setViewportWidth(1280);
	m_camera->setViewportHeight(800);

	m_globalParametersBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(GlobalParameters), true, true, nullptr));
	m_vsShader->setConstantBuffer("GlobalParameters", m_globalParametersBuffer);
	m_psShader->setConstantBuffer("GlobalParameters", m_globalParametersBuffer);

	m_atmosphereParametersBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(AtmosphereParameters), true, true, nullptr));

	ZeroMemory(&m_atmosphereParameters, sizeof(AtmosphereParameters));
	m_atmosphereParameters.bottom_radius = 6360.0f; 
	m_atmosphereParameters.top_radius = 6420.0f;
	m_atmosphereParametersBuffer->set(&m_atmosphereParameters, sizeof(AtmosphereParameters));
	m_psShader->setConstantBuffer("AtmosphereParameters", m_atmosphereParametersBuffer);
}

void SkySample::render()
{
	m_camera->updateViewProjMatrix();

	m_renderState.bind();

	ZeroMemory(&m_globalParameters, sizeof(GlobalParameters));
	m_globalParameters.eyePosition = (m_camera->getPosition() * 0.001f + Vector3(0.0,m_atmosphereParameters.bottom_radius,0.0f));
	m_globalParameters.inverseViewMatrix = m_camera->getViewMat().inverse();
	m_globalParameters.inverseProjMatrix = m_camera->getProjMat().inverse();
	m_globalParametersBuffer->set(&m_globalParameters, sizeof(GlobalParameters));

	m_quadModelPtr->draw();
}