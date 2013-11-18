
#include "stdafx.h"
#include "wx/dynlib.h"
#include "../wxMemMonitor.h"
#include "wx/evtloop.h"
#include "Global.h"
#include "../ui/LogWindow.h"
#include "../dia/DiaWrapper.h"


namespace memmonitor
{
	/// from wxWidgets sdk_exe sample code and my_dll sample code
	bool run_dll(EXECUTE_TYPE type, HINSTANCE hInst, const std::string configFileName);
	unsigned wxSTDCALL MyAppLauncher(void* event);
	wxCriticalSection gs_wxStartupCS;
	HANDLE gs_wxMainThread = NULL;
	static const int CMD_SHOW_WINDOW = wxNewId();
	static const int CMD_TERMINATE = wxNewId();

	static const bool g_IsThreadRunning = true;
}


using namespace memmonitor;

CApp::CApp()
{
	Connect(CMD_TERMINATE, wxEVT_THREAD, wxThreadEventHandler(CApp::OnTerminate));
}
void CApp::OnTerminate(wxThreadEvent& WXUNUSED(event))
{
	ExitMainLoop();
}


//------------------------------------------------------------------------
// load config file
// init dia library
// init shared memory
//------------------------------------------------------------------------
bool memmonitor::Init(EXECUTE_TYPE type, HINSTANCE hInst, const std::string configFileName)
{
	SetExecuteType(type);
	SetConfigFileName(configFileName);
	SethInstance(hInst);

	if (g_IsThreadRunning)
	{
		if (INNER_PROCESS == type)
			return run_dll(type, hInst, configFileName);
	}
	else
	{
		if (INNER_PROCESS == type)
		{
			WXDLLIMPEXP_BASE void wxSetInstance(HINSTANCE hInst);
			wxSetInstance(hInst);
			wxApp::m_nCmdShow = SW_SHOW;
			int argc = 0;
			wxChar **argv = NULL;
			wxEntryStart(argc, argv);
			if ( !wxTheApp || !wxTheApp->CallOnInit() )
				return false;
		}
		return (GetLastError().empty()? true : false);
	}

	return true;
}


/**
 @brief Show
 */
bool memmonitor::ShowToggle()
{
	if (wxTheApp)
	{
		if (wxTheApp->GetTopWindow())
		{
			if (wxTheApp->GetTopWindow()->IsShown())
				wxTheApp->GetTopWindow()->Hide();
			else
				wxTheApp->GetTopWindow()->Show(true);
		}
	}	
	return true;
}


//------------------------------------------------------------------------
// must call this message loop function
//------------------------------------------------------------------------
//void memmonitor::Loop(MSG &msg)
//{
//	if (!IsThreadRunning)
//	{
//		wxEventLoop * const
//			evtLoop = static_cast<wxEventLoop *>(wxEventLoop::GetActive());
//		if (evtLoop && evtLoop->PreProcessMessage(&msg))
//				return;
//		if (wxTheApp) 
//			wxTheApp->ProcessIdle(); // 이 함수를 호출해야 wxAuiManager Docking이 작동한다.
//	}
//}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void memmonitor::Cleanup()
{
	if (g_IsThreadRunning)
	{
		wxCriticalSectionLocker lock(gs_wxStartupCS);

		if (gs_wxMainThread)
		{
			// If wx main thread is running, we need to stop it. To accomplish this,
			// send a message telling it to terminate the app.
			if (wxApp::GetInstance())
			{
				wxThreadEvent *event =
					new wxThreadEvent(wxEVT_THREAD, CMD_TERMINATE);
				wxQueueEvent(wxApp::GetInstance(), event);
			}

			// We must then wait for the thread to actually terminate.
			WaitForSingleObject(gs_wxMainThread, INFINITE);
			CloseHandle(gs_wxMainThread);
			gs_wxMainThread = NULL;
		}	
	}

	if (INNER_PROCESS == GetExecuteType())
	{
		wxEntryCleanup();
	}

	memmonitor::Clear();
}


//------------------------------------------------------------------------
// return last error message
//------------------------------------------------------------------------
const std::string& memmonitor::GetLastError()
{
	return GetErrorMsg();
}


/**
 @brief 
 */
bool memmonitor::run_dll(EXECUTE_TYPE type, HINSTANCE hInst, const std::string configFileName)
{
	wxCriticalSectionLocker lock(gs_wxStartupCS);

	if ( !gs_wxMainThread )
	{
		HANDLE hEvent = CreateEvent
			(
			NULL,  // default security attributes
			FALSE, // auto-reset
			FALSE, // initially non-signaled
			NULL   // anonymous
			);
		if ( !hEvent )
			return false; // error

		// NB: If your compiler doesn't have _beginthreadex(), use CreateThread()
		gs_wxMainThread = (HANDLE)_beginthreadex
			(
			NULL,           // default security
			0,              // default stack size
			&MyAppLauncher,
			&hEvent,        // arguments
			0,              // create running
			NULL
			);

		if ( !gs_wxMainThread )
		{
			CloseHandle(hEvent);
			return false; // error
		}

		// Wait until MyAppLauncher signals us that wx was initialized. This
		// is because we use wxMessageQueue<> and wxString later and so must
		// be sure that they are in working state.
		WaitForSingleObject(hEvent, INFINITE);
		CloseHandle(hEvent);
	}

	// Send a message to wx thread to show a new frame:
	//wxThreadEvent *event =
	//	new wxThreadEvent(wxEVT_THREAD, CMD_SHOW_WINDOW);
	//event->SetString( "title" );
	//wxQueueEvent(wxApp::GetInstance(), event);

	return true;
}


//  wx application startup code -- runs from its own thread
unsigned wxSTDCALL memmonitor::MyAppLauncher(void* event)
{
	// Note: The thread that called run_wx_gui_from_dll() holds gs_wxStartupCS
	//       at this point and won't release it until we signal it.

	// We need to pass correct HINSTANCE to wxEntry() and the right value is
	// HINSTANCE of this DLL, not of the main .exe, use this MSW-specific wx
	// function to get it. Notice that under Windows XP and later the name is
	// not needed/used as we retrieve the DLL handle from an address inside it
	// but you do need to use the correct name for this code to work with older
	// systems as well.
	const HINSTANCE
	hInstance = wxDynamicLibrary::MSWGetModuleHandle("wxMemMonitorLib",
		&gs_wxMainThread);
	if ( !hInstance )
		return 0; // failed to get DLL's handle

	// IMPLEMENT_WXWIN_MAIN does this as the first thing
	wxDISABLE_DEBUG_SUPPORT();

	// We do this before wxEntry() explicitly, even though wxEntry() would
	// do it too, so that we know when wx is initialized and can signal
	// run_wx_gui_from_dll() about it *before* starting the event loop.
	wxInitializer wxinit;
	if ( !wxinit.IsOk() )
		return 0; // failed to init wx

	// Signal run_wx_gui_from_dll() that it can continue
	HANDLE hEvent = *(static_cast<HANDLE*>(event));
	if ( !SetEvent(hEvent) )
		return 0; // failed setting up the mutex

	// Run the app:
	wxEntry(hInstance);

	return 1;
}
