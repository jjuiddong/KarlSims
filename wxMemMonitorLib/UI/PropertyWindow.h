//------------------------------------------------------------------------
// Name:    PropertyWindow.h
// Author:  jjuiddong
// Date:    3/10/2013
// 
// show memory data
//------------------------------------------------------------------------
#pragma once
#include <wx/propgrid/manager.h>
#include "../Control/Global.h"



namespace visualizer
{
	using namespace memmonitor;
	struct SSymbolInfo;
	class CPropertyItemAdapter;


	class CPropertyWindow : public wxPropertyGridManager
	{
		enum  {
			MENU_OPEN_PROPERTY=500,
			MENU_OPEN_GRAPH,

			ID_REFRESH_TIMER = 100,
			REFRESH_INTERVAL = 1000,
		};


	public:
		struct SPropItem
		{
			std::string typeName;		// symbol type name
			std::string symbolTypeName;		// dia symbol type name
			STypeData typeData;
		};
		typedef std::list<SPropItem*> PropList;

		CPropertyWindow(wxWindow *parent);
		virtual ~CPropertyWindow();
		void UpdateSymbol( const wxString &symbolName );
		wxPGProperty* AddProperty(wxPGProperty *pParentProp, CPropertyItemAdapter &propAdapter, 
			const visualizer::SSymbolInfo *pSymbol, STypeData *pTypeData);
		void	RemoveChildProperty( wxPGProperty *pParentProp );


	protected:
		void CheckSymbol( const wxString &symbolName );
		bool	FindSymbolUpward( wxPGProperty *pProp, OUT visualizer::SSymbolInfo *pOut );
		void	RefreshPropertyItem( wxPGProperty *pProp );
		void RemovePropClientData( wxPGProperty *pParentProp );
		void ClearPropItem();

		// Event Handler
		DECLARE_EVENT_TABLE()
		void OnSize(wxSizeEvent& event);
		void OnPropertyGridChange( wxPropertyGridEvent& event );
		void OnPropertyGridSelect( wxPropertyGridEvent& event );
		void OnContextMenu(wxContextMenuEvent& event);
		void OnMenuOpenProperty(wxCommandEvent& event);
		void OnMenuOpenGraph(wxCommandEvent& event);
		void OnRefreshTimer(wxTimerEvent& event);
		void OnKeyDown(wxKeyEvent& event);


	private:
		wxString	m_CurrentSymbolName;
		wxTimer	m_Timer;
		PropList	m_PropList;
	};
}
