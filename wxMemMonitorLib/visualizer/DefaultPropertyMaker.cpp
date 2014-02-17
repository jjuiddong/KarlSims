
#include "stdafx.h"
#include "DefaultPropertyMaker.h"
#include "PropertyMaker.h"
#include "../dia/DiaWrapper.h"
#include "../ui/PropertyWindow.h"
#include "../ui/GraphWindow.h"
#include "../ui/PropertyItemAdapter.h"
#include "VisualizerDefine.h"
#include <boost/interprocess/streams/bufferstream.hpp>


namespace visualizer
{
	using namespace parser;

	struct SVisOption
	{
		int depth;
		bool isApplyVisualizer;
		bool isDrawGraph;
		SVisOption() {}
		SVisOption(int _depth, bool _isApplyVisualizer) : 
			depth(_depth), isApplyVisualizer(_isApplyVisualizer), isDrawGraph(false) {}
	}; 


	// make property
	void	MakeProperty_Preview( SVisDispDesc parentExpr, const SSymbolInfo &symbol,
		const bool IsUdtExpand, const SVisOption &option );

	bool	MakeProperty_Child( SVisDispDesc parentExpr, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );



	void	MakeProperty_UDTChild(SVisDispDesc parentExpr, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );

	void	MakeProperty_BaseClass(SVisDispDesc parentExpr, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );

	void	MakeProperty_Pointer_Children(SVisDispDesc parentExpr, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );

	void	MakeProperty_Data(SVisDispDesc parentExpr, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );

	void	MakeProperty_Array(SVisDispDesc parentExpr, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );


	SVisDispDesc MakeProperty_BaseType(SVisDispDesc parentExpr, 
		const std::string valueName,  const SSymbolInfo &symbol );

	SVisDispDesc MakeProperty_Pointer_Preview(SVisDispDesc parentExpr, 
		const SSymbolInfo &symbol );

 	SVisDispDesc MakeProperty_ArrayData(SVisDispDesc parentExpr, 
 		const SSymbolInfo &symbol );

	SVisDispDesc MakeProperty_UDTData(SVisDispDesc parentExpr, 
		const SSymbolInfo &symbol, const SVisOption &option );

	SVisDispDesc MakeProperty_BaseClassData(SVisDispDesc parentExpr, 
		const SSymbolInfo &symbol);

	SVisDispDesc AddProperty(SVisDispDesc parentExpr, 
		CPropertyItemAdapter &propAdapter,  const SSymbolInfo *pSymbol, 
		STypeData *pTypeData);

	_variant_t GetValue(IDiaSymbol *pSymbol, void *pMemPtr);

	// variables
	CPropertyWindow *g_pProperty = NULL;
	CGraphWindow *g_pGraphWnd = NULL;
}

using namespace dia;
using namespace std;
using namespace visualizer;
using namespace memmonitor;



