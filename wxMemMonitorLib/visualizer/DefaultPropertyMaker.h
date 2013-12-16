/**
Name:   DefaultPropertyMaker.h
Author:  jjuiddong
Date:    1/30/2013

*/
#pragma once


#include "VisualizerGlobal.h"

namespace memmonitor 
{
	class CPropertyWindow; 
	class CPropertyItemAdapter;
	class CGraphWindow;
}


class wxPGProperty;
namespace visualizer
{
	using namespace memmonitor;


	struct SVisExpr
	{
		wxPGProperty *prop;
		CGraphWindow *graph;

		SVisExpr() : prop(NULL), graph(NULL) { }
		SVisExpr(wxPGProperty *prop0, CGraphWindow *graph0) : prop(prop0), graph(graph0) { }
	};


	// Make Property
	bool	MakeProperty_DefaultForm( CPropertyWindow *pProperties,  wxPGProperty *pParentProp,
		const std::string &symbolName, const bool IsApplyVisualizer, const int depth );

	bool	MakeProperty_DefaultForm(  CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth );

	bool	MakePropertyChild_DefaultForm(  CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth );


	// Make Graph
	bool	MakeGraph_DefaultForm( CGraphWindow *pGraphWindow,  CGraphWindow *pParentWnd,
		const std::string &symbolName, const bool IsApplyVisualizer, const int depth );

	bool	MakeGraph_DefaultForm(  CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth );

}
