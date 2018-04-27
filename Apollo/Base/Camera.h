#pragma once

#include "Matrix4f.h"

namespace Apollo
{
	class RendererDX11;

	class Camera
	{
	public:
		Camera();
		virtual ~Camera();		

		void RenderFrame(RendererDX11* pRenderer);

		void SetProjectionParams(float zn, float zf, float aspect, float fov);
		void SetOrthographicParams(float zn, float zf, float width, float height);
		void SetOffsetProjectionParams(float l, float r, float b, float t, float zn, float zf);

		void SetClipPlanes(float zn, float zf);
		void SetAspectRatio(float aspect);
		void SetFieldOfView(float fov);

		float GetNearClipPlane();
		float GetFarClipPlane();
		float GetAspectRatio();
		float GetFieldOfView();

		//SpatialController<Node3D>& Spatial();
		const Matrix4f& ProjMatrix() const;

	//	Ray3f GetWorldSpacePickRay(const Vector2f& location) const;
		Vector2f WorldToScreenSpace(const Vector3f& point);
		Vector3f ScreenToWorldSpace(const Vector2f& cursor);

	//	virtual bool HandleEvent(EventPtr pEvent);
		virtual std::wstring GetName() { return L"sss"; }

		//ParameterContainer Parameters;

	protected:

		void ApplyProjectionParams();
		void ApplyOrthographicParams();

		

		float m_fNear;
		float m_fFar;
		float m_fAspect;
		float m_fFov;

		float m_fWidth;
		float m_fHeight;

		Matrix4f m_ProjMatrix;
		//VectorParameterWriterDX11* m_pViewPositionWriter;
		//SpatialController<Node3D>*	m_pSpatialController;
	};
}