/**
@brief  property창에 심볼을 출력한다.
@param symbolName : {symbolName#count} 메모리 정보를 얻어올 수 있는 형태
*/
bool	visualizer::MakeProperty_DefaultForm( SVisDispDesc parentDispdesc, 
	const std::string &symbolName, const bool IsApplyVisualizer, const int depth )
{
	const std::string str = ParseObjectName(symbolName);
	IDiaSymbol *pSymbol = dia::FindType(str);
	RETV(!pSymbol, false);

	SMemInfo memInfo;
	if (!FindMemoryInfo(symbolName, memInfo))
	{
		pSymbol->Release();
		return false;
	}

	memInfo.name = symbolName;
	MakeProperty_DefaultForm(parentDispdesc, SSymbolInfo(pSymbol, memInfo), IsApplyVisualizer, depth);
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm( SVisDispDesc parentDispdesc, 
				const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth)
{
	if (!symbol.mem.ptr)
		return true;

	g_pProperty = parentDispdesc.propWindow;
	g_pGraphWnd = parentDispdesc.graph;
	MakeProperty_Preview(parentDispdesc, symbol, true, SVisOption(depth, IsApplyVisualizer));
	return true;
}


//------------------------------------------------------------------------
//  symbol.pSym 의 자식을 pParentProp에 추가한다.
//------------------------------------------------------------------------
bool	 visualizer::MakePropertyChild_DefaultForm( SVisDispDesc parentDispdesc, 
	const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth )
{
	if (!symbol.mem.ptr)
		return true;

	g_pProperty = parentDispdesc.propWindow;
	g_pGraphWnd = parentDispdesc.graph;

	bool isVisualizerType = false;
	if (IsApplyVisualizer)
		isVisualizerType = visualizer::MakeVisualizerProperty(parentDispdesc, symbol, depth);

	if (!isVisualizerType)
		MakeProperty_Child(parentDispdesc, symbol, true, SVisOption(depth, IsApplyVisualizer));
	return true;
}


//------------------------------------------------------------------------
//  symbol을 pParentProp의 자식으로 추가한다.
//------------------------------------------------------------------------
bool	 visualizer::MakeProperty_Child( SVisDispDesc parentDispdesc, const SSymbolInfo &symbol, 
	const bool IsUdtExpand, const SVisOption &option )
{
	wxPGProperty *pParentProp = parentDispdesc.prop;

	const bool IsChildExpand = (pParentProp && pParentProp->GetChildCount() > 0);
	if (option.depth <= 0 && !IsChildExpand)
		return true;
	enum SymTagEnum symtag;
	if (S_OK != symbol.pSym->get_symTag((DWORD*)&symtag))
		return true;

	switch (symtag)
	{
	case SymTagArrayType:
		MakeProperty_Array(parentDispdesc, symbol, IsUdtExpand, option);
		break;

	case SymTagPointerType:
		MakeProperty_Pointer_Children(parentDispdesc, symbol, IsUdtExpand, option);
		break;

	case SymTagBaseClass:
		MakeProperty_BaseClass(parentDispdesc, symbol, IsUdtExpand, option);
		break;

	case SymTagUDT:
		MakeProperty_UDTChild(parentDispdesc, symbol, IsUdtExpand, option);
		break;

	case SymTagData: 
		{
			IDiaSymbol *pBaseType;
			HRESULT hr = symbol.pSym->get_type(&pBaseType);
			ASSERT_RETV(S_OK == hr, false);
			MakeProperty_Child(parentDispdesc, SSymbolInfo(pBaseType,symbol.mem), 
				IsUdtExpand, option);
		}
		break;

	case SymTagTypedef:
	case SymTagEnum:
	default:
		break;
	}

	return true;
}


//------------------------------------------------------------------------
// Property 생성
//------------------------------------------------------------------------
void visualizer::MakeProperty_Preview(SVisDispDesc parentDispdesc, const SSymbolInfo &symbol, 
	const bool IsUdtExpand, const SVisOption &option )
{
	wxPGProperty *pParentProp = parentDispdesc.prop;
	const bool IsChildExpand = (pParentProp && pParentProp->GetChildCount() > 0);
	if (option.depth <= 0 && !IsChildExpand)
		return;
	enum SymTagEnum symtag;
	if (S_OK != symbol.pSym->get_symTag((DWORD*)&symtag))
		return;

	wxPGProperty *pProp = NULL;
	SVisDispDesc visDispDesc;
	switch (symtag)
	{
	case SymTagBaseType:
		MakeProperty_BaseType(parentDispdesc, symbol.mem.name, symbol);
		break;

	case SymTagData: 
		MakeProperty_Data(parentDispdesc, symbol, IsUdtExpand, option);
		break;

	case SymTagArrayType:
		MakeProperty_Array(parentDispdesc, symbol, IsUdtExpand, option);
		break;

	case SymTagPointerType:
		visDispDesc = MakeProperty_Pointer_Preview(parentDispdesc, symbol);
		if (visDispDesc.prop || visDispDesc.circle)
			MakeProperty_Child(visDispDesc, symbol, IsUdtExpand, option);
		break;

	case SymTagBaseClass:
		visDispDesc = MakeProperty_BaseClassData(parentDispdesc, symbol);
		if (visDispDesc.prop || visDispDesc.circle)
			MakeProperty_Child(visDispDesc, symbol, IsUdtExpand, option);
		break;

	case SymTagUDT:
		visDispDesc = MakeProperty_UDTData(parentDispdesc, symbol, option);
		if (visDispDesc.prop || visDispDesc.circle)
			MakeProperty_Child(visDispDesc, symbol, IsUdtExpand, option);
		break;

	case SymTagTypedef:
		break;

	case SymTagEnum:
		break;

	default:
		break;
	}
}


/**
 @brief 기본 클래스 
 */
void	visualizer ::MakeProperty_BaseClass(SVisDispDesc parentDispdesc,  const SSymbolInfo &symbol, 
											const bool IsUdtExpand, const SVisOption &option )
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	LocationType locType;
	const LONG offset = dia::GetSymbolLocation(symbol.pSym, &locType);
	BYTE *ptr = (BYTE*)symbol.mem.ptr + offset;
	SMemInfo newMemInfo(symbol.mem.name.c_str(), ptr, 0);
	MakeProperty_Child(parentDispdesc, SSymbolInfo(pBaseType, newMemInfo), IsUdtExpand, option);
}


