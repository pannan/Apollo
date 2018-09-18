#include "stdafx.h"
#include "SkySample.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "VertexStruct.h"
#include "DirectXMath.h"
using namespace Apollo;
using namespace DirectX;

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

/*
V(x,y,z,w) * ProjMat = P(x,y,z,w)
ProjMat = M
Vx * M[0,0] + Vy * M[1,0] + Vz * M[2,0] + Vw * M[3,0] = Px		等式1
Vx * M[0,1] + Vy * M[1,1] + Vz * M[2,1] + Vw * M[3,1] = Py		等式2
Vx * M[0,2] + Vy * M[1,2] + Vz * M[2,2] + Vw * M[3,2] = Pz		等式3
Vx * M[0,3] + Vy * M[1,3] + Vz * M[2,3] + Vw * M[3,3] = Pw		等式4

现在有四个未知数Vx,Vy,Vz,Vw，有四个等式：

从等式4得出Vw = (Pw - Vx * M[0,3] - Vy * M[1,3] - Vz * M[2,3]) / M[3,3]  等式5
把等式5带入等式3:
Vx * M[0,2] + Vy * M[1,2] + Vz * M[2,2] + (Pw - Vx * M[0,3] - Vy * M[1,3] - Vz * M[2,3]) * M[3,2] / M[3,3] = Pz		
双方都乘以M[3,3]:
Vx*M[0,2]*M[3,3] + Vy*M[1,2]*M[3,3] + Vz*M[2,2]*M[3,3] + Pw*M[3,2] - Vx*M[0,3]*M[3,2] - Vy*M[1,3]*M[3,2] - Vz*M[2,3]*M[3,3] = Pz*M[3,3]
Vx*(M[0,2]*M[3,3] - M[0,3]*M[3,2]) + Vy*(M[1,2]*M[3,3] - M[1,3]*M[3,2]) + Vz*(M[2,2]*M[3,3] - M[2,3]*M[3,3]) + Pw*M[3,2] = Pz*M[3,3]

Vz = (Pz*M[3,3] - Pw*M[3,2] - Vx*(M[0,2]*M[3,3] - M[0,3]*M[3,2]) - Vy*(M[1,2]*M[3,3] - M[1,3]*M[3,2])) / (M[2,2]*M[3,3] - M[2,3]*M[3,3])		等式6
把等式5带入等式2：
Vx*M[0,1] + Vy*M[1,1] + Vz*M[2,1] + (Pw - Vx * M[0,3] - Vy * M[1,3] - Vz * M[2,3]) * M[3,1] / M[3,3] = Py
双方都乘以M[3,3]:
Vx*M[0,1]*M[3,3] + Vy*M[1,1]*M[3,3] + Vz*M[2,1]*M[3,3] + Pw* M[3,1] - Vx * M[0,3]*M[3,1] - Vy * M[1,3]*M[3,1] - Vz * M[2,3]*M[3,1]  = Py*M[3,3]
Vx*(M[0,1]*M[3,3] - M[0,3]*M[3,1]) + Vy*(M[1,1]*M[3,3] - M[1,3]*M[3,1]) + Vz*(M[2,1]*M[3,3] - M[2,3]*M[3,1]) = Py*M[3,3] - Pw*M[3,1]	等式7

把等式6带入等式7
Vx*(M[0,1]*M[3,3] - M[0,3]*M[3,1]) + Vy*(M[1,1]*M[3,3] - M[1,3]*M[3,1]) + 
(Pz*M[3,3] - Pw*M[3,2] - Vx*(M[0,2]*M[3,3] - M[0,3]*M[3,2]) - Vy*(M[1,2]*M[3,3] - M[1,3]*M[3,2])) * (M[2,1]*M[3,3] - M[2,3]*M[3,1]) / (M[2,2]*M[3,3] - M[2,3]*M[3,3])
 = Py*M[3,3] - Pw*M[3,1]

 为了简化等式，我们令
 A = M[0,1]*M[3,3] - M[0,3]*M[3,1]
 B = M[1,1]*M[3,3] - M[1,3]*M[3,1]
 C = M[0,2]*M[3,3] - M[0,3]*M[3,2]
 D = M[1,2]*M[3,3] - M[1,3]*M[3,2]
 E = M[2,1]*M[3,3] - M[2,3]*M[3,1]
 F = M[2,2]*M[3,3] - M[2,3]*M[3,3]
 G = Pz*M[3,3] - Pw*M[3,2]
 H = Py*M[3,3] - Pw*M[3,1]

 把上面的带入等式6：
Vz = (G - Vx*C - Vy*D) / F		等式8


 Vx*A + Vy*B + (G - Vx*C - Vy*D) * E / F = H
 两边同时乘以F:
 Vx*A*F + Vy*B*F + G*E - Vx*C*E - Vy*D*E = H*F ->

 Vx*(A*F - C*E) + Vy*(B*F - D*E) + G*E = H*F ->

 Vy = (H*F - G*E - Vx*(A*F - C*E)) / (B*F - D*E)		等式9

 把等式9带入等式8
 Vz = (G - Vx*C - (H*F - G*E - Vx*(A*F - C*E))*D / (B*F - D*E)) / F		等式10

*/

