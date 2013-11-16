
#include "stdafx.h"
#include "DefaultPropertyMaker.h"
#include "PropertyMaker.h"
#include "../dia/DiaWrapper.h"
#include "../ui/PropertyWindow.h"
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
		SVisOption() {}
		SVisOption(int _depth, bool _isApplyVisualizer) : depth(_depth), isApplyVisualizer(_isApplyVisualizer) {}
	}; 


	// make property
	void		MakeProperty_Preview(wxPGProperty *pParentProp, const SSymbolInfo &symbol,
		const bool IsUdtExpand, const SVisOption &option  );

	bool		MakeProperty_Child(  wxPGProperty *pParentProp,  const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );



	void		MakeProperty_UDTChild(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );

	void		MakeProperty_BaseClass(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );

	void		MakeProperty_Pointer_Children(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );

	void		MakeProperty_Data(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );

	void		MakeProperty_Array(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const SVisOption &option );


	wxPGProperty* MakeProperty_BaseType(wxPGProperty *pParentProp, 
		const std::string valueName,  const SSymbolInfo &symbol );

	wxPGProperty* MakeProperty_Pointer_Preview(wxPGProperty *pParentProp, 
		const SSymbolInfo &symbol );

 	wxPGProperty* MakeProperty_ArrayData(wxPGProperty *pParentProp, 
 		const SSymbolInfo &symbol );

	wxPGProperty* MakeProperty_UDTData(wxPGProperty *pParentProp, 
		const SSymbolInfo &symbol, const SVisOption &option );

	wxPGProperty* MakeProperty_BaseClassData(wxPGProperty *pParentProp, 
		const SSymbolInfo &symbol);

	wxPGProperty*  AddProperty(wxPGProperty *pParentProp, 
		CPropertyItemAdapter &propAdapter,  const SSymbolInfo *pSymbol, 
		STypeData *pTypeData);

	_variant_t GetValue(IDiaSymbol *pSymbol, void *pMemPtr);

	// variables
	CPropertyWindow *g_pProperty = NULL;
}

using namespace dia;
using namespace std;
using namespace visualizer;
using namespace memmonitor;