/**
@brief  baseclass type preview
*/
SVisDispDesc visualizer::MakeProperty_BaseClassData( SVisDispDesc parentDispdesc, 
	const SSymbolInfo &symbol)
{
	CPropertyItemAdapter prop( symbol.mem.name );
	SVisDispDesc newPropDesc = AddProperty(parentDispdesc, prop, &symbol, &STypeData(SymTagBaseClass, VT_EMPTY, NULL));
	return newPropDesc;
	//return SVisDispDesc(NULL, prop.GetProperty());
}


//------------------------------------------------------------------------
// User Define Type 
//------------------------------------------------------------------------
void visualizer::MakeProperty_UDTChild(SVisDispDesc parentDispdesc,  const SSymbolInfo &symbol, 
										const bool IsUdtExpand, const SVisOption &option)
{
	wxPGProperty *pParentProp = parentDispdesc.prop;
	RET(!pParentProp);
	
	const bool IsChildExpand = (pParentProp && pParentProp->GetChildCount() > 0);
	if (option.depth > 0 || IsChildExpand)
	{
		CComPtr<IDiaEnumSymbols> pEnumChildren;
		if (SUCCEEDED(symbol.pSym->findChildren(SymTagNull, NULL, nsNone, &pEnumChildren))) 
		{
			IDiaSymbol *pChild;
			ULONG celt = 0;
			while (SUCCEEDED(pEnumChildren->Next(1, &pChild, &celt)) && (celt == 1)) 
			{
				enum SymTagEnum childSymTag;
				HRESULT hr = pChild->get_symTag((DWORD*)&childSymTag);
				switch (childSymTag)
				{
				case SymTagUDT:
				case SymTagData:
				case SymTagEnum:
				case SymTagPointerType:
				case SymTagBaseType:
				case SymTagTypedef:
				case SymTagBaseClass:
					// process types
					break;

				case SymTagVTable:
					break;

				default:
					pChild->Release();
					continue;
				}

				bool childIsUdtExpand = IsUdtExpand;
				int childDepth = option.depth - 1;
				if (IsUdtExpand && (option.depth == 2))
				{
					if (SymTagUDT == childSymTag || SymTagBaseClass == childSymTag)
					{
						childIsUdtExpand = false;
						childDepth = 2;
					}
				}

				LONG offset = dia::GetSymbolLocation(pChild);
				SMemInfo memberMemInfo;
				memberMemInfo.name = dia::GetSymbolName(pChild);
				memberMemInfo.ptr = (BYTE*)symbol.mem.ptr + offset;
				MakeProperty_Preview(parentDispdesc, SSymbolInfo(pChild, memberMemInfo), 
					childIsUdtExpand, SVisOption(childDepth, option.isApplyVisualizer));
			}
		}
	}

	if (pParentProp &&
		(!IsChildExpand || (IsChildExpand && !pParentProp->IsExpanded())))
			pParentProp->SetExpanded(false);
}


