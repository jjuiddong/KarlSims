//------------------------------------------------------------------------
// Name:    LogWindow.h
// Author:  jjuiddong
// Date:    2013-03-07
// 
// 
//------------------------------------------------------------------------
#pragma once

namespace memmonitor
{
	class CLogWindow : public wxTextCtrl
	{
		enum
		{
			MENU_CLEAR=200,
		};

	public:
		CLogWindow(wxWindow *parent);
		virtual ~CLogWindow() {}
		void PrintText( const wxString &msg );

	protected:
		// Event Handler
		DECLARE_EVENT_TABLE()
		void OnContextMenu(wxContextMenuEvent& event);
		void OnMenuClear(wxCommandEvent& event);

	};
}
