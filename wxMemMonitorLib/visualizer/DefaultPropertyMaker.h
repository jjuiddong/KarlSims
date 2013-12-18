/**
Name:   DefaultPropertyMaker.h
Author:  jjuiddong
Date:    1/30/2013

*/
#pragma once


#include "VisualizerGlobal.h"


class wxPGProperty;
namespace visualizer
{
	using namespace memmonitor;


	// Make Property
	bool	MakeProperty_DefaultForm( SVisDispDesc parentDispdesc, 
		const std::string &symbolName, const bool IsApplyVisualizer, const int depth );

	bool	MakeProperty_DefaultForm(  SVisDispDesc parentDispdesc, 
		const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth );

	bool	MakePropertyChild_DefaultForm(  SVisDispDesc parentDispdesc, 
		const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth );
}
