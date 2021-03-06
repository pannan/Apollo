#include "stdafx.h"
#include "SkySample.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "VertexStruct.h"
#include "DirectXMath.h"
#include "Dimensional/test_case.h"
#include "Environment/Atmosphere/Reference/SkyRenderTest.h"
using namespace Apollo;
using namespace DirectX;

SkySample::SkySample()
{
	m_camera = nullptr;

	m_skyRenderTest = nullptr;
}

SkySample::~SkySample()
{
	SAFE_DELETE(m_camera);
	SAFE_DELETE(m_skyRenderTest);
}

void SkySample::initTestTerrain()
{
	const float terriainSize = 100000;
	const float halfTerrainSize = terriainSize * 0.5f;
	Vertex_Pos data[4];

	//现在中心是在0,0
	data[0].pos = Vector3(-halfTerrainSize, 0, halfTerrainSize);
	data[1].pos = Vector3(halfTerrainSize, 0, halfTerrainSize);
	data[2].pos = Vector3(halfTerrainSize, 0, -halfTerrainSize);
	data[3].pos = Vector3(-halfTerrainSize, 0, -halfTerrainSize);

	//中心变换到camera位置	
	const Vector3& camPos = m_camera->getPosition();
	Vector3 offset(camPos.m_x, 0.0, camPos.m_z);
	data[0].pos += offset;
	data[1].pos += offset;
	data[2].pos += offset;
	data[3].pos += offset;


	//m_quadMesh->createVertexBuffer(data, sizeof(Vertex_Pos_UV0), 4 * sizeof(Vertex_Pos_UV0),4);

	uint16_t index[6] = { 0,1,2,2,3,0 };
	m_testTerrainModel.createFromMemory(data, sizeof(Vertex_Pos), 4, index, 6);

	//create shader and material
	ShaderDX11Ptr terrainVS = ShaderDX11Ptr(new ShaderDX11());
	terrainVS->loadShaderFromFile(ShaderType::VertexShader,
		"../bin/Assets/Shader/MeshOnlyPos.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	ShaderDX11Ptr terrainPS = ShaderDX11Ptr(new ShaderDX11());
	terrainPS->loadShaderFromFile(ShaderType::PixelShader,
		"../bin/Assets/Shader/MeshOnlyPos.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");

	MaterialDX11* material = new MaterialDX11;
	material->m_vs = terrainVS;
	material->m_ps[(uint8_t)RenderPipelineType::ForwardRender] = terrainPS;
	m_testTerrainModel.addMaterial(MaterialPtr(material));

	m_mvpMatrixBufferPtr = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(Matrix4x4), true, true, nullptr));
	terrainVS->setConstantBuffer("PerObject", m_mvpMatrixBufferPtr);

	m_testTerrainRenderState.createState();
}

