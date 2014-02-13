
#include "stdafx.h"
#include "Frame.h"
#include "MemoryTree.h"
#include "LogWindow.h"
#include "PropertyWindow.h"
#include "GraphWindow.h"
#include "../Control/Global.h"
#include "../dia/DiaWrapper.h"
#include "../visualizer/PropertyMaker.h"
#include "../memory/SharedMemoryMng.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi")


using namespace memmonitor;

enum MAINFRAME_MENU
{
	MENU_OPEN_AUTOEXP=100,
	MENU_RELOAD_AUTOEXP,
	MENU_EXIT,
	MENU_HELP,
};

BEGIN_EVENT_TABLE(CFrame, wxFrame)
	 EVT_MENU (MENU_OPEN_AUTOEXP, CFrame::OnMenuOpenAutoExp)
	 EVT_MENU (MENU_RELOAD_AUTOEXP, CFrame::OnMenuReloadAutoExp)
	 EVT_MENU (MENU_EXIT, CFrame::OnMenuExit)
	 EVT_MENU (MENU_HELP, CFrame::OnMenuHelp)
END_EVENT_TABLE()


CFrame::CFrame(wxWindow* parent) : wxFrame(parent, -1, _("wxMemMonitor"),
	wxDefaultPosition, wxSize(400,500), wxDEFAULT_FRAME_STYLE)
,	m_pMemTree(NULL)
,	m_pLogWnd(NULL)
,	m_pPropWnd(NULL)
{
	const std::pair<bool,string> initMemResult = InitMemoryMonitor( GetConfigFileName() );

	// notify wxAUI which frame to use
	m_mgr.SetManagedWindow(this);

	CMemoryTree* memTree = new CMemoryTree(this);
	CLogWindow *logWnd = new CLogWindow(this);
	visualizer::CPropertyWindow *propWnd = new visualizer::CPropertyWindow(this);

	// add the panes to the manager
	m_mgr.AddPane(logWnd, wxBOTTOM, wxT("Log Window"));
	m_mgr.AddPane(memTree, wxLEFT, wxT("Memory List"));
	m_mgr.AddPane(propWnd, wxCENTER, wxT("Properties"));

	// tell the manager to "commit" all the changes just made
	m_mgr.Update();

	CreateMenuBar();

	m_pMemTree = memTree;
	m_pLogWnd = logWnd;
	m_pPropWnd = propWnd;
		
	if (initMemResult.first)
		GetLogWindow()->PrintText( "pdb path : " + initMemResult.second + "\n" );
	else
		GetLogWindow()->PrintText( GetLastError() );

	//if (initMemResult.first)
	{
		visualizer::OpenVisualizerScript( "autoexp.txt" );
		ReadConfigFile( GetConfigFileName() );
		RepositioningWindow();
	}

	// TitleBar setting
	char moduleName[ MAX_PATH] = "";  
	GetModuleFileNameA(GethInstance(), moduleName, MAX_PATH);
	char *name = PathFindFileNameA(moduleName);
	SetTitle(name);

	Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(CFrame::OnKeyDown));

}

CFrame::~CFrame()
{
	WriteWindowPosition();
	// deinitialize the frame manager
	m_mgr.UnInit();
	dia::Cleanup();
	sharedmemory::Release();
	visualizer::Release();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CFrame::CreateMenuBar()
{
	// file Menu
	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append( MENU_OPEN_AUTOEXP, "&Open AutoExp" );
	fileMenu->Append( MENU_RELOAD_AUTOEXP, "&Reload AutoExp" );
	fileMenu->Append( MENU_EXIT, "&Exit" );

	// help menu
	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append( MENU_HELP, "&Help" );

	// menuBar
	wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );
	menuBar->Append(fileMenu, "&File");
	menuBar->Append(helpMenu, "&Help");
	SetMenuBar(menuBar);
}


//------------------------------------------------------------------------
// add PropertyWindow
//------------------------------------------------------------------------
bool CFrame::AddPropertyWindow( const wxString &symbolName )
{
	wxMiniFrame *pframe = new wxMiniFrame(this, -1, symbolName);
	pframe->SetWindowStyle(wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX);
	wxBoxSizer* itemBoxSizer = new wxBoxSizer(wxVERTICAL);
	pframe->SetSizer(itemBoxSizer);

	visualizer::CPropertyWindow *propWnd = new visualizer::CPropertyWindow(pframe);
	itemBoxSizer->Add(propWnd);
	propWnd->UpdateSymbol(symbolName);
	pframe->Show();

	pframe->Bind(wxEVT_CLOSE_WINDOW, &CFrame::OnPropertyFrameClose, this);

	m_PropFrames.push_back(pframe);
	return true;
}


/**
 @brief 
 @date 2013-12-16
*/
bool CFrame::AddGraphWindow(const string &symbolName, const string &varName, const visualizer::SSymbolInfo &symbol)
{
	wxMiniFrame *pframe = new wxMiniFrame(this, -1, varName, wxDefaultPosition, wxSize(400,400));
	pframe->SetWindowStyle(wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX);

	visualizer::CGraphWindow *pWnd = new visualizer::CGraphWindow(pframe);
	pWnd->UpdateSymbol(symbolName, varName, symbol);
	pframe->Show();
	pframe->Bind(wxEVT_CLOSE_WINDOW, &CFrame::OnPropertyFrameClose, this);

	m_PropFrames.push_back(pframe);
	return true;
}


/**
 @brief PropertyMiniFrame Close Event Handler
 */
void CFrame::OnPropertyFrameClose(wxCloseEvent& event)
{
	event.Skip();

	// remove close miniframe object
	BOOST_FOREACH(auto &propFrame, m_PropFrames)
	{
		if (propFrame == event.GetEventObject())
		{
			const wxString title = propFrame->GetTitle();  // debug
			m_PropFrames.remove(propFrame);
			break;
		}
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CFrame::OnMenuOpenAutoExp(wxCommandEvent& event)
{
	::WinExec( "notepad.exe autoexp.txt", SW_SHOW);
}


/**
 @brief 
 @date 2013-12-18
*/
void CFrame::OnMenuReloadAutoExp(wxCommandEvent& event)
{
	GetLogWindow()->PrintText( "Reload autoexp.txt\n" );
	visualizer::OpenVisualizerScript( "autoexp.txt" );
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CFrame::OnMenuExit(wxCommandEvent& event)
{
	Close(true);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CFrame::OnMenuHelp(wxCommandEvent& event)
{
}


/**
@brief  UpdatePaneSize
*/
void CFrame::UpdatePaneSize(wxWindow *pWindow, int w, int h)
{
	wxAuiPaneInfo& pane = m_mgr.GetPane(pWindow);
	pane.MinSize(w,h);
	m_mgr.Update();
	pane.MinSize(10,10);
}


/**
@brief  OnKeyDown
*/
void CFrame::OnKeyDown(wxKeyEvent& event)
{
	event.Skip();
	if (346 == event.GetKeyCode()) // VK_F7
	{
		Hide();
	}
}
