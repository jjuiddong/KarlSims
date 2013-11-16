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

namespace memmonitor { 
	class CPropertyWindow; 
	class CPropertyItemAdapter;
}

struct IDiaSymbol;
class wxPGProperty;
namespace visualizer
{
	using namespace memmonitor;

	 // init
	bool OpenVisualizerScript( const std::string &fileName );
	void Release();

	// make property
	bool	MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
		wxPGProperty *pParentProp, 
		const SMemInfo &memInfo, const std::string &symbolName, const int depth );

	bool	MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
		wxPGProperty *pParentProp, const SSymbolInfo &symbol, const int depth );

	// find
	bool Find_ChildSymbol(  const std::string findSymbolName, 
		IN const SSymbolInfo &symbol, OUT SSymbolInfo *pOut );

	/// ���� ������ ���� ptr�� ���� ����Ű�� �ּ� ���� �����Ѵ�. return  *(DWORD*)(void*)ptr
	DWORD Point2PointValue(DWORD ptr);

}

#endif //  __VISUALIZERPROPERTYMAKER_H__
