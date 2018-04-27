#include "stdafx.h"
#include "Camera.h"
//#include "ViewPortDX11.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
Camera::Camera() :	
	m_fNear(0.1f),
	m_fFar(100.0f),
	m_fAspect(1280.0f / 800.0f),
	m_fFov(static_cast<float>(GLYPH_PI) / 4.0f),
	m_fWidth(1280.0f),
	m_fHeight(800.0f),
	m_ProjMatrix()
{
	// Create the spatial controller, which will be used to manipulate the node
	// in a simple way.

	m_ProjMatrix.MakeIdentity();

	//m_pViewPositionWriter = Parameters.SetVectorParameter(L"ViewPosition", Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

	// By default, the camera body is not pickable.  This behavior can be updated
	// by the client simply by adding the picking geometry to the entity.
}
//--------------------------------------------------------------------------------
Camera::~Camera()
{
	// If a render view has been added to the camera, then delete it when the 
	// camera is released.

//	SAFE_DELETE(m_pCameraView);
	//SAFE_DELETE(m_pOverlayView);
}
//--------------------------------------------------------------------------------
void Camera::RenderFrame(RendererDX11* pRenderer)
{
	
}
//--------------------------------------------------------------------------------
void Camera::SetProjectionParams(float zn, float zf, float aspect, float fov)
{
	m_fNear = zn;
	m_fFar = zf;
	m_fAspect = aspect;
	m_fFov = fov;

	ApplyProjectionParams();
}
//--------------------------------------------------------------------------------
void Camera::SetOrthographicParams(float zn, float zf, float width, float height)
{
	m_fNear = zn;
	m_fFar = zf;
	m_fWidth = width;
	m_fHeight = height;

	ApplyOrthographicParams();
}
//--------------------------------------------------------------------------------
void Camera::SetOffsetProjectionParams(float l, float r, float b, float t, float zn, float zf)
{
	m_ProjMatrix[0] = 2.0f * zn / (r - l);
	m_ProjMatrix[1] = 0.0f;
	m_ProjMatrix[2] = 0.0f;
	m_ProjMatrix[3] = 0.0f;

	m_ProjMatrix[4] = 0.0f;
	m_ProjMatrix[5] = 2.0f * zn / (t - b);
	m_ProjMatrix[6] = 0.0f;
	m_ProjMatrix[7] = 0.0f;

	m_ProjMatrix[8] = (l + r) / (l - r);
	m_ProjMatrix[9] = (t + b) / (b - t);
	m_ProjMatrix[10] = zf / (zf - zn);
	m_ProjMatrix[11] = 1.0f;

	m_ProjMatrix[12] = 0.0f;
	m_ProjMatrix[13] = 0.0f;
	m_ProjMatrix[14] = zn*zf / (zn - zf);
	m_ProjMatrix[15] = 0.0f;
}
//--------------------------------------------------------------------------------
void Camera::SetClipPlanes(float zn, float zf)
{
	m_fNear = zn;
	m_fFar = zf;

	ApplyProjectionParams();
}
//--------------------------------------------------------------------------------
void Camera::SetAspectRatio(float aspect)
{
	m_fAspect = aspect;

	ApplyProjectionParams();
}
//--------------------------------------------------------------------------------
void Camera::SetFieldOfView(float fov)
{
	m_fFov = fov;

	ApplyProjectionParams();
}
//--------------------------------------------------------------------------------
float Camera::GetNearClipPlane()
{
	return(m_fNear);
}
//--------------------------------------------------------------------------------
float Camera::GetFarClipPlane()
{
	return(m_fFar);
}
//--------------------------------------------------------------------------------
float Camera::GetAspectRatio()
{
	return(m_fAspect);
}
//--------------------------------------------------------------------------------
float Camera::GetFieldOfView()
{
	return(m_fFov);
}
//--------------------------------------------------------------------------------
void Camera::ApplyProjectionParams()
{
	m_ProjMatrix = Matrix4f::PerspectiveFovLHMatrix(m_fFov, m_fAspect, m_fNear, m_fFar);
}
//--------------------------------------------------------------------------------
void Camera::ApplyOrthographicParams()
{
	m_ProjMatrix = Matrix4f::OrthographicLHMatrix(m_fNear, m_fFar, m_fWidth, m_fHeight);
}
//--------------------------------------------------------------------------------
const Matrix4f& Camera::ProjMatrix() const
{
	return(m_ProjMatrix);
}