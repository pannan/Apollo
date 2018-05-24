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
#include "DX11Renderer.h"
#include "RenderStateDX11.h"

using namespace Apollo;
LogManager logManager;
MaterialParse materialParse;
// Data
//static ID3D11Device*            g_pd3dDevice = NULL;
//static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
//static IDXGISwapChain*          g_pSwapChain = NULL;
//static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

int g_windowsWidth = 1280;
int g_windowsHeight = 800;

//void CreateRenderTarget()
//{
//	DXGI_SWAP_CHAIN_DESC sd;
//	g_pSwapChain->GetDesc(&sd);
//
//	// Create the render target
//	ID3D11Texture2D* pBackBuffer;
//	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
//	ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
//	render_target_view_desc.Format = sd.BufferDesc.Format;
//	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
//	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
//	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &g_mainRenderTargetView);
//	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
//	pBackBuffer->Release();
//}

//void CleanupRenderTarget()
//{
//	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
//}

//HRESULT CreateDeviceD3D(HWND hWnd)
//{
//	// Setup swap chain
//	DXGI_SWAP_CHAIN_DESC sd;
//	{
//		ZeroMemory(&sd, sizeof(sd));
//		sd.BufferCount = 2;
//		sd.BufferDesc.Width = 0;
//		sd.BufferDesc.Height = 0;
//		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//		sd.BufferDesc.RefreshRate.Numerator = 60;
//		sd.BufferDesc.RefreshRate.Denominator = 1;
//		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
//		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//		sd.OutputWindow = hWnd;
//		sd.SampleDesc.Count = 1;
//		sd.SampleDesc.Quality = 0;
//		sd.Windowed = TRUE;
//		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//	}
//
//	UINT createDeviceFlags = 0;
//	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
//	D3D_FEATURE_LEVEL featureLevel;
//	const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0, };
//	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 1, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
//		return E_FAIL;
//
//	// Setup rasterizer
//	{
//		D3D11_RASTERIZER_DESC RSDesc;
//		memset(&RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
//		RSDesc.FillMode = D3D11_FILL_SOLID;
//		RSDesc.CullMode = D3D11_CULL_NONE;
//		RSDesc.FrontCounterClockwise = FALSE;
//		RSDesc.DepthBias = 0;
//		RSDesc.SlopeScaledDepthBias = 0.0f;
//		RSDesc.DepthBiasClamp = 0;
//		RSDesc.DepthClipEnable = TRUE;
//		RSDesc.ScissorEnable = TRUE;
//		RSDesc.AntialiasedLineEnable = FALSE;
//		RSDesc.MultisampleEnable = (sd.SampleDesc.Count > 1) ? TRUE : FALSE;
//
//		ID3D11RasterizerState* pRState = NULL;
//		g_pd3dDevice->CreateRasterizerState(&RSDesc, &pRState);
//		g_pd3dDeviceContext->RSSetState(pRState);
//		pRState->Release();
//	}
//
//	CreateRenderTarget();
//
//	return S_OK;
//}
//
//void CleanupDeviceD3D()
//{
//	CleanupRenderTarget();
//	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
//	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
//	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
//}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (DX11Renderer::getInstance().getDevice() != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX11_InvalidateDeviceObjects();
			//CleanupRenderTarget();
			DX11Renderer::getInstance().releaseMainRTT();
			DX11Renderer::getInstance().getSwapChain()->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			//CreateRenderTarget();
			DX11Renderer::getInstance().createMainRTT();
			ImGui_ImplDX11_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
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

	DX11Renderer dx11Renderer;
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
	MaterialResourceFactory materialFactory;
	HLSLResourceFactory hlslFactory;
//	TextureResourceFactory textureFactroy;
	TextureDX11ResourceFactory textureFactroy;

	RenderStateDX11	renderStateDX11;
	
	AssetsDirectoryManager* directoryManager = new AssetsDirectoryManager;
	directoryManager->init("..\\bin\\Assets");

	UIRoot uiRoot;

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

		uiRoot.render(g_windowsWidth,g_windowsHeight);
		logUI.render();

		// Rendering
		dx11Renderer.getDeviceContex()->ClearRenderTargetView(dx11Renderer.getMainRTT(), (float*)&clear_col);
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		dx11Renderer.getSwapChain()->Present(0, 0);
	}

	SAFE_DELETE(directoryManager);

	fsw.Close(1000);
	ImGui_ImplDX11_Shutdown();
	UnregisterClass(_T("ImGui Example"), wc.hInstance);

	return 0;
}
