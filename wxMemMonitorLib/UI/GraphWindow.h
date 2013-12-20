/**
 @filename GraphWindow.h
 
	draw data structure  
*/
#pragma once
#include "../visualizer/VisualizerGlobal.h"
#include "../Control/Global.h"


namespace visualizer
{
	class CPropertyItemAdapter;


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// CStructureCircle
	class CStructureCircle
	{
	public:
		CStructureCircle() : m_pParent(NULL) {}
		virtual ~CStructureCircle();
		bool AddChild(CStructureCircle *circle);
		void Refresh();
		void DataCulling();

	public:
		CStructureCircle *m_pParent;
		string m_Name;
		string m_Value;
		string m_TypeName;
		STypeData m_TypeData;
		wxSize m_Size;
		list<CStructureCircle*> m_Children;
		GRAPH_ALIGN_TYPE m_ChildAlignType;
	};



	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// CGraphWindow
	class CGraphWindow : public wxScrolledWindow
	{
		enum  {
			ID_REFRESH_TIMER = 100,
			REFRESH_INTERVAL = 1000,
		};

		enum DISP_MODE {
			DISP_T_N_V,	// type, name, value
			DISP_SMALL_T_N_V,	// small type, name, value
			DISP_SMALL_V,	// small graph value
			DISP_MAX,
		};

	public:
		CGraphWindow(wxWindow *parent);
		virtual ~CGraphWindow();

		void UpdateSymbol( const string &symbolName, const string &varName, const visualizer::SSymbolInfo &symbol );
		CStructureCircle* AddDataGraph( CStructureCircle *parent, const std::string &valueName, 
			CPropertyItemAdapter &propAdapter,
			const SSymbolInfo *pSymbol, STypeData *pTypeData, const GRAPH_ALIGN_TYPE align );


	protected:
		DECLARE_EVENT_TABLE()
		void OnPaint(wxPaintEvent &event);
		void DrawCircle(wxPaintDC *pdc, CStructureCircle *circle, const wxPoint &pos, OUT wxPoint &boundary, const bool isLineDraw=false);
		void OnRefreshTimer(wxTimerEvent& event);
		void OnKeyDown(wxKeyEvent& event);


	private:
		string m_CurrentSymbolName;
		string m_VariableName;
		CStructureCircle *m_pRoot;
		wxPoint m_oldBoundary;
		DISP_MODE m_DispMode;

		bool m_IsShowHelp;
		wxTimer	m_Timer;
		int m_TimerInterval;
	};

}
