
#include "stdafx.h"
#include "PropertyWindow.h"
#include "PropertyItemAdapter.h"
#include "LogWindow.h"
#include "../dia/DiaWrapper.h"
#include "../visualizer/DefaultPropertyMaker.h"
#include "../visualizer/PropertyMaker.h"
#include "Frame.h"


using namespace memmonitor;
using namespace dia;
using namespace visualizer;


BEGIN_EVENT_TABLE( CPropertyWindow, wxPropertyGridManager )
	EVT_SIZE(CPropertyWindow::OnSize)
	EVT_CONTEXT_MENU(CPropertyWindow::OnContextMenu)
	EVT_MENU(MENU_OPEN_PROPERTY, CPropertyWindow::OnMenuOpenProperty)
	EVT_MENU(MENU_OPEN_GRAPH, CPropertyWindow::OnMenuOpenGraph)
	EVT_TIMER(ID_REFRESH_TIMER, CPropertyWindow::OnRefreshTimer)
END_EVENT_TABLE()


CPropertyWindow::CPropertyWindow(wxWindow *parent)  : 
	wxPropertyGridManager(parent, wxID_ANY, wxDefaultPosition, wxSize(500,300), 
		wxPG_BOLD_MODIFIED |
		wxPG_AUTO_SORT |
		wxPG_SPLITTER_AUTO_CENTER |
		//wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
		//wxPG_TOOLTIPS |
		//wxPG_HIDE_CATEGORIES |
		//wxPG_LIMITED_EDITING |
		wxPG_TOOLBAR 
		//wxPG_DESCRIPTION 
		)
{
	const int extraStyle = wxPG_EX_MODE_BUTTONS |
		wxPG_EX_MULTIPLE_SELECTION;
	SetExtraStyle(extraStyle);
	SetColumnCount(3);
	GetGrid()->SetEmptySpaceColour(wxColour(237,237,237));

	m_Timer.SetOwner(this, ID_REFRESH_TIMER);
	m_Timer.Start( REFRESH_INTERVAL );

	Connect(GetId(), wxEVT_PG_CHANGED,
		wxPropertyGridEventHandler(CPropertyWindow::OnPropertyGridChange) );
	Connect(GetId(), wxEVT_PG_SELECTED,
		wxPropertyGridEventHandler(CPropertyWindow::OnPropertyGridSelect) );	
	Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(CPropertyWindow::OnKeyDown)); 
}

