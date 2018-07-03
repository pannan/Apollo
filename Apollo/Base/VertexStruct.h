#pragma once

#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"

namespace Apollo
{
	struct  Vertex_Pos
	{
		Vector3f pos;
	};

	struct  Vertex_Pos_UV0
	{
		Vector3f pos;
		Vector2f uv0;
	};

	struct  Vertex_Pos_UV0_Normal
	{
		Vector3f pos;
		Vector2f uv0;
		Vector3f normal;
	};
}
