#pragma once

#include "Vector3.h"

#include "Matrix4x4.h"

namespace Apollo
{
	class Quaternion;
	class Camera
	{
	public:

		Camera();
		Camera(Vector3 pos, Vector3 lookAt, Vector3 upDir, float nearDis, float farDis, float xViewAngle);

		void render();

		void updateViewProjMatrix();

		float getAspectRatio()
		{
			return (float)m_viewportWidth / m_viewportHeight;
		}

		void rotationYaw(float angle);

		void rotationRoll(float angle);

		void rotationQuaternion(const Quaternion& qua);

		Vector4 transformToSceenPos(const Vector3& localPos);

		void setViewportWidth(int width)
		{
			m_viewportWidth = width;
		}

		void setViewportHeight(int height)
		{
			m_viewportHeight = height;
		}

		const Matrix4x4&	getProjMat() const
		{
			return m_projectMatrix;
		}

		const Matrix4x4&	getViewProjMat() const
		{
			return m_viewProjMatrix;
		}

		const Matrix4x4&	getViewMat() const
		{
			return m_viewMatrix;
		}

		int		getViewportWidth()const { return m_viewportWidth; }

		int		getViewportHeight()const { return m_viewportHeight; }

		void	move(const Vector3& dir, float moveDis);

		const Vector3&	getDirection() { return m_camLookDir; }

		const Vector3&  getPosition() { return m_cameraPos; }

		void setposition(const Vector3& pos) { m_cameraPos = pos; }

		const Vector3& getRightDir() { return m_rightDir; }

	private:

		void init(Vector3 pos, Vector3 lookAt, Vector3 upDir, float nearDis, float farDis, int vpWidth, int vpHeight, float xViewAngle);

		void updateViewMatrix(bool isYaw = true);

		void updateProjMatrix();

	private:

		Vector3	m_cameraPos;
		Vector3	m_camLookDir;
		Vector3 m_upDir;
		Vector3	m_rightDir;

		float  m_nearClipDis;
		float  m_farClipDis;
		float  m_xVIewAngle;


		Matrix4x4	m_viewMatrix;
		Matrix4x4	m_projectMatrix;
		Matrix4x4	m_viewProjMatrix;

		int			m_viewportWidth;
		int			m_viewportHeight;
	};
}