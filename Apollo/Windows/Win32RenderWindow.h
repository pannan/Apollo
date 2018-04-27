#pragma once

#include "RenderWindow.h"

namespace Apollo
{
	class Vector2f;

	class Win32RenderWindow : public RenderWindow
	{
	public:
		Win32RenderWindow();
		virtual ~Win32RenderWindow();

		virtual void Initialize(IWindowProc* WindowProcObj);
		virtual void Shutdown();
		virtual void Paint();

		Vector2f GetCursorPosition();
	};
}
