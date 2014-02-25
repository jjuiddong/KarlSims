
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "Global.h"
#include "../ui/Frame.h"
#include "../ui/MemoryTree.h"
#include "../ui/LogWindow.h"
#include "../dia/DiaWrapper.h"
#include "../visualizer/PropertyMaker.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "../memory/SharedMemoryMng.h"


namespace memmonitor
{
	//EXECUTE_TYPE g_Type = OUTER_PROCESS;
	EXECUTE_TYPE g_Type = INNER_PROCESS;
	std::string g_ErrorMsg;
	std::string g_ConfigFileName;
	HINSTANCE g_hInstance;

	// memory map
	MapType *g_pMemMap = NULL;

	// clear
	bool g_IsClear = false;
}

using namespace memmonitor;


EXECUTE_TYPE memmonitor::GetExecuteType() 
{ 
	return g_Type; 
}
void memmonitor::SetExecuteType(EXECUTE_TYPE type) 
{ 
	g_Type = type; 
}
const std::string& memmonitor::GetErrorMsg()
{
	return g_ErrorMsg;
}
void	memmonitor::SetErrorMsg(const std::string&msg)
{
	g_ErrorMsg = msg;
}
const std::string& memmonitor::GetConfigFileName()
{
	return g_ConfigFileName;
}
void memmonitor::SetConfigFileName(const std::string &fileName)
{
	g_ConfigFileName = fileName;
}
void	memmonitor::SethInstance(HINSTANCE hInstance)
{
	g_hInstance = hInstance;	
}
HINSTANCE	memmonitor::GethInstance()
{
	return g_hInstance;
}

MemTreePtr memmonitor::GetMemoryTree()
{
	CFrame *pFrame = static_cast<CFrame*>(wxTheApp->GetTopWindow());
	RETV(!pFrame, NULL);
	return pFrame->GetMemTree();
}
LogWindowPtr	memmonitor::GetLogWindow()
{
	CFrame *pFrame = static_cast<CFrame*>(wxTheApp->GetTopWindow());
	RETV(!pFrame, NULL);
	return pFrame->GetLogWindow();
}
PropWindowPtr memmonitor::GetPropertyWindow()
{
	CFrame *pFrame = static_cast<CFrame*>(wxTheApp->GetTopWindow());
	RETV(!pFrame, NULL);
	return pFrame->GetPropWindow();
}
FramePtr memmonitor::GetFrame()
{
	CFrame *pFrame = static_cast<CFrame*>(wxTheApp->GetTopWindow());
	return pFrame;
}


//------------------------------------------------------------------------
// wxVariant1 <= variant0; variant2 <= wxVariant1
// variant0 != variant2   
// variant 에서 wxVariant로 변환되면서 타입이 바뀔 수 있다.
//------------------------------------------------------------------------
wxVariant memmonitor::Variant2wxVariant(const _variant_t &var)
{
	wxVariant wxVar;
	switch (var.vt)
	{
	case VT_I1: wxVar = (long)var.cVal; break;
	case VT_I2: wxVar = wxVariant(var.iVal); break;
	case VT_I4: wxVar = wxVariant(var.lVal); break;
	case VT_R4: wxVar = wxVariant(var.fltVal); break;
	case VT_R8: wxVar = wxVariant(var.dblVal); break;

	case VT_BOOL: wxVar = wxVariant(var.bVal); break;
	case VT_DECIMAL: break;

	case VT_UI1: wxVar = wxVariant(var.bVal); break;
	case VT_UI2: wxVar = wxVariant(var.uiVal); break;
	case VT_UI4: wxVar = wxVariant((int)var.ulVal); break;
	case VT_I8: wxVar = wxVariant(wxLongLong(var.llVal)); break;
	case VT_UI8: wxVar = wxVariant(wxULongLong(var.ullVal)); break;

	case VT_INT: wxVar = wxVariant(var.intVal); break;
	case VT_UINT: wxVar = wxVariant((int)var.uintVal); break;

	case VT_BSTR:
		{
			//			std::string str;
			//			packet  >>(str);
			//#ifdef _UNICODE
			//			varType.bstrVal = (_bstr_t)str2wstr(str).c_str();
			//#else
			//			varType.bstrVal = (_bstr_t)str.c_str();
			//#endif
		}
		break;
	default:
		break;
	}
	return wxVar;
}


