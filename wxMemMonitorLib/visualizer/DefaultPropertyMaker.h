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
}

class wxPGProperty;
namespace visualizer
{
	using namespace memmonitor;

	bool		MakeProperty_DefaultForm( CPropertyWindow *pProperties,  wxPGProperty *pParentProp,
		const std::string &symbolName, const bool IsApplyVisualizer, const int depth );

	bool		MakeProperty_DefaultForm(  CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth );

	bool		MakePropertyChild_DefaultForm(  CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth );

}