void SkySample::test()
{
	Matrix4x4 viewProjMat = m_camera->getViewProjMat();
	Matrix4x4 inverseViewProjMat = m_camera->getViewProjMat().inverse();
	Vector4 originPos = Vector4(-0.5, 0, 0.5, 1.0);
	//Vector4 worldPos = originPos * inverseViewProjMat;
	//worldPos /= worldPos.m_w;
	Vector4 viewPos = originPos * m_camera->getProjMat().inverse();
	viewPos /= viewPos.m_w;

	Vector4 worldPos = viewPos * m_camera->getViewMat().inverse();


	XMMATRIX  inMat;
	memcpy(&inMat, viewProjMat.m_matrix, sizeof(Matrix4x4));

	XMVECTOR det = XMMatrixDeterminant(inMat);
	XMMATRIX E = XMMatrixInverse(&det, inMat);

	Matrix4x4 inverseViewProjMat2;
	memcpy(&inverseViewProjMat2, &E, sizeof(XMMATRIX));

	Vector4 worldPos2 = originPos * inverseViewProjMat2;
	worldPos2 /= worldPos2.m_w;

		/*Vector3 worldPosVec3 = m_camera->getPosition() + m_camera->getDirection() * 100;
		Vector4 worldPos = Vector4(worldPosVec3.m_x, worldPosVec3.m_y, worldPosVec3.m_z, 1);
		Matrix4x4 viewMat = m_camera->getViewMat();
		Matrix4x4 projMat = m_camera->getProjMat();

		Vector4 viewPos = worldPos * viewMat;
		Vector4 projPos = viewPos * projMat;

		Vector4 clipPos = projPos / projPos.m_w;*/

	int ii = 0;
}

void SkySample::render()
{
	m_camera->updateViewProjMatrix();
	test();
	m_renderState.bind();

	ZeroMemory(&m_globalParameters, sizeof(GlobalParameters));
	m_globalParameters.eyeWorldSpacePosition = m_camera->getPosition();// (m_camera->getPosition() * 0.001f + Vector3(0.0, m_atmosphereParameters.bottom_radius, 0.0f));
	m_globalParameters.eyeEarthSpacePosition = (m_camera->getPosition() * 0.001f + Vector3(0.0, m_atmosphereParameters.bottom_radius, 0.0f));
	//m_globalParameters.inverseViewMatrix = m_camera->getViewMat().inverse();
	//m_globalParameters.inverseProjMatrix = m_camera->getProjMat().inverse();
	m_globalParameters.inverseViewProjMatrix = m_camera->getViewProjMat().inverse();
	m_globalParameters.expand = Vector2(0, 0);
	m_globalParametersBuffer->set(&m_globalParameters, sizeof(GlobalParameters));

	m_quadModelPtr->draw();
}