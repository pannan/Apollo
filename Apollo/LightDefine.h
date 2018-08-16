#pragma once
#include "Vector4.h"
#include "Vector2.h"
namespace Apollo
{
	__declspec(align(16)) class Light
	{
	public:
		enum class LightType : uint32_t
		{
			Point = 0,
			Spot = 1,
			Directional = 2
		};

		//16bit
		Vector4	positionWS;	
		//16bit
		Vector4   directionWS;
		//16bit
		Vector4   color;
		//16bit
		float       spotlightAngle;
		float       range;
		float       intensity;
		LightType   type;

		//--------------------------------------------------------------( 16 * 4 = 64 bytes )
		Light::Light()
			: positionWS(0, 0, 0, 1)
			, directionWS(0, 0, -1, 0)
			, color(1, 1, 1, 1)
			, spotlightAngle(45.0f)
			, range(100.0f)
			, intensity(1.0f)
			, type(LightType::Directional)
		{}
	};

	typedef std::shared_ptr<Light>		LightPtr;
}
