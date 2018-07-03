#pragma once

namespace Apollo
{
	#define PI			3.14159265358979323846

	class Matrix3x3;
	class Matrix4x4;
	class Quaternion  
	{
	public:

		Quaternion();
		Quaternion(float x,float y,float z,float w);
		~Quaternion();

		Quaternion operator * (const float f)const;

		float norm()const;

		float normalize();

		void toMatrix(Matrix4x4& mat);

		void fromRotationMatrix (const Matrix3x3& mat);

		void createFromAxisAngle(const float &in_x,const float &in_y,const float &in_z,const float &in_degrees);

		Quaternion operator *(const Quaternion &q);


	//private:

		float m_x;
		float m_y;
		float m_z;
		float m_w;
	};
}