
#include "stdafx.h"
#include "VisualizerGlobal.h"
#include "../Dia/DiaWrapper.h"

using namespace visualizer;

SSymbolInfo::SSymbolInfo( const SSymbolInfo &rhs )
{
	operator=(rhs);
}

visualizer::SSymbolInfo::~SSymbolInfo() 
{ 
	if (pSym && !isNotRelease)
		pSym->Release(); 
}

SSymbolInfo& SSymbolInfo::operator= (const SSymbolInfo &rhs)
{
	if (this != &rhs)
	{
		pSym = rhs.pSym;
		mem = rhs.mem;
		isNotRelease = rhs.isNotRelease;
	}
	return *this;
}