CPropertyWindow::~CPropertyWindow() 
{
	m_Timer.Stop();
	ClearPropItem();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::UpdateSymbol( const wxString &symbolName )
{
	if (m_CurrentSymbolName == symbolName)
		return;
	if ((symbolName == "@Root") || (symbolName == "--- Memory List ---"))
		return;

	GetLogWindow()->PrintText( "UpdateSymbol = " +  symbolName  + "\n" );

	wxPropertyGrid *pPropGrid = GetGrid();
	pPropGrid->Clear();
	ClearPropItem();

	pPropGrid->SetColumnCount(3);

	std::string tmpStr = symbolName;
	std::string str = ParseObjectName(tmpStr);
	const bool result = visualizer::MakeProperty_DefaultForm(visualizer::SVisDispDesc(this,NULL), tmpStr, true, 2);

	// root node expand
	wxPGVIterator it;
	for (it = pPropGrid->GetVIterator( wxPG_ITERATE_FIXED_CHILDREN ); !it.AtEnd(); it.Next())
	{
		it.GetProperty()->SetExpanded( true );
		break;
	}

	CheckSymbol( symbolName );

	//SetWindowTextW(symbolName);
	m_CurrentSymbolName = symbolName;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::CheckSymbol( const wxString &symbolName )
{
	std::string tmpStr = symbolName;
	std::string str = ParseObjectName(tmpStr);

	CComPtr<IDiaSymbol> pSymbol = dia::FindType(str);
	if (!pSymbol)
	{
		GetLogWindow()->PrintText( 
			"\"" + tmpStr + "\"" +
			" 해당하는 심볼은 Pdb파일에 없습니다.\n" );
		return;
	}

	SMemInfo memInfo;
	if (!FindMemoryInfo(tmpStr, memInfo))
	{
		GetLogWindow()->PrintText( 
			"공유메모리에 " + tmpStr + " 타입의 정보가 없습니다.\n" );
		return;
	}
}


/**
 @brief 
 */
wxPGProperty* CPropertyWindow::AddProperty( wxPGProperty *pParentProp, 
	CPropertyItemAdapter &propAdapter, 
	const visualizer::SSymbolInfo *pSymbol, STypeData *pTypeData )
{
	wxPGProperty*prop = propAdapter.GetProperty();
	RETV(!prop, NULL);

	if (pParentProp) // already exist? then return
	{		
		wxPGProperty *pChildProp = pParentProp->GetPropertyByName( prop->GetName() );
		if (pChildProp)
		{
			delete prop;
			propAdapter.SetProperty(pChildProp);
			return propAdapter.GetProperty();
		}
	}
	else
	{
		int a = 0;
		//wxPGProperty *pRoot = GetGrid()->GetRoot();
		//if (pRoot && pRoot->GetName() == prop->GetName())
		//{
		//	delete prop;
		//	propAdapter.SetProperty(pRoot);
		//	return propAdapter.GetProperty();
		//}
	}

	// setting Property Item Information
	std::string typeName;
	SPropItem *pPropItem = new SPropItem;
	pPropItem->typeData = *pTypeData;
	if (pSymbol)
	{
		pPropItem->typeName = dia::GetSymbolName(pSymbol->pSym);
		pPropItem->symbolTypeName = dia::GetSymbolTypeName(pSymbol->pSym, false);
		pPropItem->typeData.ptr = pSymbol->mem.ptr;

		typeName = dia::GetSymbolTypeName(pSymbol->pSym);
		if (typeName == "NoType") typeName = "";
	}
	m_PropList.push_back(pPropItem);
	prop->SetClientData(pPropItem);

	// Add Property
	if (pParentProp)
	{
		AppendIn( pParentProp, prop );

		// set background color
		wxColour bgColor = pParentProp->GetCell(2).GetBgCol();
		wxColour childColor = wxColour(bgColor.Red()-10, bgColor.Green()-10, bgColor.Blue()-10);
		if (childColor.Red() < 100)
			childColor = wxColour(100,100,100);
		prop->SetBackgroundColour(childColor);

		// insert type column cell
		wxPGCell cell( typeName );
		cell.SetBgCol(childColor);
		prop->SetCell(2,  cell);
		//
	}
	else
	{
		wxPGProperty *pg = Append(prop);
	}

	return prop;
}


/**
 @brief Resize Event Handler
 */
void CPropertyWindow::OnSize(wxSizeEvent& event)
{
	if (GetParent() != (void*)GetFrame())
	{
		const wxRect r = GetParent()->GetSize();
		SetSize(r);
		RecalculatePositions(r.width-20, r.height-20);
	}
	else
	{
		event.Skip();
	}
}


//------------------------------------------------------------------------
// Property Change Event Handler
//------------------------------------------------------------------------
void CPropertyWindow::OnPropertyGridChange( wxPropertyGridEvent& event )
{
	wxPGProperty* pProp = event.GetProperty();
	SPropItem *pItemData = (SPropItem*)pProp->GetClientData();
	if (pItemData && pItemData->typeData.ptr)
	{
		const wxVariant curVar = pProp->GetValue();
		_variant_t var = wxVariant2Variant(pItemData->typeData.vt, curVar);
		// write memory
		dia::SetValue( pItemData->typeData.ptr, var);
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::OnPropertyGridSelect( wxPropertyGridEvent& event )
{
	SetFocus();

	wxPGProperty* pProp = event.GetProperty();
	RET(!pProp);
	wxString name = pProp->GetName();

	SPropItem *pItemData = (SPropItem*)pProp->GetClientData();
	if (pItemData && pItemData->typeData.vt == VT_EMPTY &&
		pProp->GetChildCount() <= 0)
	{
		IDiaSymbol *pSym = dia::FindType(pItemData->symbolTypeName);
		if (!pSym)
			return;

		DWORD ptr = (DWORD)pItemData->typeData.ptr;
		if (SymTagPointerType == pItemData->typeData.symtag)
			ptr = visualizer::Point2PointValue((DWORD)pItemData->typeData.ptr);

		SSymbolInfo symbol( pSym, memmonitor::SMemInfo("*", (void*)ptr, 0) );
		visualizer::MakePropertyChild_DefaultForm( visualizer::SVisDispDesc(this,pProp), symbol, true, 2 );

			
		//if (!FindSymbolUpward( pProp, &symbol ))
		//	return;
		//if (visualizer::MakePropertyChild_DefaultForm( this, pProp, symbol))
		//{
			//pProp->Expand();
			//AdjustLayout();
		//}
	}
}


//------------------------------------------------------------------------
// find symbol from child to parent node
//------------------------------------------------------------------------
bool	CPropertyWindow::FindSymbolUpward( wxPGProperty *pProp, OUT SSymbolInfo *pOut )
{
	using std::string;

	RETV(!pProp, false);
	RETV(!pOut, false);

	SPropItem *pItemData = (SPropItem*)pProp->GetClientData();
	RETV(!pItemData, false);

	string name =  pItemData->typeName;
	const int idx = name.find(' ');
	string searchName;
	if (string::npos == idx)
		searchName = name;
	else
		searchName = name.substr(0, idx);

	wxPGProperty *pParentProp = pProp->GetParent();
	if (pParentProp)
	{
		bool retry = false;
		SSymbolInfo symbol;
		if (!FindSymbolUpward( pParentProp, &symbol ))
		{
			retry = true;
		}
		else
		{
			if (!visualizer::Find_ChildSymbol(searchName, symbol, pOut))
				retry = true;
		}

		if (retry)
		{
			// 찾기를 실패했다면, 현재 노드에서 찾기를 시도한다.
			const string typeName = ParseObjectName(searchName);
			pOut->pSym = dia::FindType( typeName );
			RETV(!pOut->pSym, false);
			pOut->mem = SMemInfo(pItemData->typeName.c_str(), pItemData->typeData.ptr, 0);
		}
	}
	else
	{
		const string typeName = ParseObjectName(searchName);
		pOut->pSym = dia::FindType( typeName );
		RETV(!pOut->pSym, false);
		pOut->mem = SMemInfo(pItemData->typeName.c_str(), pItemData->typeData.ptr, 0); 
	}

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void	CPropertyWindow::RefreshPropertyItem( wxPGProperty *pProp )
{
	// if property edit  item then return
	if (GetGrid()->IsEditorFocused() && GetSelectedProperty() == pProp)
		return;

	SPropItem *pItem = (SPropItem*)pProp->GetClientData();
	if (pItem && 
		(SymTagData == pItem->typeData.symtag 
		|| SymTagBaseType == pItem->typeData.symtag 
		||  SymTagEnum == pItem->typeData.symtag 
		)
		&& pItem->typeData.ptr
		&& pItem->typeData.vt != VT_EMPTY )
	{
		_variant_t value = dia::GetValue(pItem->typeData.ptr, pItem->typeData.vt);
		CPropertyItemAdapter adaptor(pProp);
		adaptor.SetVariant( value );
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::OnContextMenu(wxContextMenuEvent& event)
{
	wxPoint point = event.GetPosition();
	point = ScreenToClient(point);
	wxMenu menu;
	menu.Append(MENU_OPEN_PROPERTY, wxT("&Open Property"));
	menu.Append(MENU_OPEN_GRAPH, wxT("&Open Graph"));
	PopupMenu(&menu, point);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::OnMenuOpenProperty(wxCommandEvent& event)
{

}


/**
 @brief 
 @date 2013-12-16
*/
void CPropertyWindow::OnMenuOpenGraph(wxCommandEvent& event)
{
	wxPGProperty* pProp = GetSelectedProperty();
	RET(!pProp);
	const string name = pProp->GetName();

	SPropItem *pItemData = (SPropItem*)pProp->GetClientData();
	if (!pItemData)
		return;

	IDiaSymbol *pSym = dia::FindType(pItemData->symbolTypeName);
	if (!pSym)
		return;

	DWORD ptr = (DWORD)pItemData->typeData.ptr;
	if (SymTagPointerType == pItemData->typeData.symtag)
		ptr = visualizer::Point2PointValue((DWORD)pItemData->typeData.ptr);

	SSymbolInfo symbol( pSym, memmonitor::SMemInfo("*", (void*)ptr, 0) );
	GetFrame()->AddGraphWindow( pItemData->symbolTypeName, name, symbol );
}


//------------------------------------------------------------------------
// Property Refresh Timer
//------------------------------------------------------------------------
void CPropertyWindow::OnRefreshTimer(wxTimerEvent& event)
{
	wxPropertyGrid *pPropGrid = GetGrid();
	wxPGVIterator it;
	for ( it = pPropGrid->GetVIterator( wxPG_ITERATE_FIXED_CHILDREN ); !it.AtEnd(); it.Next() )
		RefreshPropertyItem( it.GetProperty() );
}


//------------------------------------------------------------------------
// remove m_PropList
//------------------------------------------------------------------------
void CPropertyWindow::ClearPropItem()
{
	BOOST_FOREACH(auto &item, m_PropList)
	{
		SAFE_DELETE(item);
	}
	m_PropList.clear();
}


/**
 @brief KeyDown Event Handler
 */
void CPropertyWindow::OnKeyDown(wxKeyEvent& event)
{
	event.Skip();
	if (344 == event.GetKeyCode()) // F5
	{
		wxPropertyGrid *pPropGrid = GetGrid();
		wxPGProperty *pRoot = pPropGrid->GetRoot();
		if (pRoot)
		{
			const std::string symbolName = m_CurrentSymbolName;
			const bool result = visualizer::MakeProperty_DefaultForm(visualizer::SVisDispDesc(this,pRoot), symbolName, true, 2);
			Refresh();
		}

		wxPGVIterator it;
		for ( it = pPropGrid->GetVIterator( wxPG_ITERATE_FIXED_CHILDREN ); !it.AtEnd(); it.Next() )
		{
			it.GetProperty()->SetExpanded( true );
			break;
		}
	}
}


/**
@brief  자식 프로퍼티 제거
*/
void	CPropertyWindow::RemoveChildProperty( wxPGProperty *pParentProp )
{
	RET(!pParentProp);

	RemovePropClientData(pParentProp);
	pParentProp->DeleteChildren();
	Refresh();
}


/**
@brief  RemovePropClientData
*/
void CPropertyWindow::RemovePropClientData( wxPGProperty *pParentProp )
{
	RET(!pParentProp);

	const int size = pParentProp->GetChildCount();
	for (int i=0; i < size; ++i)
	{
		wxPGProperty *pProp = pParentProp->Item(i);
		RemovePropClientData( pProp );
		SPropItem *pitem = (SPropItem*)pProp->GetClientData();
		m_PropList.remove(pitem);
		SAFE_DELETE(pitem);
		pProp->SetClientData(NULL);
	}
}