/**
@brief  property창에 심볼을 출력한다.
@param symbolName : {symbolName#count} 메모리 정보를 얻어올 수 있는 형태
*/
bool	visualizer::MakeProperty_DefaultForm( CPropertyWindow *pProperties,  wxPGProperty *pParentProp,
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
	MakeProperty_DefaultForm(pProperties, pParentProp, SSymbolInfo(pSymbol, memInfo), IsApplyVisualizer, depth);
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm(  CPropertyWindow *pProperties,
				wxPGProperty *pParentProp,  const SSymbolInfo &symbol,
				const bool IsApplyVisualizer, const int depth)
{
	if (!symbol.mem.ptr)
		return true;

	g_pProperty = pProperties;
	MakeProperty_Preview(pParentProp, symbol, true, SVisOption(depth, IsApplyVisualizer));
	return true;
}


//------------------------------------------------------------------------
//  symbol.pSym 의 자식을 pParentProp에 추가한다.
//------------------------------------------------------------------------
bool	 visualizer::MakePropertyChild_DefaultForm(  CPropertyWindow *pProperties, 
												   wxPGProperty *pParentProp,  const SSymbolInfo &symbol, 
												   const bool IsApplyVisualizer, const int depth)
{
	if (!symbol.mem.ptr)
		return true;

	g_pProperty = pProperties;

	bool isVisualizerType = false;
	if (IsApplyVisualizer)
		isVisualizerType = visualizer::MakeVisualizerProperty( pProperties, pParentProp, symbol, depth );

	if (!isVisualizerType)
		MakeProperty_Child(pParentProp, symbol, true, SVisOption(depth, IsApplyVisualizer));
	return true;
}


//------------------------------------------------------------------------
//  symbol을 pParentProp의 자식으로 추가한다.
//------------------------------------------------------------------------
bool	 visualizer::MakeProperty_Child(  wxPGProperty *pParentProp,  const SSymbolInfo &symbol, 
	const bool IsUdtExpand, const SVisOption &option )
{
	const bool IsChildExpand = (pParentProp && pParentProp->GetChildCount() > 0);
	if (option.depth <= 0 && !IsChildExpand)
		return true;
	enum SymTagEnum symtag;
	if (S_OK != symbol.pSym->get_symTag((DWORD*)&symtag))
		return true;

	switch (symtag)
	{
	case SymTagArrayType:
		MakeProperty_Array(pParentProp, symbol, IsUdtExpand, option);
		break;

	case SymTagPointerType:
		MakeProperty_Pointer_Children(pParentProp, symbol, IsUdtExpand, option);
		break;

	case SymTagBaseClass:
		MakeProperty_BaseClass(pParentProp, symbol, IsUdtExpand, option);
		break;

	case SymTagUDT:
		MakeProperty_UDTChild(pParentProp, symbol, IsUdtExpand, option);
		break;

	case SymTagData: 
		{
			IDiaSymbol *pBaseType;
			HRESULT hr = symbol.pSym->get_type(&pBaseType);
			ASSERT_RETV(S_OK == hr, false);
			MakeProperty_Child(pParentProp, SSymbolInfo(pBaseType,symbol.mem), 
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
void visualizer::MakeProperty_Preview(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
	const bool IsUdtExpand, const SVisOption &option )
{
	const bool IsChildExpand = (pParentProp && pParentProp->GetChildCount() > 0);
	if (option.depth <= 0 && !IsChildExpand)
		return;
	enum SymTagEnum symtag;
	if (S_OK != symbol.pSym->get_symTag((DWORD*)&symtag))
		return;

	wxPGProperty *pProp = NULL;
	switch (symtag)
	{
	case SymTagBaseType:
		MakeProperty_BaseType(pParentProp, symbol.mem.name, symbol);
		break;

	case SymTagData: 
		MakeProperty_Data(pParentProp, symbol, IsUdtExpand, option);
		break;

	case SymTagArrayType:
		MakeProperty_Array(pParentProp, symbol, IsUdtExpand, option);
		break;

	case SymTagPointerType:
		pProp = MakeProperty_Pointer_Preview(pParentProp, symbol);
		if (pProp)
			MakeProperty_Child(pProp, symbol, IsUdtExpand, option);
		break;

	case SymTagBaseClass:
		pProp = MakeProperty_BaseClassData(pParentProp, symbol);
		if (pProp)
			MakeProperty_Child(pProp, symbol, IsUdtExpand, option);
		break;

	case SymTagUDT:
		pProp = MakeProperty_UDTData(pParentProp, symbol, option);
		if (pProp)
			MakeProperty_Child(pProp, symbol, IsUdtExpand, option);
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
void	visualizer ::MakeProperty_BaseClass(wxPGProperty *pParentProp, 
											const SSymbolInfo &symbol, 
											const bool IsUdtExpand, const SVisOption &option )
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	LocationType locType;
	const LONG offset = dia::GetSymbolLocation(symbol.pSym, &locType);
	BYTE *ptr = (BYTE*)symbol.mem.ptr + offset;
	SMemInfo newMemInfo(symbol.mem.name.c_str(), ptr, 0);
	MakeProperty_Child(pParentProp, SSymbolInfo(pBaseType, newMemInfo), IsUdtExpand, option);
}


/**
@brief  baseclass type preview
*/
wxPGProperty* visualizer::MakeProperty_BaseClassData(
	wxPGProperty *pParentProp, const SSymbolInfo &symbol)
{
	CPropertyItemAdapter prop( symbol.mem.name );
	AddProperty(pParentProp, prop, &symbol, 
		&STypeData(SymTagBaseClass, VT_EMPTY, NULL));
	return prop.GetProperty();
}


//------------------------------------------------------------------------
// User Define Type 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_UDTChild(wxPGProperty *pParentProp, 
										const SSymbolInfo &symbol, 
										const bool IsUdtExpand, const SVisOption &option)
{
	RET (!pParentProp);
	
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
				MakeProperty_Preview(pParentProp, SSymbolInfo(pChild, memberMemInfo), 
					childIsUdtExpand, SVisOption(childDepth, option.isApplyVisualizer));
			}
		}
	}
	if (!IsChildExpand || (IsChildExpand && !pParentProp->IsExpanded()))
		pParentProp->SetExpanded(false);
}


//------------------------------------------------------------------------
// Pointer 타입 출력 
//------------------------------------------------------------------------
void	visualizer::MakeProperty_Pointer_Children(wxPGProperty *pParentProp, const SSymbolInfo &symbol,
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
			SMemInfo ptrMemInfo(symbol.mem.name.c_str(), newPtr, (size_t)0);
			MakeProperty_Child(pParentProp, SSymbolInfo(pBaseType, ptrMemInfo, false), 
				IsUdtExpand, option);
		}
	}
	pBaseType->Release();
}


