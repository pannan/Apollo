#pragma once

namespace Apollo
{
	class IWindowProc
	{
	public:
		virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
		virtual void BeforeRegisterWindowClass(WNDCLASSEX &wc) = 0;
	};
}
