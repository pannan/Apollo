// ImGui - standalone example application for DirectX 11
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
#include "stdafx.h"
#include <imgui.h>
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include "AssetsDirectoryManager.h"
#include "UIRoot.h"
#include "FileSystemWatcher.h"
#include "LogManager.h"
#include "LogUI.h"
#include "MaterialParse.h"
#include "ResourceManager.h"
#include "MaterialResourceFactory.h"
#include "HLSLResourceFactory.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
//#include "RenderStateDX11.h"
#include "Timer.h"
#include "Sample/GPUParticleSample.h"
#include "Sample/ComputerShaderProcessTexture.h"
#include "EventManager.h"
#include "Sample/TestSample.h"
#include "KeyCodes.h"
#include "Sample/SampleManager.h"
#include "HLSLDX11Factory.h"

using namespace Apollo;
LogManager logManager;
MaterialParse materialParse;

int g_windowsWidth = 1600;
int g_windowsHeight = 900;

EventManager	g_eventManager;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	if (GetKeyState('W') & 0x8000/*check if high-order bit is set (1 << 15)*/)
	{
		g_eventManager.notifyKeyDownEvent(KeyCode::W);
	}
	else if (GetKeyState('D') & 0x8000)
	{
		g_eventManager.notifyKeyDownEvent(KeyCode::D);
	}
	else if (GetKeyState('A') & 0x8000)
	{
		g_eventManager.notifyKeyDownEvent(KeyCode::A);
	}
	else if (GetKeyState('S') & 0x8000)
	{
		g_eventManager.notifyKeyDownEvent(KeyCode::S);
	}

	switch (msg)
	{
	case WM_SIZE:
		if (RendererDX11::getInstance().getDevice() != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX11_InvalidateDeviceObjects();
			//CleanupRenderTarget();
			RendererDX11::getInstance().releaseMainRTT();
			RendererDX11::getInstance().getSwapChain()->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			//CreateRenderTarget();
			RendererDX11::getInstance().createMainRTT();
			ImGui_ImplDX11_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_KEYDOWN:
	{
		MSG charMsg;

		// Get the unicode character (UTF-16)
		unsigned int c = 0;
		// For printable characters, the next message will be WM_CHAR.
		// This message contains the character code we need to send the KeyPressed event.
		// Inspired by the SDL 1.2 implementation.
		/*if (PeekMessage(&charMsg, hwnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
		{
			GetMessage(&charMsg, hwnd, 0, 0);
			c = static_cast<unsigned int>(charMsg.wParam);
		}*/

		bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
		bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
		bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

		KeyCode key = (KeyCode)wParam;
		unsigned int scanCode = (lParam & 0x00FF0000) >> 16;
		//KeyEventArgs keyEventArgs(*pWindow, key, c, KeyEventArgs::Pressed, control, shift, alt);
		//pWindow->OnKeyPressed(keyEventArgs);
		//g_eventManager.notifyKeyDownEvent(key);
	}
	break;
	case WM_MOUSEMOVE:
	{
		MouseEventArg mea;
		mea.lButton = (wParam & MK_LBUTTON) != 0;
		mea.rButton = (wParam & MK_RBUTTON) != 0;
		mea.mButton = (wParam & MK_MBUTTON) != 0;
		mea.shiftButton = (wParam & MK_SHIFT) != 0;
		mea.controlButton = (wParam & MK_CONTROL) != 0;

		mea.mouseX = ((int)(short)LOWORD(lParam));
		mea.mouseY = ((int)(short)HIWORD(lParam));

		g_eventManager.notifyMouseMoveEvent(&mea);
//		MouseMotionEventArgs mouseMotionEventArgs(*pWindow, lButton, mButton, rButton, control, shift, x, y);
		//pWindow->OnMouseMoved(mouseMotionEventArgs);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void __stdcall MyDeal(FileSystemWatcher::ACTION act, LPCWSTR filename, LPVOID lParam)
{
	static FileSystemWatcher::ACTION pre = FileSystemWatcher::ACTION_ERRSTOP;
	switch (act)
	{
	case FileSystemWatcher::ACTION_ADDED:
		wprintf_s(L"Added     - %s\n", filename);
		break;
	case FileSystemWatcher::ACTION_REMOVED:
		wprintf_s(L"Removed   - %s\n", filename);
		break;
	case FileSystemWatcher::ACTION_MODIFIED:
		wprintf_s(L"Modified  - %s\n", filename);
		break;
	case FileSystemWatcher::ACTION_RENAMED_OLD:
		wprintf_s(L"Rename(O) - %s\n", filename);
		break;
	case FileSystemWatcher::ACTION_RENAMED_NEW:
		assert(pre == FileSystemWatcher::ACTION_RENAMED_OLD);
		wprintf_s(L"Rename(N) - %s\n", filename);
		break;
	case FileSystemWatcher::ACTION_ERRSTOP:
	default:
		wprintf_s(L"---ERROR---%s\n", filename);
		break;
	}
	pre = act;
}

int main(int, char**)
{
	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("ImGui Example"), NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(_T("ImGui Example"), _T("ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, g_windowsWidth, g_windowsHeight, NULL, NULL, wc.hInstance, NULL);

	RendererDX11 dx11Renderer;
	// Initialize Direct3D
	if (dx11Renderer.init(hwnd) != S_OK)
	{
		dx11Renderer.release();
		UnregisterClass(_T("ImGui Example"), wc.hInstance);
		return 1;
	}
	// Show the window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Setup ImGui binding
	ImGui_ImplDX11_Init(hwnd, dx11Renderer.getDevice(), dx11Renderer.getDeviceContex());

	// Setup style
	ImGui::StyleColorsDark();
	// Load Fonts
	// (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
	//     io.Fonts->AddFontDefault();
	     //io.Fonts->AddFontFromFileTTF("../bin/Assets/extra_fonts/Cousine-Regular.ttf", 15.0f);
	//     io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
	io.Fonts->AddFontFromFileTTF("../bin/Assets/extra_fonts/segoeui.ttf", 18);
	//     io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
	//     io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_col = ImColor(114, 144, 154);

	ResourceManager resManager;
	TextureDX11ResourceFactory textureFactroy;
	dx11Renderer.createMainDepthStencil();
	MaterialResourceFactory materialFactory;
	HLSLResourceFactory hlslFactory;
	HLSLDX11Factory hlslDX11Factroy;
//	TextureResourceFactory textureFactroy;
	
	
	AssetsDirectoryManager* directoryManager = new AssetsDirectoryManager;
	directoryManager->init("..\\bin\\Assets");

	UIRoot uiRoot;

	Timer timer;
	timer.update();

	//GPUParticleSample	particleSample;
	//particleSample.init();

//	ComputerShaderProcessTexture cspt;
//	cspt.init();
	//HeightMapTerrain terrain;
	//terrain.init();
	//TestSample testSample;
	//testSample.init();
	SampleManager sampleManager;
	sampleManager.init();

	LPCTSTR sDir = TEXT("F:\\GitHub\\Apollo\\bin\\Assets");
	DWORD dwNotifyFilter = FileSystemWatcher::FILTER_FILE_NAME | FileSystemWatcher::FILTER_DIR_NAME | FileSystemWatcher::FILTER_LAST_WRITE_NAME | 
		FileSystemWatcher::FILTER_LAST_ACCESS_NAME;

	FileSystemWatcher fsw;
	bool r = fsw.Run(sDir, true, dwNotifyFilter, &MyDeal, 0);

	
	LogUI logUI;

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		ImGui_ImplDX11_NewFrame();

		// 1. Show a simple window
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		//{
		//	static float f = 0.0f;
		//	ImGui::Text("Hello, world!");
		//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		//	ImGui::ColorEdit3("clear color", (float*)&clear_col);
		//	if (ImGui::Button("Test Window")) show_test_window ^= 1;
		//	if (ImGui::Button("Another Window")) show_another_window ^= 1;
		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//}

		//// 2. Show another simple window, this time using an explicit Begin/End pair
		//if (show_another_window)
		//{
		//	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		//	ImGui::Begin("Another Window", &show_another_window);
		//	ImGui::Text("Hello");
		//	ImGui::End();
		//}

		//// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		//if (show_test_window)
		//{
		//	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		//	ImGui::ShowTestWindow();
		//}
		timer.update();
		//uiRoot.render(g_windowsWidth,g_windowsHeight);
		//logUI.render();
		sampleManager.debugOverlay();

		sampleManager.preRender();

		//dx11Renderer.getDeviceContex()->OMSetRenderTargets(1, dx11Renderer.getMainRTTView(), dx11Renderer.getMainDepthSteniclView());
		// Rendering
		dx11Renderer.getDeviceContex()->ClearRenderTargetView(dx11Renderer.getMainRTTView(), (float*)&clear_col);
		dx11Renderer.getDeviceContex()->ClearDepthStencilView(dx11Renderer.getMainDepthSteniclView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		ImGui::Render();

		//terrain.render();
		sampleManager.render();
		//testSample.render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		//cspt.render();
		
		dx11Renderer.getSwapChain()->Present(0, 0);

		sampleManager.postRender();
	}

	SAFE_DELETE(directoryManager);

	fsw.Close(1000);
	ImGui_ImplDX11_Shutdown();
	UnregisterClass(_T("ImGui Example"), wc.hInstance);

	return 0;
}
