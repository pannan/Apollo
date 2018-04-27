#pragma once

#include "Application.h"
#include "ResourceProxyDX11.h"
namespace Apollo
{
	class Camera;
	class Win32RenderWindow;
	class RendererDX11;


	class RenderApplication : public Application
	{

	public:
		RenderApplication();
		virtual ~RenderApplication();

	public:
		virtual bool ConfigureRenderingEngineComponents(UINT width, UINT height, D3D_FEATURE_LEVEL desiredLevel, D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE);
		virtual bool ConfigureRenderingSetup() { return true; }

		virtual void ShutdownRenderingEngineComponents();
		virtual void ShutdownRenderingSetup();

		virtual void HandleWindowResize(HWND handle, UINT width, UINT height);

		virtual Win32RenderWindow* CreateRenderWindow();

		virtual bool ConfigureEngineComponents();

		virtual void ShutdownEngineComponents();
	
		virtual void Initialize() {}
		virtual void Update() {}
		virtual void Shutdown(){}


		// Request an exit from windows
		void RequestTermination();
		virtual void TakeScreenShot() {}

		void ToggleMultiThreadedMode();
		void SetMultiThreadedMode(bool mode);
	//	bool GetMultiThreadedMode();

	//	virtual void TakeScreenShot();
		virtual void SetScreenShotName(const std::wstring name);

		virtual std::wstring GetName() { return L"RenderAPP"; }
		virtual bool HandleEvent(EventPtr pEvent) { return false; }

	protected:

		RendererDX11*			m_pRenderer11;
		Win32RenderWindow*		m_pWindow;

		UINT					m_iWidth;
		UINT					m_iHeight;

		ResourcePtr				m_BackBuffer;

		//SceneRenderTask*		m_pRenderView;
		//ViewTextOverlay*		m_pTextOverlayView;

		bool					m_bMultithreadedMode;

		std::wstring			m_ScreenShotName;

	public:

		/*enum class InputMode
		{
			Console,
			Camera
		};*/

		// For the camera, we have a reference to the object itself, and
		// also provide another event manager for it to use.  This allows
		// us to control when the camera receives events and when they 
		// should be passed to a different object.

		EventManager			CameraEventHub;
		Camera*					m_pCamera;

		EventManager			ConsoleEventHub;
		//ConsoleActor*			m_pConsole;

		//InputMode				m_InputMode;
	};
}
