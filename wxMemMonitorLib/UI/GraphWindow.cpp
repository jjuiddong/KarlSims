
#include "stdafx.h"
#include "GraphWindow.h"
#include "LogWindow.h"
#include "../dia/DiaWrapper.h"
#include "../visualizer/DefaultPropertyMaker.h"
#include "../visualizer/PropertyMaker.h"
#include "Frame.h"
#include "../Control/Global.h"


using namespace memmonitor;
using namespace dia;
using namespace visualizer;



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CStructureCircle
//////////////////////////////////////////////////////////////////////////////////////////////////////////

CStructureCircle::~CStructureCircle()
{
	BOOST_FOREACH (auto &child, m_Children)
	{
		delete child;
	}
	m_Children.clear();
}


/**
 @brief 
 @date 2013-12-17
*/
bool CStructureCircle::AddChild(CStructureCircle *circle)
{
	RETV(!circle, false);
	circle->m_pParent = this;
	m_Children.push_back(circle);
	return true;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGraphWindow
//////////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CGraphWindow, wxScrolledWindow)
	EVT_PAINT  (CGraphWindow::OnPaint)
END_EVENT_TABLE()

CGraphWindow::CGraphWindow(wxWindow *parent) : 
	wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition,wxDefaultSize)
,	m_DrawPosBoundary(20,20)
,	m_pRoot(NULL)
{
	//SetBackgroundColour(wxColour(0,100,0));
}

CGraphWindow::~CGraphWindow()
{
	SAFE_DELETE(m_pRoot);
}


/**
 @brief UpdateSymbol
 @date 2013-12-16
*/
void CGraphWindow::UpdateSymbol( const string &symbolName, const visualizer::SSymbolInfo &symbol )
{
	if (m_CurrentSymbolName == symbolName)
		return;
	if ((symbolName == "@Root") || (symbolName == "--- Memory List ---"))
		return;

	GetLogWindow()->PrintText( "GVis UpdateSymbol = " +  symbolName  + "\n" );

	visualizer::MakePropertyChild_DefaultForm( visualizer::SVisDispDesc(NULL, NULL, this, NULL), symbol, true, 2 );

	// test code
	if (0)
	{
		visualizer::SSymbolInfo sym;
		AddDataGraph( NULL, "test",  &sym, NULL, GRAPH_ALIGN_VERT);
		CStructureCircle *circle0 = AddDataGraph( NULL, "test1",  &sym, NULL, GRAPH_ALIGN_HORZ);
		AddDataGraph( circle0, "test10",  &sym, NULL, GRAPH_ALIGN_VERT);
		CStructureCircle *circle01 = AddDataGraph( circle0, "test11",  &sym, NULL, GRAPH_ALIGN_VERT);
		AddDataGraph( circle01, "test110",  &sym, NULL, GRAPH_ALIGN_VERT);
		CStructureCircle *circle010 = AddDataGraph( circle01, "test111",  &sym, NULL, GRAPH_ALIGN_HORZ);
		AddDataGraph( circle010, "test1110",  &sym, NULL, GRAPH_ALIGN_HORZ);
		AddDataGraph( circle01, "test112",  &sym, NULL, GRAPH_ALIGN_VERT);
		AddDataGraph( circle01, "test113",  &sym, NULL, GRAPH_ALIGN_VERT);
		CStructureCircle *circle02 = AddDataGraph( circle0, "test12",  &sym, NULL, GRAPH_ALIGN_VERT);

		AddDataGraph( NULL, "test2",  &sym, NULL, GRAPH_ALIGN_VERT);
		CStructureCircle *circle1 = AddDataGraph( NULL, "test3",  &sym, NULL, GRAPH_ALIGN_HORZ);
		AddDataGraph( circle1, "test30",  &sym, NULL, GRAPH_ALIGN_VERT);
	}
}


/**
 @brief AddDataGraph
 @date 2013-12-17
 */
CStructureCircle* CGraphWindow::AddDataGraph( CStructureCircle *parent, const std::string &valueName, 
	const visualizer::SSymbolInfo *pSymbol, STypeData *pTypeData, const GRAPH_ALIGN_TYPE align )
{
	CStructureCircle *circle = new CStructureCircle();

	switch (align)
	{
	case GRAPH_ALIGN_VERT: m_DrawPosBoundary.y += 40; break;
	case GRAPH_ALIGN_HORZ: m_DrawPosBoundary.x += 40; break;
	}

	circle->m_Name = valueName;
	circle->m_Pos = m_DrawPosBoundary;
	circle->m_ChildAlignType = align;
	m_Circles.push_back(circle);

	if (parent)
	{
		parent->AddChild(circle);
	}
	else
	{
		if (!m_pRoot)
		{
			m_pRoot = new CStructureCircle();
			m_pRoot->m_Name = "root";
			m_pRoot->m_ChildAlignType = GRAPH_ALIGN_VERT;
		}
		m_pRoot->AddChild(circle);
	}

	return circle;
}


/**
 @brief 
 @date 2013-12-17
*/
void CGraphWindow::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

	DrawCircle(&dc, m_pRoot, wxPoint(10,10), wxPoint(0,0), true);
	wxPoint maxBoundary;
	DrawCircle(&dc, m_pRoot, wxPoint(10,10), maxBoundary);

	static wxPoint oldBoundary;
	if (oldBoundary != maxBoundary)
	{
		SetVirtualSize(maxBoundary.x, maxBoundary.y);
		SetScrollRate(10,10);
		Layout();
		oldBoundary = maxBoundary;
	}
}


/**
 @brief 
 @date 2013-12-17
*/
void CGraphWindow::DrawCircle(wxPaintDC *pdc, CStructureCircle *circle, const wxPoint &pos, OUT wxPoint &boundary, const bool isLineDraw)
{
	RET(!circle);

	const int RECT_W = 50;
	const int RECT_H = 30;
	const int GAP_W = 10;
	const int GAP_H = 10;

	if (!isLineDraw && ("root" != circle->m_Name))
	{
		wxRect rect(0,0,RECT_W,RECT_H);//circle->m_Radian, circle->m_Radian);
		rect.Offset(pos);
		pdc->DrawRoundedRectangle(rect, 5);
		pdc->DrawText(circle->m_Name, pos+wxPoint(2,5));
	}

	wxPoint nextPos = pos;
	if (!circle->m_Children.empty())
	{
		switch (circle->m_ChildAlignType)
		{
		case GRAPH_ALIGN_VERT: nextPos += wxPoint(15,0); break;
		case GRAPH_ALIGN_HORZ: nextPos += wxPoint(0,15); break;
		}
	}

	boundary.x = max(nextPos.x, boundary.x);
	boundary.y = max(nextPos.y, boundary.y);

	wxPoint oldPos = pos;
	BOOST_FOREACH (auto &child, circle->m_Children)
	{
		switch (circle->m_ChildAlignType)
		{
		case GRAPH_ALIGN_VERT: nextPos.y = boundary.y + RECT_H+GAP_H; break;
		case GRAPH_ALIGN_HORZ: nextPos.x = boundary.x + RECT_W+GAP_W; break;
		}

		if (isLineDraw)
		{
			pdc->DrawLine(oldPos+wxPoint(15,15), nextPos+wxPoint(15,15));
			oldPos = nextPos;
		}

		wxPoint maxBoundary(0,0);
		DrawCircle(pdc, child, nextPos, maxBoundary, isLineDraw);

		boundary.x = max(maxBoundary.x, boundary.x);
		boundary.y = max(maxBoundary.y, boundary.y);
	}
}
