#pragma once

#include "Assert.h"

namespace Apollo
{

	// Forward declarations
	struct Quaternion;
	struct Float3x3;
	struct Float4x4;

	struct Float2
	{
		float x, y;

		Float2();
		Float2(float x);
		Float2(float x, float y);
		Float2(const DirectX::XMFLOAT2& xy);
		Float2(DirectX::FXMVECTOR xy);

		Float2& operator+=(const Float2& other);
		Float2 operator+(const Float2& other) const;

		Float2& operator-=(const Float2& other);
		Float2 operator-(const Float2& other) const;

		Float2& operator*=(const Float2& other);
		Float2 operator*(const Float2& other) const;

		Float2& operator*=(float s);
		Float2 operator*(float s) const;

		Float2& operator/=(const Float2& other);
		Float2 operator/(const Float2& other) const;

		Float2& operator/=(float s);
		Float2 operator/(float s) const;

		bool operator==(const Float2& other) const;
		bool operator!=(const Float2& other) const;

		Float2 operator-() const;

		DirectX::XMVECTOR ToSIMD() const;

		static Float2 Clamp(const Float2& val, const Float2& min, const Float2& max);
		static float Length(const Float2& val);
	};

	struct Float3
	{
		float x, y, z;

		Float3();
		Float3(float x);
		Float3(float x, float y, float z);
		Float3(Float2 xy, float z);
		Float3(const DirectX::XMFLOAT3& xyz);
		Float3(DirectX::FXMVECTOR xyz);

		float operator[](unsigned int idx) const;
		Float3& operator+=(const Float3& other);
		Float3 operator+(const Float3& other) const;

		Float3& operator+=(float other);
		Float3 operator+(float other) const;

		Float3& operator-=(const Float3& other);
		Float3 operator-(const Float3& other) const;

		Float3& operator-=(float s);
		Float3 operator-(float s) const;

		Float3& operator*=(const Float3& other);
		Float3 operator*(const Float3& other) const;

		Float3& operator*=(float s);
		Float3 operator*(float s) const;

		Float3& operator/=(const Float3& other);
		Float3 operator/(const Float3& other) const;

		Float3& operator/=(float s);
		Float3 operator/(float s) const;

		bool operator==(const Float3& other) const;
		bool operator!=(const Float3& other) const;

		Float3 operator-() const;

		DirectX::XMVECTOR ToSIMD() const;
		Float2 To2D() const;

		float Length() const;

		static float Dot(const Float3& a, const Float3& b);
		static Float3 Cross(const Float3& a, const Float3& b);
		static Float3 Normalize(const Float3& a);
		static Float3 Transform(const Float3& v, const Float3x3& m);
		static Float3 Transform(const Float3& v, const Float4x4& m);
		static Float3 TransformDirection(const Float3&v, const Float4x4& m);
		static Float3 Transform(const Float3& v, const Quaternion& q);
		static Float3 Clamp(const Float3& val, const Float3& min, const Float3& max);
		static Float3 Perpendicular(const Float3& v);
		static float Distance(const Float3& a, const Float3& b);
		static float Length(const Float3& v);
	};

	struct Quaternion
	{
		float x, y, z, w;

		Quaternion();
		Quaternion(float x, float y, float z, float w);
		Quaternion(const Float3& axis, float angle);
		Quaternion(const Float3x3& m);
		Quaternion(const DirectX::XMFLOAT4& q);
		Quaternion(DirectX::FXMVECTOR q);

		Quaternion& operator*=(const Quaternion& other);
		Quaternion operator*(const Quaternion& other) const;

		bool operator==(const Quaternion& other) const;
		bool operator!=(const Quaternion& other) const;

		Float3x3 ToFloat3x3() const;
		Float4x4 ToFloat4x4() const;

		static Quaternion Identity();
		static Quaternion Invert(const Quaternion& q);
		static Quaternion FromAxisAngle(const Float3& axis, float angle);
		static Quaternion FromEuler(float x, float y, float z);
		static Quaternion Normalize(const Quaternion& q);
		static Float3x3 ToFloat3x3(const Quaternion& q);
		static Float4x4 ToFloat4x4(const Quaternion& q);

		DirectX::XMVECTOR ToSIMD() const;
	};

	struct Float3x3 : public DirectX::XMFLOAT3X3
	{
		Float3x3();
		Float3x3(const Float3& x, const Float3& y, const Float3& z);
		Float3x3(const DirectX::XMFLOAT3X3& m);
		Float3x3(DirectX::CXMMATRIX m);

		Float3x3& operator*=(const Float3x3& other);
		Float3x3 operator*(const Float3x3& other) const;

		Float3 Up() const;
		Float3 Down() const;
		Float3 Left() const;
		Float3 Right() const;
		Float3 Forward() const;
		Float3 Back() const;

		void SetXBasis(const Float3& x);
		void SetYBasis(const Float3& y);
		void SetZBasis(const Float3& z);

		static Float3x3 Transpose(const Float3x3& m);
		static Float3x3 Invert(const Float3x3& m);
		static Float3x3 ScaleMatrix(float s);
		static Float3x3 ScaleMatrix(const Float3& s);

		DirectX::XMMATRIX ToSIMD() const;
	};

	struct Float4x4 : public DirectX::XMFLOAT4X4
	{
		Float4x4();
		Float4x4(const DirectX::XMFLOAT4X4& m);
		Float4x4(DirectX::CXMMATRIX m);

		Float4x4& operator*=(const Float4x4& other);
		Float4x4 operator*(const Float4x4& other) const;

		Float3 Up() const;
		Float3 Down() const;
		Float3 Left() const;
		Float3 Right() const;
		Float3 Forward() const;
		Float3 Back() const;

		Float3 Translation() const;
		void SetTranslation(const Float3& t);

		void SetXBasis(const Float3& x);
		void SetYBasis(const Float3& y);
		void SetZBasis(const Float3& z);

		void Scale(const Float3& scale);

		static Float4x4 Transpose(const Float4x4& m);
		static Float4x4 Invert(const Float4x4& m);
		static Float4x4 ScaleMatrix(float s);
		static Float4x4 ScaleMatrix(const Float3& s);
		static Float4x4 TranslationMatrix(const Float3& t);

		bool operator==(const Float4x4& other) const;
		bool operator!=(const Float4x4& other) const;

		DirectX::XMMATRIX ToSIMD() const;

		std::string Print() const;
	};

	// Clamps a value to the specified range
	template<typename T> T Clamp(T val, T min, T max)
	{
		assert(max >= min);

		if (val < min)
			val = min;
		else if (val > max)
			val = max;
		return val;
	}

	// Constants
	const float Pi = 3.141592654f;
	const float Pi2 = 6.283185307f;
	const float Pi_2 = 1.570796327f;
	const float Pi_4 = 0.7853981635f;
	const float InvPi = 0.318309886f;
	const float InvPi2 = 0.159154943f;

	// Max value that we can store in an fp16 buffer (actually a little less so that we have room for error, real max is 65504)
	const float FP16Max = 65000.0f;

	// Scale factor used for storing physical light units in fp16 floats (equal to 2^-10).
	const float FP16Scale = 0.0009765625f;

	const float FloatMax = std::numeric_limits<float>::max();
	const float FloatInfinity = std::numeric_limits<float>::infinity();
}