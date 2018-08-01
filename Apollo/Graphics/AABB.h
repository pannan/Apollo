#pragma once

#include "Vector3.h"

namespace Apollo
{

	//front表示aabb在前面的face，由于z轴是向屏幕里的，所以front face的z应该是较小的
	//LT 表示 left top,后面的同理
	//这里是按顺时针来排列顶点，这里的顺时针是要面对face时的顺时针
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
