
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "../Control/Global.h"
#include "Frame.h"
#include <Shellapi.h>

using namespace memmonitor;

bool CApp::OnInit()
{
	SetExitOnFrameDelete(true);

	//get Cofig File Name from Command Line
	std::string configFileName = GetConfigFileName();
	if (configFileName.empty())
	{
		int nArgs;
		LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
		if (nArgs > 1)
		{
			configFileName = wstr2str(szArglist[ 1]);
			SetConfigFileName( configFileName );
		}
	}
	//

	wxFrame* frame = new CFrame(NULL);
	SetTopWindow(frame);
	frame->Show();
	return true;
}

void CApp::ExitMainLoop()
{
	// instead of existing wxWidgets main loop, terminate the MFC one
	//::PostQuitMessage(0);
	wxApp::ExitMainLoop();
}
