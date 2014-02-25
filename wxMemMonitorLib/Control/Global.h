//------------------------------------------------------------------------
// Name:    Global.h
// Author:  jjuiddong
// Date:    3/11/2013
// 
// memmonitor global function
//------------------------------------------------------------------------
#pragma once

namespace memmonitor
{
	struct STypeData
	{
		enum SymTagEnum symtag;
		VARTYPE vt;
		void *ptr;
		STypeData( enum SymTagEnum _symtag,  VARTYPE _vt, void *_ptr) : 
		symtag(_symtag), vt(_vt), ptr(_ptr) { }
		STypeData() {}
	};

	typedef  std::map<std::string, SMemInfo> MapType;


	std::pair<bool,string> InitMemoryMonitor(const std::string &configFileName);
	bool							ReadConfigFile(const std::string &fileName);
	bool							RepositioningWindow();
	void							WriteWindowPosition();

	EXECUTE_TYPE		GetExecuteType();
	void							SetExecuteType(EXECUTE_TYPE type);

	MemTreePtr				GetMemoryTree();
	LogWindowPtr			GetLogWindow();
	PropWindowPtr		GetPropertyWindow();
	FramePtr					GetFrame();

	void							SethInstance(HINSTANCE hInstance);
	HINSTANCE				GethInstance();
	const std::string&	GetConfigFileName();
	void							SetConfigFileName(const std::string &fileName);
	const std::string&	GetErrorMsg();
	void							SetErrorMsg(const std::string&msg);

	wxVariant					Variant2wxVariant(const _variant_t &var);
	_variant_t				wxVariant2Variant(const wxVariant &wxVar);
	_variant_t				wxVariant2Variant(const VARTYPE &vartype, const wxVariant &wxVar);


	MapType&				GetMemoryMap();
	void							Clear();
	bool							IsClear();
}
