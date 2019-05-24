#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Apollo
{
	struct  Vertex_Pos
	{
		Vector3 pos;
	};

	struct  Vertex_Pos_Color
	{
		Vector3 pos;
		unsigned int	color;
	};

	struct  Vertex_Pos_UV0
	{
		Vector3 pos;
		Vector2 uv0;
	};

	struct  Vertex_Pos_UV0_Normal
	{
		Vector3 pos;
		Vector2 uv0;
		Vector3 normal;
	};
}
