
#include "stdafx.h"
#include "GraphWindow.h"
#include "LogWindow.h"
#include "../dia/DiaWrapper.h"
#include "../visualizer/DefaultPropertyMaker.h"
#include "../visualizer/PropertyMaker.h"
#include "PropertyItemAdapter.h"
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


const int RECT_W = 50;
const int RECT_H = 30;
const int GAP_W = 10;
const int GAP_H = 10;


BEGIN_EVENT_TABLE(CGraphWindow, wxScrolledWindow)
	EVT_PAINT  (CGraphWindow::OnPaint)
END_EVENT_TABLE()

CGraphWindow::CGraphWindow(wxWindow *parent) : 
	wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition,wxDefaultSize)
,	m_DrawPosBoundary(20,20)
,	m_pRoot(NULL)
,	m_oldBoundary(0,0)
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
		//visualizer::SSymbolInfo sym;
		//AddDataGraph( NULL, "test",  &sym, NULL, GRAPH_ALIGN_VERT);
		//CStructureCircle *circle0 = AddDataGraph( NULL, "test1",  &sym, NULL, GRAPH_ALIGN_HORZ);
		//AddDataGraph( circle0, "test10",  &sym, NULL, GRAPH_ALIGN_VERT);
		//CStructureCircle *circle01 = AddDataGraph( circle0, "test11",  &sym, NULL, GRAPH_ALIGN_VERT);
		//AddDataGraph( circle01, "test110",  &sym, NULL, GRAPH_ALIGN_VERT);
		//CStructureCircle *circle010 = AddDataGraph( circle01, "test111",  &sym, NULL, GRAPH_ALIGN_HORZ);
		//AddDataGraph( circle010, "test1110",  &sym, NULL, GRAPH_ALIGN_HORZ);
		//AddDataGraph( circle01, "test112",  &sym, NULL, GRAPH_ALIGN_VERT);
		//AddDataGraph( circle01, "test113",  &sym, NULL, GRAPH_ALIGN_VERT);
		//CStructureCircle *circle02 = AddDataGraph( circle0, "test12",  &sym, NULL, GRAPH_ALIGN_VERT);

		//AddDataGraph( NULL, "test2",  &sym, NULL, GRAPH_ALIGN_VERT);
		//CStructureCircle *circle1 = AddDataGraph( NULL, "test3",  &sym, NULL, GRAPH_ALIGN_HORZ);
		//AddDataGraph( circle1, "test30",  &sym, NULL, GRAPH_ALIGN_VERT);
	}
}


/**
 @brief AddDataGraph
 @date 2013-12-17
 */
CStructureCircle* CGraphWindow::AddDataGraph( CStructureCircle *parent, const std::string &valueName, 
	CPropertyItemAdapter &propAdapter, 
	const SSymbolInfo *pSymbol, STypeData *pTypeData, const GRAPH_ALIGN_TYPE align )
{
	CStructureCircle *circle = new CStructureCircle();

	switch (align)
	{
	case GRAPH_ALIGN_VERT: m_DrawPosBoundary.y += 40; break;
	case GRAPH_ALIGN_HORZ: m_DrawPosBoundary.x += 40; break;
	}

	circle->m_Name = propAdapter.GetValueName();
	circle->m_TypeName = propAdapter.GetValueType();
	circle->m_Value = propAdapter.GetValue();
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

	if (m_oldBoundary != maxBoundary)
	{
		SetVirtualSize(maxBoundary.x+RECT_W+30, maxBoundary.y+RECT_H+30);
		SetScrollRate(10,10);
		Layout();
		m_oldBoundary = maxBoundary;
	}
}


/**
 @brief 
 @date 2013-12-17
*/
void CGraphWindow::DrawCircle(wxPaintDC *pdc, CStructureCircle *circle, const wxPoint &pos, OUT wxPoint &boundary, const bool isLineDraw)
{
	RET(!circle);
	const bool IsCircleDraw = !isLineDraw;

	// circle size, and text setting
	std::stringstream ss;
	ss << circle->m_TypeName << " " << circle->m_Name;
	if (!circle->m_Value.empty())
		ss << " : " << circle->m_Value;

	wxSize textSize;
	pdc->GetTextExtent(ss.str(), &textSize.x, &textSize.y);
	textSize.x = max(RECT_W, textSize.x+5);
	textSize.y = RECT_H;
	circle->m_Size = textSize;
	//

	if (IsCircleDraw && ("root" != circle->m_Name))
	{
		wxRect rect(0,0,textSize.x,textSize.y);
		rect.Offset(pos);
		pdc->DrawRoundedRectangle(rect, 5);
		pdc->DrawText(ss.str(), pos+wxPoint(2,5));
	}

	wxPoint nextPos = pos;
	if (!circle->m_Children.empty())
	{
		nextPos += wxPoint(RECT_W/2+GAP_W, RECT_H+GAP_H);
	}

	boundary.x = max(nextPos.x+textSize.x, boundary.x);
	boundary.y = max(nextPos.y+textSize.y, boundary.y);

	wxPoint oldPos = pos;
	BOOST_FOREACH (auto &child, circle->m_Children)
	{
		if (isLineDraw)
		{
			const wxPoint offset(RECT_W/2, RECT_H/2);
			pdc->DrawLine(oldPos+offset, nextPos+offset);
			oldPos = nextPos;
		}

		wxPoint maxBoundary(0,0);
		DrawCircle(pdc, child, nextPos, maxBoundary, isLineDraw);

		boundary.x = max(maxBoundary.x, boundary.x);
		boundary.y = max(maxBoundary.y, boundary.y);

		switch (circle->m_ChildAlignType)
		{
		case GRAPH_ALIGN_VERT: nextPos.y = boundary.y + GAP_H; break;
		case GRAPH_ALIGN_HORZ: nextPos.x = boundary.x + GAP_W; break;
		}
	}
}