void SkySample::initQuadMeshAndMaterial()
{
	m_quadModelPtr = ModelDX11::createFullScreenQuadModel();

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
	initQuadMeshAndMaterial();

	m_renderState.setCullingMode(D3D11_CULL_NONE);
	m_renderState.setDepthWriteMask(D3D11_DEPTH_WRITE_MASK_ZERO);
	m_renderState.createState();

	m_camera = new Camera(Vector3(4000, 10, -150), Vector3(0, 0, 0), Vector3(0, 1, 0), 0.001, 5000, 90 * APOLLO_PI / 180.0f);
	m_camera->setViewportWidth(1280);
	m_camera->setViewportHeight(800);

	m_globalParametersBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(GlobalParameters), true, true, nullptr));
	m_vsShader->setConstantBuffer("GlobalParameters", m_globalParametersBuffer);
	m_psShader->setConstantBuffer("GlobalParameters", m_globalParametersBuffer);

	m_atmosphereParametersBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(AtmosphereParameters_Simple), true, true, nullptr));
	//m_atmosphereParametersBuffer = 
	//	ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(Apollo::Atmosphere::Reference::AtmosphereParameters), true, true, nullptr));

	ZeroMemory(&m_atmosphereParameters, sizeof(AtmosphereParameters_Simple));
	m_atmosphereParameters.bottom_radius = 6360.0f; 
	m_atmosphereParameters.top_radius = 6420.0f;
	m_atmosphereParametersBuffer->set(&m_atmosphereParameters, sizeof(AtmosphereParameters_Simple));
	m_psShader->setConstantBuffer("AtmosphereParameters", m_atmosphereParametersBuffer);

	initTestTerrain();
	dimensional::TestCase::RunAllTests();

	m_skyRenderTest = new Apollo::Atmosphere::Reference::SkyRenderTest(40, 30);
	m_skyRenderTest->init();
	//m_skyRenderCPUTest->renderSingleScatting();
	//m_skyRenderCPUTest->saveRadianceRGBBufferToFile();
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
	Vector4 originPos = Vector4(100, -100, 111, 1.0);
	//Vector4 worldPos = originPos * inverseViewProjMat;
	//worldPos /= worldPos.m_w;
	Vector4 viewPos = originPos * m_camera->getProjMat().inverse();
	viewPos /= viewPos.m_w;

	Vector4 worldPos = viewPos * m_camera->getViewMat().inverse();


	XMMATRIX  inMat;
	Matrix4x4 tansMat = viewProjMat.transposition();
	memcpy(&inMat, tansMat.m_matrix, sizeof(Matrix4x4));

	XMVECTOR det = XMMatrixDeterminant(inMat);
	XMMATRIX E = XMMatrixInverse(&det, inMat);

	Matrix4x4 inverseViewProjMat2;
	memcpy(&inverseViewProjMat2, &E, sizeof(XMMATRIX));
	inverseViewProjMat2 = inverseViewProjMat2.transposition();
	Vector4 worldPos2 = originPos * inverseViewProjMat2;
	worldPos2 /= worldPos2.m_w;

		/*Vector3 worldPosVec3 = m_camera->getPosition() + m_camera->getDirection() * 100;
		Vector4 worldPos = Vector4(worldPosVec3.m_x, worldPosVec3.m_y, worldPosVec3.m_z, 1);
		Matrix4x4 viewMat = m_camera->getViewMat();
		Matrix4x4 projMat = m_camera->getProjMat();

		Vector4 viewPos = worldPos * viewMat;
		Vector4 projPos = viewPos * projMat;

		Vector4 clipPos = projPos / projPos.m_w;*/


		Vector4  clipPos(-0.5, 0, 0.5, 1);
		Matrix4x4 projMat = m_camera->getProjMat();
		/*
		project matrix
		1		0		0		0
		0		B		0		0
		0		0		C		A
		0		0		D		1

		这里的z不用除以w
		*/
		//z*C + D = z` ->z = (z` - D) / C
		float z = (clipPos.m_z - projMat.m_matrix[3][2]) / projMat.m_matrix[2][2];
		//x / (z*A) = x` -> x = x` * z*A;
		float x = clipPos.m_x * z * projMat.m_matrix[2][3];
		
		//y*B/(z*A) = y` -> y = y` * (z*A) / B
		float y = clipPos.m_y * z * projMat.m_matrix[2][3] / projMat.m_matrix[1][1];

	int ii = 0;
}

