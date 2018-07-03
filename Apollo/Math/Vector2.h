#pragma once

namespace Apollo
{
	class Vector2
	{
	public:

		Vector2() : m_x(0.0f) , m_y(0.0f){}
		Vector2(float x,float y) : m_x(x), m_y(y){}

		Vector2 operator * (const float& v) 
		{
			Vector2 vec2;
			vec2.m_x = m_x * v;
			vec2.m_y = m_y * v;
			return vec2;
		}

		Vector2 operator - (const Vector2& vec) const
		{
			Vector2 vec2;
			vec2.m_x = m_x - vec.m_x;
			vec2.m_y = m_y - vec.m_y;
			return vec2;
		}

		float m_x;
		float m_y;
	};
}