//------------------------------------------------------------------------
// Property창에 Control을 추가한다.
// 변수 이름과 타입, 값을 설정한다.
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Data(wxPGProperty *pParentProp, const SSymbolInfo &symbol,
									const bool IsUdtExpand, const SVisOption &option)
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);
	ASSERT_RET(hr == S_OK);

	wxPGProperty *pPgProp = NULL;
	switch (baseSymTag)
	{
	case SymTagBaseType:
		{
			string valueTypeName = symbol.mem.name;// + " (" + dia::GetSymbolTypeName(symbol.pSym) + ")";
			MakeProperty_BaseType( pParentProp, valueTypeName, symbol );
		}
		break;

	case SymTagEnum:
		{
			std::string typeName = dia::GetSymbolTypeName(pBaseType);
			std::string valueTypeName =  symbol.mem.name;// + " (" +  typeName + ")";

			CPropertyItemAdapter prop( valueTypeName,  CPropertyItemAdapter::PROPTYPE_ENUM ); 
			AddProperty(pParentProp, prop, &symbol, &STypeData(baseSymTag, VT_UI4, symbol.mem.ptr));

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
		pPgProp = MakeProperty_UDTData(pParentProp, symbol, option);
		if (pPgProp)
			MakeProperty_Child(pPgProp, SSymbolInfo(pBaseType, symbol.mem), IsUdtExpand, SVisOption(option.depth-1, option.isApplyVisualizer));
		break;

	case SymTagArrayType:
		pPgProp = MakeProperty_ArrayData(pParentProp, symbol);
		if (pPgProp)
			MakeProperty_Child(pPgProp, SSymbolInfo(pBaseType, symbol.mem), IsUdtExpand, SVisOption(option.depth-1, option.isApplyVisualizer));
		break;

	case SymTagPointerType:
		pPgProp = MakeProperty_Pointer_Preview(pParentProp, SSymbolInfo(pBaseType, symbol.mem));
		if (pPgProp)
			MakeProperty_Child(pPgProp, SSymbolInfo(pBaseType, symbol.mem, false), IsUdtExpand, SVisOption(option.depth-1, option.isApplyVisualizer));
		break;

	default:
		pBaseType->Release();
		break;
	}
}


//------------------------------------------------------------------------
// Pointer type Preview
//------------------------------------------------------------------------
wxPGProperty* visualizer::MakeProperty_ArrayData(wxPGProperty *pParentProp, 
												 const SSymbolInfo &symbol )
{
	wxPGProperty *pProp = NULL;

	CComPtr<IDiaSymbol> pArrayType;
	HRESULT hr = symbol.pSym->get_type(&pArrayType); // ArrayDataType
	ASSERT_RETV(S_OK == hr, pProp);

	CComPtr<IDiaSymbol> pElementType;	// 배열 개개의 타입
	hr = pArrayType->get_type(&pElementType);
	ASSERT_RETV(S_OK == hr, pProp);

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
		ASSERT_RETV(S_OK == hr, pProp);
		
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
	AddProperty(pParentProp, prop, &symbol,  &STypeData(SymTagArrayType, VT_EMPTY, NULL) );

	return prop.GetProperty();
}