void test2()
{
	XMVECTOR eyePos;
	eyePos.m128_f32[0] = 400;
	eyePos.m128_f32[1] = 100;
	eyePos.m128_f32[2] = -150;
	eyePos.m128_f32[3] = 0;

	XMVECTOR lookAt;
	lookAt.m128_f32[0] = 0;
	lookAt.m128_f32[1] = 0;
	lookAt.m128_f32[2] = 0;
	lookAt.m128_f32[3] = 0;

	XMVECTOR upDir;
	upDir.m128_f32[0] = 0;
	upDir.m128_f32[1] = 1;
	upDir.m128_f32[2] = 0;
	upDir.m128_f32[3] = 0;
	XMMATRIX viewMat = XMMatrixLookAtLH(eyePos, lookAt, upDir);

	XMMATRIX projMat = XMMatrixPerspectiveFovLH(90 * APOLLO_PI / 180.0f, 1280.0f / 800.0f, 0.001, 5000);
	XMMATRIX viewProjMat = XMMatrixMultiply(viewMat,projMat);

	XMVECTOR det = XMMatrixDeterminant(viewProjMat);
	XMMATRIX inverseMat = XMMatrixInverse(&det, viewProjMat);
	det = XMMatrixDeterminant(projMat);
	XMMATRIX inverseProjMat = XMMatrixInverse(&det, projMat);

	XMVECTOR pos;
	pos.m128_f32[0] = 1;
	pos.m128_f32[1] = 0.0;
	pos.m128_f32[2] = 0.5;
	pos.m128_f32[3] = 1.0;

	XMVECTOR pos2 = XMVector4Transform(pos ,inverseMat);
	pos2 /= pos2.m128_f32[3];

	XMVECTOR pos3 = XMVector4Transform(pos, inverseProjMat);
	pos3 /= pos3.m128_f32[3];

	//XMVECTOR pos;
	pos.m128_f32[0] = 250;
	pos.m128_f32[1] = 50;
	pos.m128_f32[2] = 0;
	pos.m128_f32[3] = 1.0;

	XMVECTOR pos4 = XMVector4Transform(pos, viewProjMat);
	pos4 /= pos4.m128_f32[3];

	pos4 = XMVector4Transform(pos, inverseMat);
	pos4 /= pos4.m128_f32[3];

	int ii = 0;
}

void SkySample::drawTestTerrain()
{	
	Matrix4x4 mvpMat = m_camera->getViewProjMat();
	m_mvpMatrixBufferPtr->set(&mvpMat, sizeof(Matrix4x4));

	m_testTerrainRenderState.bind();

	m_testTerrainModel.draw();
}

void SkySample::onGUI()
{
	m_skyRenderTest->onGUI();
}

void SkySample::render()
{
	m_camera->updateViewProjMatrix();
	//test();
	//test2();
	m_renderState.bind();

	ZeroMemory(&m_globalParameters, sizeof(GlobalParameters));
	Vector3 worldPosVec3 = m_camera->getPosition();
	Vector3 earthSpacePosVec3 = (m_camera->getPosition() * 0.001f + Vector3(0.0, m_atmosphereParameters.bottom_radius, 0.0f));
	m_globalParameters.eyeWorldSpacePosition = Vector4(worldPosVec3.m_x, worldPosVec3.m_y, worldPosVec3.m_z, 1.0f);
	m_globalParameters.eyeEarthSpacePosition = Vector4(earthSpacePosVec3.m_x, earthSpacePosVec3.m_y, earthSpacePosVec3.m_z, 1.0f);
	//m_globalParameters.inverseViewMatrix = m_camera->getViewMat().inverse();
	//m_globalParameters.inverseProjMatrix = m_camera->getProjMat().inverse();
	m_globalParameters.inverseViewProjMatrix = m_camera->getViewProjMat().transposition().inverse();
	m_globalParameters.inverseViewMatrix = m_camera->getViewMat().inverse();
	//memcpy(m_globalParameters.projMat, m_camera->getProjMat().m_matrix, sizeof(Matrix4x4));
	Matrix4x4 projMat = m_camera->getProjMat();
	m_globalParameters.projMat[0] = Vector4(projMat.m_matrix[0][0], projMat.m_matrix[0][1], projMat.m_matrix[0][2], projMat.m_matrix[0][3]);
	m_globalParameters.projMat[1] = Vector4(projMat.m_matrix[1][0], projMat.m_matrix[1][1], projMat.m_matrix[1][2], projMat.m_matrix[1][3]);
	m_globalParameters.projMat[2] = Vector4(projMat.m_matrix[2][0], projMat.m_matrix[2][1], projMat.m_matrix[2][2], projMat.m_matrix[2][3]);
	m_globalParameters.projMat[3] = Vector4(projMat.m_matrix[3][0], projMat.m_matrix[3][1], projMat.m_matrix[3][2], projMat.m_matrix[3][3]);
	//m_globalParameters.projMat = m_camera->getProjMat();

	m_globalParametersBuffer->set(&m_globalParameters, sizeof(GlobalParameters));

	m_quadModelPtr->draw();

	
	drawTestTerrain();
}