
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


CGraphWindow::CGraphWindow(wxWindow *parent)
{

}

CGraphWindow::~CGraphWindow()
{

}


/**
 @brief 
 @date 2013-12-16
*/
void CGraphWindow::UpdateSymbol( const wxString &symbolName )
{
	if (m_CurrentSymbolName == symbolName)
		return;
	if ((symbolName == "@Root") || (symbolName == "--- Memory List ---"))
		return;

	GetLogWindow()->PrintText( "GVis UpdateSymbol = " +  symbolName  + "\n" );

	std::string tmpStr = symbolName;
	std::string str = ParseObjectName(tmpStr);
	//const bool result = visualizer::MakeProperty_DefaultForm(this, NULL, tmpStr, true, 2);


}