//------------------------------------------------------------------------
// variant <= wxVariant
//------------------------------------------------------------------------
_variant_t memmonitor::wxVariant2Variant(const wxVariant &wxVar)
{
	_variant_t var;
	const wxString type = wxVar.GetType();
	if (type == "bool")
	{
		var = _variant_t(wxVar.GetBool());
	}
	else if (type == "char")
	{
		var = _variant_t((char)wxVar);
	}
	else if (type == "datetime")
	{
	}
	else if (type == "double")
	{
		var = _variant_t(wxVar.GetDouble());
	}
	else if (type == "list")
	{
	}
	else if (type == "long")
	{
		var = _variant_t(wxVar.GetLong());
	}
	else if (type == "longlong")
	{
	}
	else if (type == "string")
	{
	}
	else if (type == "ulonglong")
	{
	}
	else if (type == "arrstring")
	{
	}
	else if (type == "void*")
	{
	}
	return var;
}


//------------------------------------------------------------------------
// convert wxVariant to _variant_t with vartype format 
//------------------------------------------------------------------------
_variant_t memmonitor::wxVariant2Variant(const VARTYPE &vartype, const wxVariant &wxVar)
{
	_variant_t tmp = wxVariant2Variant(wxVar);

	_variant_t var;	
	switch (vartype)
	{
	case VT_I1: var = (char)tmp; break;
	case VT_I2: var = (SHORT)tmp; break;
	case VT_I4: var = (long)tmp; break;
	case VT_I8: var = (LONGLONG)tmp; break;
	case VT_UI1: var = (BYTE)tmp; break;
	case VT_UI2: var = (u_short)tmp; break;
	case VT_UI4: var = (DWORD)tmp; break;
	case VT_UI8: var = (ULONGLONG)tmp; break;	
	case VT_R4: var = (float)tmp; break;
	case VT_R8: var = (double)tmp; break;
	case VT_INT: var = (int)tmp; break;
	case VT_UINT: var = (unsigned int)tmp; break;

	case VT_BOOL: var = (bool)tmp; break;
	case VT_DECIMAL: break;

	case VT_BSTR:
		break;
	default:
		break;
	}
	return var;
}



//------------------------------------------------------------------------
// open configfile, json file format 
//------------------------------------------------------------------------
std::pair<bool,string>	memmonitor::InitMemoryMonitor(const std::string &configFileName)
{
	typedef std::pair<bool,string> RetType;
	string pdbPath;

	try
	{
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(configFileName.c_str(), props);
		pdbPath = props.get<string>("pdbpath");

		// Pdb Load
		if (!dia::Init(pdbPath))
		{
			SetErrorMsg(format("%s Pdb 파일이 없습니다.\n", pdbPath.c_str()) );
			return RetType(false,"");
		}

		if (OUTER_PROCESS == GetExecuteType())
		{
			string shareMemoryName = props.get<string>("sharedmemoryname");
			if (!sharedmemory::Init(shareMemoryName, sharedmemory::SHARED_CLIENT))
			{
				SetErrorMsg(
					format("%s  이름의 공유메모리가 없습니다.\n", 
					shareMemoryName.c_str()) );
				return RetType(false,"");
			}
		}

	}
	catch (std::exception &e)
	{
		SetErrorMsg( format("\"%s\" json script Err!! [%s]\n",  configFileName.c_str(), e.what()) );
		return RetType(false,"");
	}

	return RetType(true, pdbPath);
}


