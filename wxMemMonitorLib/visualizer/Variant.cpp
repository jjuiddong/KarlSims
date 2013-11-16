
#include "stdafx.h"
#include "Variant.h"

using namespace visualizer;


CVariant::CVariant(const CVariant& varSrc) : _variant_t(varSrc)
{
}

//		CVariant(VARIANT& varSrc, bool fCopy) ;          // Attach VARIANT if !fCopy
// 		CVariant(short sSrc, VARTYPE vtSrc = VT_I2) ;    // Creates a VT_I2, or a VT_BOOL
// 		CVariant(long lSrc, VARTYPE vtSrc = VT_I4) ;     // Creates a VT_I4, a VT_ERROR, or a VT_BOOL
CVariant::CVariant(float fltSrc) : _variant_t(fltSrc)
{
}

CVariant::CVariant(double dblSrc, VARTYPE vtSrc) : _variant_t(dblSrc,vtSrc) // Creates a VT_R8, or a VT_DATE
{
}

//		_variant_t(const CY& cySrc) throw();                                // Creates a VT_CY
//		_variant_t(const _bstr_t& bstrSrc) ;               // Creates a VT_BSTR
CVariant::CVariant(const wchar_t *pSrc) : _variant_t(pSrc)// Creates a VT_BSTR
{
}

CVariant::CVariant(const char* pSrc) : _variant_t(pSrc)                     // Creates a VT_BSTR
{
}

//		_variant_t(IDispatch* pSrc, bool fAddRef = true) throw();           // Creates a VT_DISPATCH
CVariant::CVariant(bool boolSrc) : _variant_t(boolSrc)                                   // Creates a VT_BOOL
{
}

//		_variant_t(IUnknown* pSrc, bool fAddRef = true) throw();            // Creates a VT_UNKNOWN
//		_variant_t(const DECIMAL& decSrc) throw();                          // Creates a VT_DECIMAL
CVariant::CVariant(BYTE bSrc) : _variant_t(bSrc)                                      // Creates a VT_UI1
{
}

CVariant::CVariant(char cSrc) : _variant_t(cSrc)                                      // Creates a VT_I1
{
}

CVariant::CVariant(unsigned short usSrc) : _variant_t(usSrc)                           // Creates a VT_UI2
{
}

CVariant::CVariant(unsigned long ulSrc) : _variant_t(ulSrc)                            // Creates a VT_UI4
{
}

CVariant::CVariant(int iSrc) : _variant_t(iSrc)                                       // Creates a VT_INT
{
}

CVariant::CVariant(unsigned int uiSrc) : _variant_t(uiSrc)                             // Creates a VT_UINT
{
}


template<class T>
CComVariant _VariantCalc(char op, const T &lhs, const T&rhs )
{
	CComVariant reval;
	switch (op)
	{
	case '+': reval = lhs + rhs; break;
	case '-':
	case '*':
	case '/':
		break;		
	}
	return reval;
}


// op : ascii operator (ex : '+', '-' ~~~ )
CComVariant visualizer::VariantCalc(char op, const CComVariant &lhs, const CComVariant &rhs)
{
	CComVariant left = lhs, right = rhs;
	CComVariant reval;

	if (lhs.vt == rhs.vt)
	{
		reval = _VariantCalc(op, lhs.intVal, rhs.intVal);
	}
	else
	{
//		switch (

	}





	return reval;
}