//------------------------------------------------------------------------
// Pointer 타입 출력 
//------------------------------------------------------------------------
void	visualizer::MakeProperty_Pointer_Children(SVisDispDesc parentDispdesc, const SSymbolInfo &symbol,
										  const bool IsUdtExpand, const SVisOption &option)
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);

	void *srcPtr = (void*)*(DWORD*)symbol.mem.ptr;
	void *newPtr = MemoryMapping(srcPtr);

	wxPGProperty *pProp = NULL;
	if (SymTagUDT == baseSymTag)
	{
		if (newPtr) 
		{
			const string typeName = dia::GetSymbolTypeName(symbol.pSym);

			SMemInfo ptrMemInfo(symbol.mem.name.c_str(), newPtr, (size_t)0);
			//MakeProperty_Child(parentDispdesc, SSymbolInfo(pBaseType, ptrMemInfo, false), 
			//	IsUdtExpand, option);
			//MakeProperty_UDTData(parentDispdesc, SSymbolInfo(pBaseType, ptrMemInfo, false), option);

			bool isVisualizerType = false;
			// todo: visualizer preview 작업이 끝나면 없애야한다.
			if (option.isApplyVisualizer && !strncmp(typeName.c_str(),  "std::basic_string",17 ))
			{
				isVisualizerType = visualizer::MakeVisualizerProperty( parentDispdesc,  SSymbolInfo(pBaseType, ptrMemInfo, false), option.depth );
			}
			else if(option.isApplyVisualizer)
			{
				isVisualizerType = visualizer::MakeVisualizerProperty( parentDispdesc,  SSymbolInfo(pBaseType, ptrMemInfo, false), option.depth );
			}
		}
	}
	pBaseType->Release();
}


//------------------------------------------------------------------------
// Property창에 Control을 추가한다.
// 변수 이름과 타입, 값을 설정한다.
//------------------------------------------------------------------------
void visualizer::MakeProperty_Data(SVisDispDesc parentDispdesc, const SSymbolInfo &symbol,
									const bool IsUdtExpand, const SVisOption &option)
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);
	ASSERT_RET(hr == S_OK);

	SVisDispDesc visDispDesc;
	switch (baseSymTag)
	{
	case SymTagBaseType:
		{
			string valueTypeName = symbol.mem.name;// + " (" + dia::GetSymbolTypeName(symbol.pSym) + ")";
			MakeProperty_BaseType( parentDispdesc, valueTypeName, symbol );
		}
		break;

	case SymTagEnum:
		{
			std::string typeName = dia::GetSymbolTypeName(pBaseType);
			std::string valueTypeName =  symbol.mem.name;// + " (" +  typeName + ")";

			CPropertyItemAdapter prop( valueTypeName,  CPropertyItemAdapter::PROPTYPE_ENUM ); 
			AddProperty(parentDispdesc, prop, &symbol, &STypeData(baseSymTag, VT_UI4, symbol.mem.ptr));

			CComPtr<IDiaEnumSymbols> pEnumChildren;
			IDiaSymbol *pChild;
			if (SUCCEEDED(pBaseType->findChildren(SymTagNull, NULL, nsNone, &pEnumChildren))) 
			{
				//int enum_v = 0;
				ULONG celt = 0;
				while (SUCCEEDED(pEnumChildren->Next(1, &pChild, &celt)) && (celt == 1)) 
				{
					std::string e_name = dia::GetSymbolName(pChild);
					prop.AddChoice(e_name);
					pChild->Release();
				}
			}

			ULONGLONG length = 0;
			HRESULT hr = pBaseType->get_length(&length);
			_variant_t value = dia::GetValueFromAddress( symbol.mem.ptr, btUInt, length );
			prop.SetValue( value.intVal );
		}
		break;

	case SymTagUDT:
		visDispDesc = MakeProperty_UDTData(parentDispdesc, symbol, option);
		if (visDispDesc.prop || visDispDesc.circle)
			MakeProperty_Child(visDispDesc, SSymbolInfo(pBaseType, symbol.mem), IsUdtExpand, SVisOption(option.depth-1, option.isApplyVisualizer));
		break;

	case SymTagArrayType:
		visDispDesc = MakeProperty_ArrayData(parentDispdesc, symbol);
		if (visDispDesc.prop || visDispDesc.circle)
			MakeProperty_Child(visDispDesc, SSymbolInfo(pBaseType, symbol.mem), IsUdtExpand, SVisOption(option.depth-1, option.isApplyVisualizer));
		break;

	case SymTagPointerType:
		visDispDesc = MakeProperty_Pointer_Preview(parentDispdesc, SSymbolInfo(pBaseType, symbol.mem, false));
		if (visDispDesc.prop || visDispDesc.circle)
			MakeProperty_Child(visDispDesc, SSymbolInfo(pBaseType, symbol.mem), IsUdtExpand, SVisOption(option.depth-1, option.isApplyVisualizer));
		break;

	default:
		pBaseType->Release();
		break;
	}
}


