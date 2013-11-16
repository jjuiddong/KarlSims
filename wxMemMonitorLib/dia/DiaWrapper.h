//------------------------------------------------------------------------
// Name:    DiaWrapper.h
// Author:  jjuiddong
// Date:    1/10/2013
// 
// Dia SDK Wrapper class
//------------------------------------------------------------------------
#pragma once

#include <cvconst.h>

struct IDiaDataSource;
struct IDiaSession;
struct IDiaSymbol;

namespace dia
{
	bool						Init(const std::string &pdbFileName);
	void						Cleanup();

	IDiaSymbol*		FindType(const std::string &typeName);
	IDiaSymbol*		FindChildSymbol( const std::string &symbolName, IDiaSymbol *pSymbol, 
															OUT LONG *pOffset=NULL );

	enum SymbolState { NEW_SYMBOL, PARAM_SYMBOL };
	IDiaSymbol*		GetBaseTypeSymbol( IDiaSymbol *pSymbol, DWORD option, OUT SymbolState &result  );

	std::string			GetSymbolName(IDiaSymbol *pSymbol);
	LONG					GetSymbolLocation(IDiaSymbol *pSymbol, OUT LocationType *pLocType=NULL);
	ULONGLONG		GetSymbolLength(IDiaSymbol *pSymbol);

	std::string			GetSymbolTypeName(IDiaSymbol *pSymbol, bool addOptionName=true);
	std::string			GetBasicTypeName(BasicType btype, ULONGLONG length);
	_variant_t			GetValueFromAddress(void *srcPtr, const BasicType btype, const ULONGLONG length );
	_variant_t			GetValueFromSymbol(void *srcPtr,  IDiaSymbol *pSymbol);
	_variant_t			GetValue(void *srctPtr, VARTYPE varType);
	void						SetValue(void *destPtr, _variant_t value);

}
