#include "stdafx.h"
#include "SkySample.h"
#include "Matrix4x4.h"
#include "Camera.h"
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

void SkySample::init()
{

}

void SkySample::render()
{

}