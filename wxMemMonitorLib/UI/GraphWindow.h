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


	// CStructureCircle
	class CStructureCircle
	{
	public:
		CStructureCircle() : m_pParent(NULL), m_Radian(15) {}
		virtual ~CStructureCircle();
		bool AddChild(CStructureCircle *circle);

		
	protected:


	private:
	public:
		CStructureCircle *m_pParent;
		string m_Name;
		string m_Value;
		string m_TypeName;
		wxPoint m_Pos;
		float m_Radian;
		wxSize m_Size;
		list<CStructureCircle*> m_Children;
		GRAPH_ALIGN_TYPE m_ChildAlignType;
	};


	// CGraphWindow
	class CGraphWindow : public wxScrolledWindow
	{
	public:
		CGraphWindow(wxWindow *parent);
		virtual ~CGraphWindow();

		void UpdateSymbol( const string &symbolName, const visualizer::SSymbolInfo &symbol );
		CStructureCircle* AddDataGraph( CStructureCircle *parent, const std::string &valueName, 
			CPropertyItemAdapter &propAdapter,
			const SSymbolInfo *pSymbol, STypeData *pTypeData, const GRAPH_ALIGN_TYPE align );


	protected:
		DECLARE_EVENT_TABLE()
		void OnPaint(wxPaintEvent &event);
		void DrawCircle(wxPaintDC *pdc, CStructureCircle *circle, const wxPoint &pos, OUT wxPoint &boundary, const bool isLineDraw=false);


	private:
		wxString	m_CurrentSymbolName;
		wxTimer	m_Timer;
		list<CStructureCircle*> m_Circles;
		wxPoint m_DrawPosBoundary;
		CStructureCircle *m_pRoot;
		wxPoint m_oldBoundary;
	};

}