//------------------------------------------------------------------------
// read configfile, read attribute "property" : [ ] 
// then create property window
//------------------------------------------------------------------------
bool memmonitor::ReadConfigFile(const std::string &fileName)
{
	if (fileName.empty())
	{
		GetLogWindow()->PrintText( "not found config file\n" );
		return false;
	}

	try
	{
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(fileName.c_str(), props);
		ptree &childs = props.get_child("property");
		BOOST_FOREACH(ptree::value_type &vt, childs)
		{
			const string name = vt.second.get<string>("symbolname");
			GetFrame()->AddPropertyWindow( name );
		}
	}
	catch (std::exception &e)
	{
		GetLogWindow()->PrintText( 
			wxString("(") + fileName.c_str() + ") " + e.what() );
	}
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
wxRect ParseRect(const std::string rectStr)
{
	wxRect r(0,0,0,0);
	sscanf( rectStr.c_str(), "%d %d %d %d", &r.x, &r.y, &r.width, &r.height);
	return r;
}


//------------------------------------------------------------------------
// re calculate window position
// main frame window, memory tree window, property window
//------------------------------------------------------------------------
bool memmonitor::RepositioningWindow()
{
	try
	{
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		std::string filename = g_ConfigFileName + ".windowsize.json";
		boost::property_tree::read_json( filename, props );

		string mainW  = props.get<string>("main window");
		wxRect mainR = ParseRect(mainW);
		GetFrame()->SetSize(mainR);

		string memW  = props.get<string>("memory tree", "");
		if (!memW.empty())
		{
			wxRect memR = ParseRect(memW);
			GetFrame()->UpdatePaneSize(GetMemoryTree(), memR.GetWidth(), memR.GetHeight());
		}

		const CFrame::PropertyFrames &frames = GetFrame()->GetPropFrames();
		BOOST_FOREACH(auto &frame, frames)
		{
			const string name = (const char*)frame->GetTitle();
			const string rectStr = props.get<string>(name, "" );
			if (rectStr.empty()) continue;

			wxRect r = ParseRect(rectStr);
			frame->SetSize(r);
		}
	}
	catch (std::exception &e)
	{
		GetLogWindow()->PrintText( wxString(e.what()) + "\n" );
		GetLogWindow()->PrintText( "\n" );
	}

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void memmonitor::WriteWindowPosition()
{
	try
	{
		using boost::property_tree::ptree;
		using std::string;
		ptree props;

		wxRect mainW = GetFrame()->GetRect();
		props.add( "main window", 
			format("%d %d %d %d", mainW.x, mainW.y, mainW.width, mainW.height) );
		if (GetMemoryTree())
		{
			wxRect memW = GetMemoryTree()->GetRect();
			props.add( "memory tree", 
				format("%d %d %d %d", memW.x, memW.y, memW.width, memW.height) );
		}

		const CFrame::PropertyFrames &frames = GetFrame()->GetPropFrames();
		BOOST_FOREACH(auto &frame, frames)
		{
			string name = (const char*)frame->GetTitle();
			wxRect r = frame->GetScreenRect();
			props.add( name.c_str(), format("%d %d %d %d", r.x, r.y, r.width, r.height) );
		}

		std::string filename = g_ConfigFileName + ".windowsize.json";
		boost::property_tree::write_json( filename, props );
	}
	catch (std::exception &e)
	{
		GetLogWindow()->PrintText( e.what() );
	}
}


/**
 @brief Memory Map Clear
 */
void	memmonitor::Clear()
{
	if (g_pMemMap)
	{
		BOOST_FOREACH(auto &it, *g_pMemMap)
		{
			delete it.second.ptr;
		}
		SAFE_DELETE(g_pMemMap);
	}
	g_IsClear = true;
}

bool	memmonitor::IsClear()
{
	return g_IsClear;
}

MapType& memmonitor::GetMemoryMap()
{
	if (!g_pMemMap)
		g_pMemMap = new MapType;
	return *g_pMemMap;
}
