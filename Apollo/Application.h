#pragma once

#include "IEventListener.h"
#include "IWindowProc.h"
#include "Singleton.h"
#include "EventManager.h"

namespace Apollo
{
	class Application : public IEventListener, public IWindowProc , public SingletonEx<Application>
	{
	public:
		Application();
		virtual ~Application();

		// Initialization functions
		//static Application* GetApplication();

		// Overloadable functions for end user
		virtual bool ConfigureCommandLine(LPSTR lpcmdline);
		virtual bool ConfigureEngineComponents() = 0;
		virtual void ShutdownEngineComponents() = 0;
		virtual void Initialize() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
		virtual void MessageLoop();
		virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		virtual void BeforeRegisterWindowClass(WNDCLASSEX &wc);

		virtual bool HandleEvent(EventPtr pEvent);

		// Request an exit from windows
		void RequestTermination();
		virtual void TakeScreenShot() = 0;

		// Helpers
		//Timer* m_pTimer;

		// Engine Components
		EventManager EvtManager;

		//Scene* m_pScene;

		bool m_bSaveScreenshot;
		bool m_bLoop;

	protected:
		// Application pointer to ensure single instance
		//static Application* ms_pApplication;
	};
}