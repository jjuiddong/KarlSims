
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


/**
 @brief refresh
 @date 2013-12-18
*/
void CStructureCircle::Refresh()
{
	if ( (SymTagData == m_TypeData.symtag 
		|| SymTagBaseType == m_TypeData.symtag 
		||  SymTagEnum == m_TypeData.symtag 
		)
		&& m_TypeData.ptr
		&& m_TypeData.vt != VT_EMPTY )
	{
		_variant_t var = dia::GetValue(m_TypeData.ptr, m_TypeData.vt);
		wxVariant wxVar = memmonitor::Variant2wxVariant(var);
		// bool 형일 때, enum 형태의 값으로 바꿔주어야 한다. 
		if (var.vt == VT_BOOL)
			wxVar = wxVariant((int)(var.bVal? true : false)); // bool 값은 widgets에서는 0/1 값이어야 한다.
		wxString str = wxVar;
		m_Value = str;
		DataCulling();
	}

	BOOST_FOREACH (auto &child, m_Children)
		child->Refresh();
}


/**
 @brief double, float display 5digit
 @date 2013-12-20
*/
void CStructureCircle::DataCulling()
{
	if (boost::iequals(m_TypeName, "double") || boost::iequals(m_TypeName, "float"))
	{
		const double f = atof(m_Value.c_str());
		m_Value = format("%.4f", f);
	}
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
	EVT_TIMER(ID_REFRESH_TIMER, CGraphWindow::OnRefreshTimer)
END_EVENT_TABLE()

CGraphWindow::CGraphWindow(wxWindow *parent) : 
	wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition,wxDefaultSize)
,	m_pRoot(NULL)
,	m_oldBoundary(0,0)
,	m_DispMode(DISP_T_N_V)
,	m_TimerInterval(REFRESH_INTERVAL)
,	m_IsShowHelp(false)
{
	m_Timer.SetOwner(this, ID_REFRESH_TIMER);
	m_Timer.Start( m_TimerInterval );

	Connect(wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(CGraphWindow::OnKeyDown), (wxObject*)0, this);
}

CGraphWindow::~CGraphWindow()
{
	SAFE_DELETE(m_pRoot);
}


/**
 @brief UpdateSymbol
 @date 2013-12-16
*/
void CGraphWindow::UpdateSymbol( const string &symbolName, const string &varName, const visualizer::SSymbolInfo &symbol )
{
	if (m_CurrentSymbolName == symbolName)
		return;
	if ((symbolName == "@Root") || (symbolName == "--- Memory List ---"))
		return;

	m_VariableName = varName;
	GetLogWindow()->PrintText( "GVis UpdateSymbol = " +  symbolName  + "\n" );

	visualizer::MakePropertyChild_DefaultForm( visualizer::SVisDispDesc(NULL, NULL, this, NULL), symbol, true, 2 );



}


/**
 @brief AddDataGraph
 @date 2013-12-17
 */