//------------------------------------------------------------------------
// Pointer Type Preview 
//------------------------------------------------------------------------
wxPGProperty* visualizer::MakeProperty_Pointer_Preview(
	wxPGProperty *pParentProp, const SSymbolInfo &symbol )
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
	ASSERT_RETV(hr == S_OK, pProp);  // BasicDataType or UDTDataType

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
		ss << (char*)(CheckValidAddress(newPtr)? " " : " not shared memory");
		//ss << " (" << typeName << ")";
	}
	else if (SymTagBaseType == baseSymTag)
	{
		BasicType btype;
		hr = pBaseType->get_baseType((DWORD*)&btype);
		ASSERT_RETV(hr == S_OK, pProp);

		// char* 타입이라면 스트링을 출력한다.
		if (btChar == btype)
		{
			ss << symbol.mem.name;// << " 0x" << newPtr << " {\"";
			ss << (char*)(CheckValidAddress(newPtr)? newPtr : " not shared memory")  << "\"}";
		}
	}

	if (ss.str().empty()) // default pointer 작업
	{
		ss << symbol.mem.name;// << " 0x" << newPtr;
		ss << (char*)(CheckValidAddress(newPtr)? " " : " not shared memory");
		//ss << " (" << typeName << ")";
	}

	CPropertyItemAdapter prop( ss.str(), CPropertyItemAdapter::PROPTYPE_POINTER, (DWORD)newPtr );
	AddProperty( pParentProp, prop, &symbol, &STypeData(SymTagPointerType, VT_EMPTY, NULL));

	return prop.GetProperty();
}


//------------------------------------------------------------------------
// UDT type Preview
//------------------------------------------------------------------------
wxPGProperty* visualizer::MakeProperty_UDTData(
	wxPGProperty *pParentProp, const SSymbolInfo &symbol, const SVisOption &option ) //  IsApplyVisualizer = true
{
	const string typeName = dia::GetSymbolTypeName(symbol.pSym);

	// 최상위 UDT가 아닐때만 타입을 출력한다.
	stringstream ss;
	ss << symbol.mem.name;

	CPropertyItemAdapter prop( ss.str());
	AddProperty(pParentProp, prop, &symbol, &STypeData(SymTagUDT, VT_EMPTY, symbol.mem.ptr));

	const bool isExpand = (prop.GetProperty() && (prop.GetProperty()->GetChildCount() > 0)
		&& prop.GetProperty()->IsExpanded());

	bool isVisualizerType = false;
	// todo: visualizer preview 작업이 끝나면 없애야한다.
	if (option.isApplyVisualizer && !strncmp(typeName.c_str(),  "std::basic_string",17 ))
	{
		isVisualizerType = visualizer::MakeVisualizerProperty( g_pProperty, prop.GetProperty(), symbol, option.depth );
	}
	else if(option.isApplyVisualizer)
	{
		isVisualizerType = visualizer::MakeVisualizerProperty( g_pProperty, prop.GetProperty(), symbol, option.depth );
	}

	if (!isExpand)
		prop.GetProperty()->SetExpanded(false);

	return (isVisualizerType)? NULL : prop.GetProperty();
}


//------------------------------------------------------------------------
// pSymbol : Array Type을 가리킨다. 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Array(wxPGProperty *pParentProp, 
	const SSymbolInfo &symbol, const bool IsUdtExpand, const SVisOption &option)
{
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
			MakeProperty_BaseType(pParentProp, valueName, SSymbolInfo(pElementType, arrayElem, false));
		}
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
			AddProperty( pParentProp, prop, &arraySymbol, &STypeData(SymTagUDT,VT_EMPTY,NULL));

			MakeProperty_Preview(prop.GetProperty(), arraySymbol, IsUdtExpand, option);
		}
		pParentProp->SetExpanded(false);
	}
	pElementType->Release();
}


//------------------------------------------------------------------------
// m_wndPropList 에 Row 를 추가한다.
// pSymbol 은 데이타를 가르키는 심볼이어야 한다.
//------------------------------------------------------------------------
wxPGProperty* visualizer ::MakeProperty_BaseType(
	wxPGProperty *pParentProp, const std::string valueName, 
	const SSymbolInfo &symbol )
{
	_variant_t value = dia::GetValueFromSymbol(symbol.mem.ptr, symbol.pSym);

	CPropertyItemAdapter prop( valueName, symbol, value );
	AddProperty(pParentProp, prop, &symbol, 
		&STypeData(SymTagData, (prop.IsEnabled()? value.vt : VT_EMPTY), symbol.mem.ptr));

	return prop.GetProperty();
}


//------------------------------------------------------------------------
// Property추가
//------------------------------------------------------------------------
wxPGProperty* visualizer ::AddProperty(
								 wxPGProperty *pParentProp, 
								 CPropertyItemAdapter &propAdapter,
								 const SSymbolInfo *pSymbol,
								 STypeData *typeData)
{
	RETV(!g_pProperty, NULL);

	return g_pProperty->AddProperty(pParentProp, propAdapter,  pSymbol, typeData);
}