//------------------------------------------------------------------------
// Pointer type Preview
//------------------------------------------------------------------------
SVisDispDesc visualizer::MakeProperty_ArrayData(SVisDispDesc parentDispdesc, 
												 const SSymbolInfo &symbol)
{
	wxPGProperty *pParentProp = parentDispdesc.prop;
	wxPGProperty *pProp = NULL;

	CComPtr<IDiaSymbol> pArrayType;
	HRESULT hr = symbol.pSym->get_type(&pArrayType); // ArrayDataType
	ASSERT_RETV(S_OK == hr, SVisDispDesc());

	CComPtr<IDiaSymbol> pElementType;	// 배열 개개의 타입
	hr = pArrayType->get_type(&pElementType);
	ASSERT_RETV(S_OK == hr, SVisDispDesc());

	enum SymTagEnum elemSymTag;
	hr = pElementType->get_symTag((DWORD*)&elemSymTag);
	assert(S_OK == hr);

	const string typeName = dia::GetSymbolTypeName(symbol.pSym);
	stringstream ss;
	string stringVal;
	ss << symbol.mem.name;

	if (//SymTagData == elemSymTag ||
		SymTagBaseType == elemSymTag) // BaseType Array
	{
		BasicType btype;
		hr = pElementType->get_baseType((DWORD*)&btype);
		ASSERT_RETV(S_OK == hr, SVisDispDesc());
		
		// char*, char[] 타입이라면 스트링을 출력한다.
		if (btChar == btype)
			stringVal = (char*)symbol.mem.ptr;
	}

	// todo : 나중에 preview visualizer 로 통합해야 한다.
	// string 타입일 때, 출력
	if (pParentProp)
	{
		CPropertyWindow::SPropItem *pItemData = 
			(CPropertyWindow::SPropItem*)pParentProp->GetClientData();
		if (pItemData)
		{
			if (!strncmp(pItemData->symbolTypeName.c_str(),  "std::basic_string",17 ))
			{
				pParentProp->SetValue( stringVal.c_str() );
			}
		}
	}

	CPropertyItemAdapter prop( ss.str(), CPropertyItemAdapter::PROPERTY_STRING, 0, stringVal );
	SVisDispDesc newPropDesc = AddProperty(parentDispdesc, prop, &symbol,  &STypeData(SymTagArrayType, VT_EMPTY, NULL) );
	return newPropDesc;
	//return SVisDispDesc(NULL, prop.GetProperty());
}


