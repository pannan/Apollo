#include "stdafx.h"

#include "RenderApplication.h"
#include "Win32RenderWindow.h"
#include <sstream>

#include "EventManager.h"

#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"

using namespace Apollo;
//--------------------------------------------------------------------------------
RenderApplication::RenderApplication() :
	m_bMultithreadedMode(false),
	CameraEventHub(),
	ConsoleEventHub()
{
	m_pRenderer11 = 0;
	m_pWindow = 0;

	m_iWidth = 800;
	m_iHeight = 600;

	m_pCamera = 0;

	// Register for window based events here.
	RequestEvent(WINDOW_RESIZE);
	RequestEvent(RENDER_FRAME_START);
	RequestEvent(SYSTEM_RBUTTON_DOWN);
	RequestEvent(SYSTEM_RBUTTON_UP);
	RequestEvent(SYSTEM_MOUSE_MOVE);
	RequestEvent(SYSTEM_MOUSE_LEAVE);
	RequestEvent(RENDER_FRAME_START);
}
//--------------------------------------------------------------------------------
RenderApplication::~RenderApplication()
{
}
//--------------------------------------------------------------------------------
Win32RenderWindow* RenderApplication::CreateRenderWindow()
{
	return(new Win32RenderWindow());
}
//--------------------------------------------------------------------------------
bool RenderApplication::ConfigureRenderingEngineComponents(UINT width, UINT height, D3D_FEATURE_LEVEL desiredLevel, D3D_DRIVER_TYPE driverType)
{
	// Create the renderer and initialize it for the desired device
	// type and feature level.

	m_pRenderer11 = new RendererDX11();

	if (!m_pRenderer11->Initialize(driverType, desiredLevel))
	{
		//Log::Get().Write(L"Could not create hardware device, trying to create the reference device...");

		if (!m_pRenderer11->Initialize(D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_11_0))
		{
			MessageBox(0, L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
			RequestTermination();
			return(false);
		}

		// If using the reference device, utilize a fixed time step for any animations.
		//m_pTimer->SetFixedTimeStep(1.0f / 10.0f);
	}

	// Create the window.
	m_iWidth = width;
	m_iHeight = height;

	// Create the window wrapper class instance.
	m_pWindow = CreateRenderWindow();
	m_pWindow->SetPosition(20, 20);
	m_pWindow->SetSize(m_iWidth, m_iHeight);
	m_pWindow->SetCaption(GetName());
	m_pWindow->Initialize(this);

	// Create a swap chain for the window.
	SwapChainConfigDX11 Config;
	Config.SetWidth(m_pWindow->GetWidth());
	Config.SetHeight(m_pWindow->GetHeight());
	Config.SetOutputWindow(m_pWindow->GetHandle());
	m_pWindow->SetSwapChain(m_pRenderer11->CreateSwapChain(&Config));

	// We'll keep a copy of the swap chain's render target index to 
	// use later.
	m_BackBuffer = m_pRenderer11->GetSwapChainResource(m_pWindow->GetSwapChain());

	SetMultiThreadedMode(true);

	SetScreenShotName(GetName());

	// Create the console actor and 

	//m_pConsole = new ConsoleActor();
	//m_pConsole->SetEventManager(&ConsoleEventHub);

	return(true);
}

//--------------------------------------------------------------------------------
void RenderApplication::ShutdownRenderingEngineComponents()
{
	m_pRenderer11->Shutdown();
	SAFE_DELETE(m_pRenderer11);

	m_pWindow->Shutdown();
	SAFE_DELETE(m_pWindow);
}
//--------------------------------------------------------------------------------
void RenderApplication::ShutdownRenderingSetup()
{

}

//--------------------------------------------------------------------------------
void RenderApplication::HandleWindowResize(HWND handle, UINT width, UINT height)
{
	if (width < 1) width = 1;
	if (height < 1) height = 1;
	m_iWidth = width;
	m_iHeight = height;

	// Resize our rendering window state if the handle matches
	/*if ((m_pWindow != 0) && (m_pWindow->GetHandle() == handle)) {
		m_pWindow->ResizeWindow(width, height);
		m_pRenderer11->ResizeSwapChain(m_pWindow->GetSwapChain(), width, height);
	}*/
}
//--------------------------------------------------------------------------------
void RenderApplication::ToggleMultiThreadedMode()
{

}
//--------------------------------------------------------------------------------
void RenderApplication::SetMultiThreadedMode(bool mode)
{
	
}

//--------------------------------------------------------------------------------
void RenderApplication::SetScreenShotName(const std::wstring name)
{
	m_ScreenShotName = name;
}
//--------------------------------------------------------------------------------
bool RenderApplication::ConfigureEngineComponents()
{
	if (!ConfigureRenderingEngineComponents(800, 480, D3D_FEATURE_LEVEL_11_0)) {
		return(false);
	}

	if (!ConfigureRenderingSetup()) {
		return(false);
	}

	SetMultiThreadedMode(false);

	return(true);
}

void RenderApplication::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}