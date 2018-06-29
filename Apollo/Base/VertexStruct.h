#pragma once

#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"

namespace Apollo
{
	struct  Vertex_Pos
	{
		Vector4f pos;
	};

	struct  Vertex_Pos_UV0
	{
		Vector4f pos;
		Vector2f uv0;
	};

	struct  Vertex_Pos_UV0_Normal
	{
		Vector4f pos;
		Vector2f uv0;
		Vector3f normal;
	};
}