//------------------------------------------------------------------------
// Pointer Type Preview 
//------------------------------------------------------------------------
SVisDispDesc visualizer::MakeProperty_Pointer_Preview( SVisDispDesc parentDispdesc, 
	const SSymbolInfo &symbol )
{
	wxPGProperty *pProp = NULL;

	//CComPtr<IDiaSymbol> pPointerType;
	//HRESULT hr = symbol.pSym->get_type(&pPointerType);
	//ASSERT_RETV(hr == S_OK, pProp);  // PointerType

	//CComPtr<IDiaSymbol> pBaseType;
	//hr = pPointerType->get_type(&pBaseType);
	//ASSERT_RETV(hr == S_OK, pProp);  // BasicDataType or UDTDataType

	CComPtr<IDiaSymbol> pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RETV(hr == S_OK, SVisDispDesc());  // BasicDataType or UDTDataType

	void *srcPtr = (void*)*(DWORD*)symbol.mem.ptr;
	void *newPtr = MemoryMapping(srcPtr);
	if (!newPtr)
		newPtr = srcPtr; // 공유메모리에 없는 데이타일경우 주소만 출력한다.

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);

	const string name = dia::GetSymbolName(symbol.pSym); // debug용
	const string typeName = dia::GetSymbolTypeName(symbol.pSym);
	stringstream ss;

	if (SymTagUDT == baseSymTag)
	{
		ss << symbol.mem.name;// << " 0x" << newPtr;
		ss << (char*)(CheckValidStringAddress(newPtr)? " " : " {memory access deny}");
		//ss << " (" << typeName << ")";
	}
	else if (SymTagBaseType == baseSymTag)
	{
		BasicType btype;
		hr = pBaseType->get_baseType((DWORD*)&btype);
		ASSERT_RETV(hr == S_OK, SVisDispDesc());

		// char* 타입이라면 스트링을 출력한다.
		if (btChar == btype)
		{
			ss << symbol.mem.name;// << " 0x" << newPtr << " {\"";
			ss << (char*)(CheckValidStringAddress(newPtr)? " " : " {memory access deny}");
		}
	}

	if (ss.str().empty()) // default pointer 작업
	{
		ss << symbol.mem.name;// << " 0x" << newPtr;
		ss << (char*)(CheckValidStringAddress(newPtr)? " " : " {memory access deny}");
		//ss << " (" << typeName << ")";
	}

	CPropertyItemAdapter prop( ss.str(), CPropertyItemAdapter::PROPTYPE_POINTER, (DWORD)newPtr );
	SVisDispDesc newPropDesc = AddProperty( parentDispdesc, prop, &symbol, &STypeData(SymTagPointerType, VT_EMPTY, NULL));
	return newPropDesc;
	//return SVisDispDesc(NULL, prop.GetProperty());
}


//------------------------------------------------------------------------
// UDT type Preview
//------------------------------------------------------------------------
SVisDispDesc visualizer::MakeProperty_UDTData( SVisDispDesc parentDispdesc, 
	const SSymbolInfo &symbol, const SVisOption &option ) //  IsApplyVisualizer = true
{
	const string typeName = dia::GetSymbolTypeName(symbol.pSym);

	// 최상위 UDT가 아닐때만 타입을 출력한다.
	stringstream ss;
	ss << symbol.mem.name;

	CPropertyItemAdapter prop( ss.str());
	SVisDispDesc newPropDesc = AddProperty(parentDispdesc, prop, &symbol, &STypeData(SymTagUDT, VT_EMPTY, symbol.mem.ptr));

	const bool isExpand = (prop.GetProperty() && (prop.GetProperty()->GetChildCount() > 0)
		&& prop.GetProperty()->IsExpanded());

	bool isVisualizerType = false;
	// todo: visualizer preview 작업이 끝나면 없애야한다.
	if (option.isApplyVisualizer && !strncmp(typeName.c_str(),  "std::basic_string",17 ))
	{
		isVisualizerType = visualizer::MakeVisualizerProperty( newPropDesc, symbol, option.depth );
	}
	else if(option.isApplyVisualizer)
	{
		isVisualizerType = visualizer::MakeVisualizerProperty( newPropDesc, symbol, option.depth );
	}

	if (!isExpand && prop.GetProperty())
		prop.GetProperty()->SetExpanded(false);

	return newPropDesc;
	//return SVisDispDesc(NULL, (isVisualizerType)? NULL : prop.GetProperty());
}


