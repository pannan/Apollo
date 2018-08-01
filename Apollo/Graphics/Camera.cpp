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


	//viewmatrix�����֣�һ����ƽ�Ƶ��ӵ�λ�ã�����ѡ������������������ϵһ��
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
			//yaw����up����ת������up�ǲ���ģ�����up��dir������rightdir
			m_camLookDir = m_rightDir.corss(m_upDir);
			m_upDir = m_camLookDir.corss(m_rightDir);
			//m_upDir = m_camLookDir.corss(m_rightDir);			
		}
		else
		{
			//Pitch����rightdir����ת������rightdir�ǲ���ģ�����rightdir��dir������up dir
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
		͸��Ч�������ݵȱ������Σ���viewspace�ĵ�Pv(x,y,z,1)ͶӰ��������ΪPs(x,y,d)(dΪ���嵽camera�ľ���)->Psy / d = Pvy / Pvz -> Psy = Pvy * (d / Pvz);
		�����������Ļ�ϵ�����Ϊ Ps(Pvx * (d / Pvz),Pvy * (d / Pvz),d,1)�������ҵĵõ�����һ������w���������ϵ������ÿ����������d/Pvz�õ� (Pvx,Pvy,Pvz,Pvz/d)
		��Ϊ�������(x,y,z,w)->(x/w,y/w,z/w,1),����͸�Ӿ���Ϊ
		1 0 0 0
		0 1 0 0
		0 0 1 1/d
		0 0 0 0

		�����dֵ����ȡ��0�������ֵ�����ȡ��������ô����ͼ���ת��������Ҫ���ý��ü��档Ϊ�˷��㣬dһ��ȡ1��������
		1 0 0 0
		0 1 0 0
		0 0 1 1
		0 0 0 0
		*/

		Matrix4x4 mat1;
		mat1.m_matrix[2][3] = 1.0f;

		/*
		���Ǹ�ͨ�õ����d��ֵ��������ˮƽ�ӽ�xAngle��Ϊ90��,��xAngleΪa����ô��xΪ[-1,1]��Χ�£�half_x = x * 0.5 = 1  ->half_x / d = tan(a * 0.5)
		d = half_x / tan(a * 0.5) = 1.0f / tan(a * 0.5f)
		*/
		mat1.m_matrix[2][3] = 1.0f / tan(m_xVIewAngle * 0.5f);

		/*
		����׶���ӽ�Ϊ90��ʱ��ˮƽ�Ƕȣ�����ôx=z������x���걻ͶӰ��[-1,+1],��������������Σ���ôyҲ��ͶӰ��[-1,+1]��Χ��
		���ǻ���ܿ��ܲ��������Σ���ô���߱�w/h=p ->1/h(����xΪ1) = p ->h = 1/p���õ�����߱�Ϊp��yͶӰ��Χ��[-1/p,1/p]
		��������ϣ��͸�ӱ任������귶Χ����[-1,+1]������y������Ҫ����p
		1 0 0 0
		0 p 0 0
		0 0 1 0
		0 0 0 1
		*/

		Matrix4x4 mat2;
		mat2.m_matrix[1][1] = getAspectRatio();

		/*
		���⻹��Ҫ��zֵ����near far plane mapping��0,1.0  depth = (z - near) / (far - near) = z / (far - near) - near / (far - near) = z / L - near / L ,L = far - near
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

		//���������w=1ʱ������� �����ھ���mat1 * mat2��w��Ϊ1,����Ϊmat1.m_matrix[2][3]������zMapping��m_matrix[3][2]= 1.0f / tan(m_xVIewAngle * 0.5f);
		//��Ҫ����tan(m_xVIewAngle * 0.5f);
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

		//����proj space���һ��x,y����[-1,1.0]��z��(0,1.0]�ĳ�����

		/*
		����x,y���궼��[-1,+1]����Ҫ�ѱ任����Ļ���ꡣ
		������ĻΪw,h.����Ҫ��[-1,+1]ת��Ϊ[0,1],x = x * 0.5 + 0.5,y�Ǵ������£���Ҫy = 1.0f - (y * 0.5 + 0.5) = 0.5 - y*0.5;
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
		�ڳ�w,h.
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