/**
Name:    PropertyMaker.h
Author:  jjuiddong
Date:    1/29/2013

 make property attribute with visualizer autoexp.txt script
*/
#ifndef __VISUALIZERPROPERTYMAKER_H__
#define __VISUALIZERPROPERTYMAKER_H__

#include "VisualizerDefine.h"
#include "VisualizerGlobal.h"



struct IDiaSymbol;
class wxPGProperty;
namespace visualizer
{
	using namespace memmonitor;


	 // init
	bool OpenVisualizerScript( const std::string &fileName );
	void Release();

	// make property
	bool	MakeVisualizerProperty( SVisDispDesc visDispdesc, 
		const SMemInfo &memInfo, const std::string &symbolName, const int depth );

	bool	MakeVisualizerProperty( SVisDispDesc visDispdesc, const SSymbolInfo &symbol, const int depth );

	// find
	bool Find_ChildSymbol(  const std::string findSymbolName, 
		IN const SSymbolInfo &symbol, OUT SSymbolInfo *pOut );

	/// 이중 포인터 값인 ptr을 실제 가르키는 주소 값을 리턴한다. return  *(DWORD*)(void*)ptr
	DWORD Point2PointValue(DWORD ptr);

}

#endif //  __VISUALIZERPROPERTYMAKER_H__
