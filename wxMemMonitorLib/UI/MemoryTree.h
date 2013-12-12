//------------------------------------------------------------------------
// Name:    MemoryTree.h
// Author:  jjuiddong
// Date:    3/7/2013
// 
// show memory data
//------------------------------------------------------------------------
#pragma once
#include "wx/treectrl.h"

class wxTreeCtrl;
namespace memmonitor
{
	class CMemoryTree : public wxPanel
	{
		enum  {
			MENU_OPEN_PROPERTY = 300,
			ID_REFRESH_TIMER,
			REFRESH_INTERVAL = 60000, // 1 second
		};

	public:
		CMemoryTree(wxWindow *parent);
		virtual ~CMemoryTree();

	protected:
		bool LoadMemoryMap();
		bool UpdateMemoryMap();

		// Event Handler
		DECLARE_EVENT_TABLE()
		void OnSize(wxSizeEvent& event);
		void OnTreectrlSelChanged( wxTreeEvent& event );
		void OnContextMenu(wxContextMenuEvent& event);
		void OnMenuOpenProperty(wxCommandEvent& event);
		void OnRefreshTimer(wxTimerEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnDestroy(wxWindowDestroyEvent &event);

	protected:
		enum { ID_TREE };
		wxTreeCtrl *m_pTree;
		wxTimer	m_Timer;
		bool m_IsUpdate;
	};
}