//------------------------------------------------------------------------
// pSymbol : Array Type을 가리킨다. 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Array(SVisDispDesc parentDispdesc, 
	const SSymbolInfo &symbol, const bool IsUdtExpand, const SVisOption &option)
{
	wxPGProperty *pParentProp = parentDispdesc.prop;

	ULONGLONG length=0;
	HRESULT hr = symbol.pSym->get_length(&length);
	ASSERT_RET(S_OK == hr);

	IDiaSymbol* pElementType;	// 배열 개개의 타입
	hr = symbol.pSym->get_type(&pElementType);
	ASSERT_RET(S_OK == hr);

	ULONGLONG element_length=0;
	hr = pElementType->get_length(&element_length);
	ASSERT(S_OK == hr);

	enum SymTagEnum elemSymTag;
	hr = pElementType->get_symTag((DWORD*)&elemSymTag);
	ASSERT(S_OK == hr);

	char valueName[ 128];
	using boost::interprocess::basic_bufferstream;
	basic_bufferstream<char> formatter(valueName, sizeof(valueName));

	if (//SymTagData == elemSymTag ||
		SymTagBaseType == elemSymTag)
	{
		for (ULONGLONG i=0; i < length; i += element_length)
		{
			formatter.seekp(0);
			formatter << "[" << i / element_length << "]" << std::ends;

			void *ptr = (BYTE*)symbol.mem.ptr + i;
			SMemInfo arrayElem(valueName, ptr, (size_t)element_length);
			MakeProperty_BaseType(parentDispdesc, valueName, SSymbolInfo(pElementType, arrayElem, false));
		}
		if (pParentProp)
			pParentProp->SetExpanded(false);
	}
	else // UDT Array
	{
		for (ULONGLONG i=0; i < length; i += element_length)
		{
			formatter.seekp(0);
			formatter << "[" << i / element_length << "]" << std::ends;

			void *ptr = (BYTE*)symbol.mem.ptr + i;
			SMemInfo arrayElem(valueName, ptr, (size_t)element_length);
			SSymbolInfo arraySymbol(pElementType, arrayElem, false);

 			CPropertyItemAdapter prop( valueName );
			AddProperty( parentDispdesc, prop, &arraySymbol, &STypeData(SymTagUDT,VT_EMPTY,NULL));

			MakeProperty_Preview(SVisDispDesc(NULL, prop.GetProperty()), arraySymbol, IsUdtExpand, option);
		}
		if (pParentProp)
			pParentProp->SetExpanded(false);
	}
	pElementType->Release();
}


//------------------------------------------------------------------------
// m_wndPropList 에 Row 를 추가한다.
// pSymbol 은 데이타를 가르키는 심볼이어야 한다.
//------------------------------------------------------------------------
SVisDispDesc visualizer ::MakeProperty_BaseType( SVisDispDesc parentDispdesc, 
	const std::string valueName, 
	const SSymbolInfo &symbol )
{
	_variant_t value = dia::GetValueFromSymbol(symbol.mem.ptr, symbol.pSym);

	CPropertyItemAdapter prop( valueName, symbol, value );
	SVisDispDesc newPropDesc = AddProperty(parentDispdesc, prop, &symbol, 
		&STypeData(SymTagData, (prop.IsEnabled()? value.vt : VT_EMPTY), symbol.mem.ptr));

	return newPropDesc;
	//return SVisDispDesc(NULL, prop.GetProperty());
}


//------------------------------------------------------------------------
// Property추가
//------------------------------------------------------------------------
SVisDispDesc visualizer::AddProperty( SVisDispDesc parentDispdesc, 
								 CPropertyItemAdapter &propAdapter,
								 const SSymbolInfo *pSymbol,
								 STypeData *typeData )
{
	if (g_pProperty)
	{
		wxPGProperty *pParentProp = parentDispdesc.prop;
		wxPGProperty *prop = g_pProperty->AddProperty(pParentProp, propAdapter, pSymbol, typeData);
		return SVisDispDesc(g_pProperty, prop);
	}
	else if (g_pGraphWnd)
	{
		CStructureCircle *circle = g_pGraphWnd->AddDataGraph(parentDispdesc.circle,  pSymbol->mem.name, 
			propAdapter, pSymbol, typeData, parentDispdesc.alignGraph);
		return SVisDispDesc(NULL, NULL, g_pGraphWnd, circle, parentDispdesc.alignGraph);
	}

	return SVisDispDesc();
}