CStructureCircle* CGraphWindow::AddDataGraph( CStructureCircle *parent, const std::string &valueName, 
	CPropertyItemAdapter &propAdapter, const SSymbolInfo *pSymbol, STypeData *pTypeData, const GRAPH_ALIGN_TYPE align )
{
	CStructureCircle *circle = new CStructureCircle();

	circle->m_Name = propAdapter.GetValueName();
	circle->m_TypeName = propAdapter.GetValueType();
	circle->m_Value = propAdapter.GetValue();
	circle->m_ChildAlignType = align;

	if (pTypeData)
		circle->m_TypeData = *pTypeData;
	if (pSymbol)
		circle->m_TypeData.ptr = pSymbol->mem.ptr;

	circle->DataCulling();

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

	int y = 0;
	dc.DrawText(m_VariableName, wxPoint(10,y));
	dc.DrawText(format("'H' Key Down to Show/Hide Help Information"), wxPoint(10,y+=17));
	if (m_IsShowHelp)
	{
		dc.DrawText(format("Refresh Timer Interval : %d", m_TimerInterval), wxPoint(10,y+=17));
		dc.DrawText("- F5 Key Refresh", wxPoint(10, y+=17));
		dc.DrawText("- ,< Key Refresh Timer Fast, ", wxPoint(10, y+=17));
		dc.DrawText("- .> Key Refresh Timer Slow", wxPoint(10, y+=17));
		switch (m_DispMode)
		{
		case DISP_T_N_V: dc.DrawText("- Tab Key Change Display Mode : Type, Name, Value", wxPoint(10, y+=17)); break;
		case DISP_SMALL_T_N_V: dc.DrawText("- Tab Key Change Display Mode : Samll Graph Type, Name, Value", wxPoint(10, y+=17)); break;
		case DISP_SMALL_V: dc.DrawText("- Tab Key Change Display Mode : Small Graph Value", wxPoint(10, y+=17)); break;
		}
	}

	DrawCircle(&dc, m_pRoot, wxPoint(10,y+17), wxPoint(0,0), true);
	wxPoint maxBoundary;
	DrawCircle(&dc, m_pRoot, wxPoint(10,y+17), maxBoundary);

	if (m_oldBoundary != maxBoundary)
	{
		SetVirtualSize(maxBoundary.x+30, maxBoundary.y+30);
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

	switch (m_DispMode)
	{
	case DISP_T_N_V: 
	case DISP_SMALL_T_N_V:
		ss << circle->m_TypeName << " " << circle->m_Name;
		if (!circle->m_Value.empty())
			ss << " : " << circle->m_Value;
		break;
	case DISP_SMALL_V:
		ss << circle->m_Value;
		break;
	}

	wxSize CircleSize(RECT_W, RECT_H);
	wxSize textPosOffset(2,5);
	wxSize gapSize(GAP_W, GAP_H);
	if ((m_DispMode == DISP_SMALL_V) || (m_DispMode == DISP_SMALL_T_N_V))
	{
		CircleSize = wxSize(5,20);
		textPosOffset = wxSize(2,2);
		gapSize = wxSize(GAP_W/2, GAP_H/2);

		if (ss.str().empty())
			CircleSize = wxSize(3,3);
	}

	wxSize textSize;
	pdc->GetTextExtent(ss.str(), &textSize.x, &textSize.y);
	textSize.x = max(CircleSize.x, textSize.x+5);
	textSize.y = CircleSize.y;
	circle->m_Size = textSize;
	//

	if (IsCircleDraw && ("root" != circle->m_Name))
	{
		wxRect rect(0,0,textSize.x,textSize.y);
		rect.Offset(pos);
		pdc->DrawRoundedRectangle(rect, 5);
		pdc->DrawText(ss.str(), pos+textPosOffset);
	}

	wxPoint nextPos = pos;
	if (!circle->m_Children.empty())
	{
		nextPos += (wxPoint(CircleSize.x/2, CircleSize.y) + gapSize);
	}

	boundary.x = max(nextPos.x+textSize.x, boundary.x);
	boundary.y = max(nextPos.y+textSize.y, boundary.y);

	wxPoint oldPos = pos;
	BOOST_FOREACH (auto &child, circle->m_Children)
	{
		if (isLineDraw)
		{
			const wxPoint offset(CircleSize.x/2, CircleSize.y/2);
			pdc->DrawLine(oldPos+offset, nextPos+offset);
			oldPos = nextPos;
		}

		wxPoint maxBoundary(0,0);
		DrawCircle(pdc, child, nextPos, maxBoundary, isLineDraw);

		boundary.x = max(maxBoundary.x, boundary.x);
		boundary.y = max(maxBoundary.y, boundary.y);

		switch (circle->m_ChildAlignType)
		{
		case GRAPH_ALIGN_VERT: nextPos.y = boundary.y + gapSize.y; break;
		case GRAPH_ALIGN_HORZ: nextPos.x = boundary.x + gapSize.x; break;
		}
	}
}


/**
 @brief 
 @date 2013-12-18
*/
void CGraphWindow::OnRefreshTimer(wxTimerEvent& event)
{
	if (m_pRoot)
		m_pRoot->Refresh();
	Refresh();
}



/**
 @brief 
 @date 2013-12-18
*/
void CGraphWindow::OnKeyDown(wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
	case WXK_F5:
		{
			if (m_pRoot)
				m_pRoot->Refresh();
			Refresh();
		}
		break;

	case WXK_TAB:
		{
			int val = (int)m_DispMode;
			++val;
			m_DispMode = (DISP_MODE)(val % DISP_MAX);
			Refresh();
		}
		break;

	case 44: // <
		m_Timer.Stop();
		m_TimerInterval = max(100, m_TimerInterval-100);
		m_Timer.Start( m_TimerInterval );
		Refresh();
		break;

	case 46: // >
		m_Timer.Stop();
		m_TimerInterval = max(100, m_TimerInterval+100);
		m_Timer.Start( m_TimerInterval );
		Refresh();
		break;

	case 72: // H
		m_IsShowHelp = !m_IsShowHelp;
		Refresh();
		break;

	default:
		event.Skip();
		break;
	}
}
