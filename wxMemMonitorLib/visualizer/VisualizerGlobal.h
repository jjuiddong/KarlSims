//------------------------------------------------------------------------
// Name:    VisualizerGlobal.h
// Author:  jjuiddong
// Date:    2/4/2013
// 
// visualizer 에서 사용하는 전역 정의들 모음
//------------------------------------------------------------------------
#pragma once
#include "../wxMemMonitor.h"
#include "VisualizerDefine.h"



struct IDiaSymbol;
class wxPGProperty;
namespace visualizer
{
	using namespace memmonitor;
	class CGraphWindow;
	class CStructureCircle;
	class CPropertyWindow; 
	class CPropertyItemAdapter;


	enum GRAPH_ALIGN_TYPE {
		GRAPH_ALIGN_VERT,	// grow vertical
		GRAPH_ALIGN_HORZ, // grow horizontal
	};



	// Symbol Information
	struct SSymbolInfo
	{
		IDiaSymbol *pSym;
		SMemInfo mem;	
		bool isNotRelease; // defalut : false

		SSymbolInfo() 
		{ 
			pSym = NULL; 
			isNotRelease = false;
		}
		SSymbolInfo( const SSymbolInfo &rhs );
		SSymbolInfo( IDiaSymbol *psym, const SMemInfo &_mem) 
			: pSym(psym), mem(_mem), isNotRelease(false)
		{ 
		}
		SSymbolInfo( IDiaSymbol *psym, const SMemInfo &_mem, bool _isRelease) 
			: pSym(psym), mem(_mem), isNotRelease(!_isRelease)
		{
		}
		~SSymbolInfo(); // 소멸자에서 pSym 변수를 제거한다. isNotRelease가 false일때만

		SSymbolInfo& operator= (const SSymbolInfo &rhs);
		
	};



	// Visualizer Display Description
	struct SVisDispDesc
	{
		wxPGProperty *prop;
		CPropertyWindow *propWindow;
		CGraphWindow *graph;
		CStructureCircle *circle;
		GRAPH_ALIGN_TYPE alignGraph;

		SVisDispDesc() : prop(NULL), graph(NULL), propWindow(NULL), circle(NULL), alignGraph(GRAPH_ALIGN_VERT) { }

		explicit SVisDispDesc(CPropertyWindow *pwnd, wxPGProperty *prop0=NULL, 
			CGraphWindow *graph0=NULL, CStructureCircle *circle0=NULL, GRAPH_ALIGN_TYPE kind0=GRAPH_ALIGN_VERT) : 
			propWindow(pwnd), prop(prop0), graph(graph0), circle(circle0), alignGraph(kind0) { }

		SVisDispDesc(const SVisDispDesc &rhs) {
			operator=(rhs);
		}

		const SVisDispDesc& operator=(const SVisDispDesc &rhs) {
			if (this != &rhs) {
				prop = rhs.prop;
				propWindow = rhs.propWindow;
				graph = rhs.graph;
				circle = rhs.circle;
				alignGraph = rhs.alignGraph;
			}
			return *this;
		}

	};

}
