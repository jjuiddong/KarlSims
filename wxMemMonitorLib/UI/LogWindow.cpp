
#include "stdafx.h"
#include "LogWindow.h"

using namespace  memmonitor;

BEGIN_EVENT_TABLE( memmonitor::CLogWindow, wxTextCtrl )
	EVT_CONTEXT_MENU(CLogWindow::OnContextMenu)
	EVT_MENU(MENU_CLEAR, CLogWindow::OnMenuClear)
END_EVENT_TABLE()


CLogWindow::CLogWindow(wxWindow *parent) :
	wxTextCtrl(parent, -1, wxT("LogWindow\n"), wxDefaultPosition, wxSize(200,150),
		wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER)
{
	SetBackgroundColour(wxColour(237,237,237));
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CLogWindow::PrintText( const wxString &msg )
{
	AppendText( msg );
}


/**
 @brief 
 */
void CLogWindow::OnContextMenu(wxContextMenuEvent& event)
{
	wxPoint point = event.GetPosition();
	point = ScreenToClient(point);

	wxMenu menu;
	menu.Append(MENU_CLEAR, wxT("&Clear"));
	PopupMenu(&menu, point);
}


/**
 @brief 
 */
void CLogWindow::OnMenuClear(wxCommandEvent& event)
{
	Clear();

}
