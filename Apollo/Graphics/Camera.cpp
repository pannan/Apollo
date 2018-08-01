#include "stdafx.h"
#include "Camera.h"
#include "Quaternion.h"
#include "Timer.h"
#include "EventManager.h"
namespace Apollo
{

	Camera::Camera()
	{
		init(Vector3(0.0, 0.0, -50.0), Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0), 1.0, 60.0, 800, 600, 90.0f);
		
		EventManager::getInstance().addMouseEventListener(this);
		EventManager::getInstance().addKeyDownEventListener(this);
	}

	Camera::Camera(Vector3 pos, Vector3 lookAt, Vector3 upDir, float nearDis, float farDis, float xViewAngle)
	{
		init(pos, lookAt, upDir, nearDis, farDis, 800, 600, xViewAngle);
		
		EventManager::getInstance().addMouseEventListener(this);
		EventManager::getInstance().addKeyDownEventListener(this);
	}
	Camera::~Camera()
	{
		if (EventManager::getInstancePtr())
		{
			EventManager::getInstance().removeMouseEventListener(this);
			EventManager::getInstance().removeKeyDownEventListener(this);
		}
	}

	void Camera::init(Vector3 pos, Vector3 lookAt, Vector3 upDir, float nearDis, float farDis, int vpWidth, int vpHeight, float xViewAngle)
	{
		m_viewportWidth = vpWidth;
		m_viewportHeight = vpHeight;

		m_cameraPos = pos;
		m_camLookDir = (lookAt - pos);
		m_camLookDir.normalize();

		m_upDir = upDir;

		m_rightDir = m_upDir.corss(m_camLookDir);
		m_rightDir.normalize();

		m_nearClipDis = nearDis;
		m_farClipDis = farDis;
		m_xVIewAngle = xViewAngle;
	}

	void Camera::updateViewProjMatrix()
	{
		updateViewMatrix();
		updateProjMatrix();

		m_viewProjMatrix = m_viewMatrix * m_projectMatrix;
	}


	//viewmatrix两部分，一个是平移到视点位置，二是选择世界坐标和相机坐标系一致
	void Camera::updateViewMatrix(bool isYaw)
	{
		Matrix4x4 tranMat;
		tranMat.makeTransformMatrix(Vector3(-m_cameraPos.m_x, -m_cameraPos.m_y, -m_cameraPos.m_z));
		Matrix4x4 rotateMatrix;

		m_camLookDir.normalize();
		m_upDir.normalize();
		m_rightDir.normalize();

		if (isYaw)
		{
			//yaw是绕up轴旋转，所以up是不变的，利用up和dir来计算rightdir
			m_camLookDir = m_rightDir.corss(m_upDir);
			m_upDir = m_camLookDir.corss(m_rightDir);
			//m_upDir = m_camLookDir.corss(m_rightDir);			
		}
		else
		{
			//Pitch是绕rightdir轴旋转，所以rightdir是不变的，利用rightdir和dir来计算up dir
			m_upDir = m_camLookDir.corss(m_rightDir);
			//m_camLookDir = m_rightDir.corss(m_upDir);
		//	m_upDir = m_camLookDir.corss(m_rightDir);		
		}

		m_camLookDir.normalize();
		m_upDir.normalize();
		m_rightDir.normalize();
		
		rotateMatrix.Identity();
		rotateMatrix.m_matrix[0][0] = m_rightDir.m_x; rotateMatrix.m_matrix[0][1] = m_rightDir.m_y; rotateMatrix.m_matrix[0][2] = m_rightDir.m_z;
		rotateMatrix.m_matrix[1][0] = m_upDir.m_x; rotateMatrix.m_matrix[1][1] = m_upDir.m_y; rotateMatrix.m_matrix[1][2] = m_upDir.m_z;
		rotateMatrix.m_matrix[2][0] = m_camLookDir.m_x; rotateMatrix.m_matrix[2][1] = m_camLookDir.m_y; rotateMatrix.m_matrix[2][2] = m_camLookDir.m_z;

		rotateMatrix.transposition();

		m_viewMatrix = tranMat * rotateMatrix;
	}

	void Camera::updateProjMatrix()
	{
		/*
		透视效果：根据等比三角形，在viewspace的点Pv(x,y,z,1)投影到画板上为Ps(x,y,d)(d为画板到camera的距离)->Psy / d = Pvy / Pvz -> Psy = Pvy * (d / Pvz);
		所以最后在屏幕上的坐标为 Ps(Pvx * (d / Pvz),Pvy * (d / Pvz),d,1)。现在我的得到的是一个除以w的齐次坐标系，我们每个分量除以d/Pvz得到 (Pvx,Pvy,Pvz,Pvz/d)
		因为齐次坐标(x,y,z,w)->(x/w,y/w,z/w,1),所以透视矩阵为
		1 0 0 0
		0 1 0 0
		0 0 1 1/d
		0 0 0 0

		这里的d值可以取除0外的任意值。如果取负数，那么最后的图像最倒转，所以需要设置近裁剪面。为了方便，d一般取1。矩阵变成
		1 0 0 0
		0 1 0 0
		0 0 1 1
		0 0 0 0
		*/

		Matrix4x4 mat1;
		mat1.m_matrix[2][3] = 1.0f;

		/*
		考虑更通用的情况d的值。如果相机水平视角xAngle不为90度,设xAngle为a，那么再x为[-1,1]范围下，half_x = x * 0.5 = 1  ->half_x / d = tan(a * 0.5)
		d = half_x / tan(a * 0.5) = 1.0f / tan(a * 0.5f)
		*/
		mat1.m_matrix[2][3] = 1.0f / tan(m_xVIewAngle * 0.5f);

		/*
		当视锥的视角为90度时（水平角度），那么x=z，所以x坐标被投影到[-1,+1],如果画板是正方形，那么y也被投影到[-1,+1]范围。
		但是画板很可能不是正方形，那么设宽高比w/h=p ->1/h(这里x为1) = p ->h = 1/p，得到，宽高比为p的y投影范围是[-1/p,1/p]
		由于我们希望透视变换后的坐标范围都在[-1,+1]，所以y坐标需要缩放p
		1 0 0 0
		0 p 0 0
		0 0 1 0
		0 0 0 1
		*/

		Matrix4x4 mat2;
		mat2.m_matrix[1][1] = getAspectRatio();

		/*
		另外还需要把z值根据near far plane mapping到0,1.0  depth = (z - near) / (far - near) = z / (far - near) - near / (far - near) = z / L - near / L ,L = far - near
		1 0 0		0
		0 1 0		0
		0 0 1/L		0
		0 0 -near/L	1
		*/
		Matrix4x4 zMapping;
		const float zLenght = m_farClipDis - m_nearClipDis;
		if (zLenght > 0.0f)
		{
			zMapping.m_matrix[2][2] = 1.0f / zLenght;
			zMapping.m_matrix[3][2] = -m_nearClipDis / zLenght;
		}

		//上面的是在w=1时的情况， 但是在经过mat1 * mat2后w不为1,而是为mat1.m_matrix[2][3]，所以zMapping的m_matrix[3][2]= 1.0f / tan(m_xVIewAngle * 0.5f);
		//需要乘以tan(m_xVIewAngle * 0.5f);
		//zMapping.m_matrix[3][2] *= tan(m_xVIewAngle * 0.5f);

		Matrix4x4 finalProject;
		finalProject.m_matrix[2][3] = 1.0f / tan(m_xVIewAngle * 0.5f);
		finalProject.m_matrix[1][1] = getAspectRatio();
		finalProject.m_matrix[2][2] = 1.0f / zLenght;
		finalProject.m_matrix[3][2] = -m_nearClipDis / zLenght;

		Matrix4x4 tempMaat = mat1 * mat2;
		Vector4 vv(1, 1, 50, 1);
		Vector4 vvv = vv * tempMaat;
		Vector4 v = vvv * zMapping;
		v = vv * finalProject;

		m_projectMatrix = finalProject;//tempMaat * zMapping;
	}

	Vector4 Camera::transformToSceenPos(const Vector3& localPos)
	{
		updateViewProjMatrix();

		//local pos to proj pos
		Vector4 vec4(localPos.m_x, localPos.m_y, localPos.m_z, 1.0f);
		Vector4 pos1 = vec4 * m_viewMatrix;
		Vector4 pos2 = pos1 * m_projectMatrix;
		Vector4 hpos = pos2 / pos2.m_w;

		//现在proj space变成一个x,y轴在[-1,1.0]，z在(0,1.0]的长方体

		/*
		现在x,y坐标都在[-1,+1]。需要把变换到屏幕坐标。
		假设屏幕为w,h.首先要把[-1,+1]转换为[0,1],x = x * 0.5 + 0.5,y是从上往下，需要y = 1.0f - (y * 0.5 + 0.5) = 0.5 - y*0.5;
		0.5 0    0  0
		0   -0.5 0  0
		0   0    0  0
		0.5 0.5  0  0
		*/
		Matrix4x4 mat1;
		mat1.m_matrix[0][0] = 0.5f;
		mat1.m_matrix[1][1] = -0.5f;
		mat1.m_matrix[2][2] = 0.0f;
		mat1.m_matrix[3][3] = 0.0f;
		mat1.m_matrix[3][0] = 0.5f;
		mat1.m_matrix[3][1] = 0.5f;

		Vector4 pos3 = hpos * mat1;
		/*
		在乘w,h.
		w 0 0 0
		0 h 0 0
		0 0 1 0
		0 0 0 1
		*/
		Matrix4x4 mat2;
		mat2.m_matrix[0][0] = m_viewportWidth;
		mat2.m_matrix[1][1] = m_viewportHeight;

		Vector4 pos4 = pos3 * mat2;

		return pos4;
	}

	void Camera::rotationQuaternion(const Quaternion& qua)
	{
		/*Quaternion qnorm = qua;
		qnorm.normalize();
		mOrientation = qnorm * mOrientation;*/
	}

	void Camera::rotationYaw(float angle)
	{
		Quaternion rotQua;
		//rotQua.createFromAxisAngle(m_upDir.m_x, m_upDir.m_y, m_upDir.m_z, angle);
		rotQua.createFromAxisAngle(0,1,0, angle);
		Quaternion viewDirQua;
		/*viewDirQua.m_x = m_camLookDir.m_x;
		viewDirQua.m_y = m_camLookDir.m_y;
		viewDirQua.m_z = m_camLookDir.m_z;*/
		viewDirQua.m_x = m_rightDir.m_x;
		viewDirQua.m_y = m_rightDir.m_y;
		viewDirQua.m_z = m_rightDir.m_z;
		viewDirQua.m_w = 0.0;

		Quaternion rel = viewDirQua * rotQua;
		/*m_camLookDir.m_x = rel.m_x;
		m_camLookDir.m_y = rel.m_y;
		m_camLookDir.m_z = rel.m_z;*/
		m_rightDir.m_x = rel.m_x;
		m_rightDir.m_y = rel.m_y;
		m_rightDir.m_z = rel.m_z;
		m_rightDir.normalize();

		updateViewMatrix();
	}

	void Camera::rotationPitch(float angle)
	{
		Quaternion rotQua;
		rotQua.createFromAxisAngle(m_rightDir.m_x, m_rightDir.m_y, m_rightDir.m_z, angle);
		Quaternion viewDirQua;
		viewDirQua.m_x = m_camLookDir.m_x;
		viewDirQua.m_y = m_camLookDir.m_y;
		viewDirQua.m_z = m_camLookDir.m_z;
		viewDirQua.m_w = 0.0;

		Quaternion rel = viewDirQua * rotQua;
		m_camLookDir.m_x = rel.m_x;
		m_camLookDir.m_y = rel.m_y;
		m_camLookDir.m_z = rel.m_z;
		m_camLookDir.normalize();

		updateViewMatrix(false);
	}

	void Camera::move(const Vector3& dir, float moveDis)
	{
		m_cameraPos = m_cameraPos + dir * moveDis;
	}

	void Camera::onMouseMoveEvent(MouseEventArg* arg)
	{
		static bool g_firstMoveMouse = true;
		if (g_firstMoveMouse)
		{
			g_firstMoveMouse = false;
			m_lastMousePos = Vector2(arg->mouseX, arg->mouseY);
			return;
		}

		Vector2 currentMousePos = Vector2(arg->mouseX, arg->mouseY);
		Vector2 dxdy = currentMousePos - m_lastMousePos;
		m_lastMousePos = currentMousePos;

		if (arg->rButton == false)
			return;

		const float CamRotSpeed = 0.180f * Timer::getInstance().elapsed();
		Vector2 dxdySpeed = dxdy * 0.18f;
		//if (abs(dxdySpeed.x) > abs(dxdySpeed.y))
		rotationYaw(-dxdySpeed.m_x);
		//else
		rotationPitch(-dxdySpeed.m_y);
	}

	void Camera::onKeyDownEvent(KeyCode code)
	{
		float CamMoveSpeed = 1;//1000 *  Timer::getInstance().elapsed();
		Vector3 camPos = m_cameraPos;
		switch (code)
		{
		case  KeyCode::W:
			camPos += m_camLookDir * CamMoveSpeed;
			m_cameraPos = camPos;
			break;
		case  KeyCode::S:
			camPos -= m_camLookDir * CamMoveSpeed;
			m_cameraPos = camPos;
			break;

		case  KeyCode::D:
			move(m_rightDir, CamMoveSpeed);
			//m_camera->rotationYaw(-1);
			break;
		case  KeyCode::A:
			move(m_rightDir, -CamMoveSpeed);
			//m_camera->rotationYaw(1);
			break;
		}
	}
}