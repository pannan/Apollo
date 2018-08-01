#pragma once

#include "Vector3.h"

namespace Apollo
{

	//front��ʾaabb��ǰ���face������z��������Ļ��ģ�����front face��zӦ���ǽ�С��
	//LT ��ʾ left top,�����ͬ��
	//�����ǰ�˳ʱ�������ж��㣬�����˳ʱ����Ҫ���faceʱ��˳ʱ��
	enum AABBCornerIndex
	{
		ACI_FRONT_LT,
		ACI_FRONT_RT,
		ACI_FRONT_RB,
		ACI_FRONT_LB,
		ACI_BACK_LT,
		ACI_BACK_RT,
		ACI_BACK_RB,
		ACI_BACK_LB
	};

	class AABB
	{
	public:

		AABB();

		void			merge(const Vector3& pos);

		const Vector3*	getCotner()const;

	protected:

		Vector3		m_min;
		Vector3		m_max;

		Vector3		m_corner[8];

		bool				m_isDirty;

	private:
	};
}
