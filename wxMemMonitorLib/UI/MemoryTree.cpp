
#include "stdafx.h"
#include "MemoryTree.h"
#include "wx/treectrl.h"
#include "PropertyWindow.h"
#include "../Control/Global.h"
#include "Frame.h"

using namespace memmonitor;

BEGIN_EVENT_TABLE( memmonitor::CMemoryTree, wxPanel )
	EVT_SIZE(CMemoryTree::OnSize)
	EVT_TREE_SEL_CHANGED(ID_TREE, OnTreectrlSelChanged)
	EVT_CONTEXT_MENU(CMemoryTree::OnContextMenu)
	EVT_MENU(MENU_OPEN_PROPERTY, CMemoryTree::OnMenuOpenProperty)
	EVT_TIMER(ID_REFRESH_TIMER, CMemoryTree::OnRefreshTimer)
	EVT_WINDOW_DESTROY(CMemoryTree::OnDestroy)
END_EVENT_TABLE()


CMemoryTree::CMemoryTree(wxWindow *parent) :
	wxPanel(parent, -1, wxDefaultPosition, wxSize(200, 400) )
{
	m_pTree = new wxTreeCtrl(this, ID_TREE, wxDefaultPosition, wxSize(200,400),
		wxTR_HAS_BUTTONS 
		|wxTR_LINES_AT_ROOT
		|wxTR_HIDE_ROOT
		|wxTR_ROW_LINES|wxTR_SINGLE |
		wxTR_FULL_ROW_HIGHLIGHT
		);
	m_pTree->SetBackgroundColour(wxColour(237,237,237));

	UpdateMemoryMap();

	//m_Timer.SetOwner(this, ID_REFRESH_TIMER);
	//m_Timer.Start( REFRESH_INTERVAL );

	Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(CMemoryTree::OnKeyDown));
}

CMemoryTree::~CMemoryTree()
{

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CMemoryTree::UpdateMemoryMap()
{
	RETV(!m_pTree, false);

	m_IsUpdate = true;

	const wxTreeItemId selectId = m_pTree->GetSelection();
	wxString selectItemName;
	if (selectId.IsOk())
		selectItemName = m_pTree->GetItemText(selectId);

	m_pTree->DeleteAllItems();

	wxTreeItemId rootId = m_pTree->AddRoot(wxT("@Root"));
	m_pTree->AppendItem( rootId, "--- Memory List ---" ); // empty item

	MemoryList memList;
	EnumerateMemoryInfo(memList);
	BOOST_FOREACH(SMemInfo &info, memList)
	{
		wxTreeItemId itemId = m_pTree->AppendItem( rootId, info.name );
		std::stringstream ss;
		ss << "size: " << info.size;
		m_pTree->AppendItem( itemId, ss.str()); //wxString::Format("size: %d", info.size) );
		m_pTree->AppendItem( itemId, wxString::Format("ptr: 0x%x", (DWORD)info.ptr) );

		if (selectItemName == info.name)
			m_pTree->SelectItem(itemId);
	}
	//m_pTree->SortChildren(rootId);
	//m_pTree->Expand(rootId);
	m_IsUpdate = false;

	return true;
}


//------------------------------------------------------------------------
// Property Refresh Timer
//------------------------------------------------------------------------
void CMemoryTree::OnRefreshTimer(wxTimerEvent& event)
{
	UpdateMemoryMap();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CMemoryTree::OnTreectrlSelChanged( wxTreeEvent& event )
{
	if (m_IsUpdate)
		return;

	m_pTree->SetItemTextColour( event.GetItem(), wxColour(0,0,255) );
	if (event.GetOldItem())
		m_pTree->SetItemTextColour( event.GetOldItem(), wxColour(0,0,0) );

	const wxString text = m_pTree->GetItemText( event.GetItem() );

	PropWindowPtr pPropWnd = GetPropertyWindow();
	RET(!pPropWnd);
	pPropWnd->UpdateSymbol( text );
}


//------------------------------------------------------------------------
// Open Popup Menu
//------------------------------------------------------------------------
void CMemoryTree::OnContextMenu(wxContextMenuEvent& event)
{
	wxPoint point = event.GetPosition();
	point = ScreenToClient(point);

	wxMenu menu;
	menu.Append(MENU_OPEN_PROPERTY, wxT("&Open Property"));
	PopupMenu(&menu, point);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CMemoryTree::OnMenuOpenProperty(wxCommandEvent& WXUNUSED(event))
{
	const wxString text = m_pTree->GetItemText(m_pTree->GetSelection());
	GetFrame()->AddPropertyWindow( text );
}


/**
 @brief KeyDown Event Handler
 */
void CMemoryTree::OnKeyDown(wxKeyEvent& event)
{
	event.Skip();
	if (344 == event.GetKeyCode()) // F5
	{
		UpdateMemoryMap();
	}

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CMemoryTree::OnDestroy(wxWindowDestroyEvent &event)
{
	m_Timer.Stop();
}


/**
 @brief Resize Event Handler
 */
void CMemoryTree::OnSize(wxSizeEvent& event)
{
	const wxRect r = GetSize();
	if (m_pTree)
		m_pTree->SetSize(r);
}
