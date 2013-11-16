//------------------------------------------------------------------------
// Name:    Variant.h
// Author:  jjuiddong
// Date:    1/30/2013
// 
// _variant_ wrapper
//------------------------------------------------------------------------
#ifndef __VARIANT_H__
#define __VARIANT_H__


namespace visualizer
{
	class CVariant : public _variant_t
	{
	public:
		CVariant() {}
		virtual ~CVariant() {}

// 		CVariant(const VARIANT& varSrc) ;
// 		CVariant(const VARIANT* pSrc) ;
		CVariant(const CVariant& varSrc) ;
//		CVariant(VARIANT& varSrc, bool fCopy) ;          // Attach VARIANT if !fCopy
// 		CVariant(short sSrc, VARTYPE vtSrc = VT_I2) ;    // Creates a VT_I2, or a VT_BOOL
// 		CVariant(long lSrc, VARTYPE vtSrc = VT_I4) ;     // Creates a VT_I4, a VT_ERROR, or a VT_BOOL
		CVariant(float fltSrc) throw();                                   // Creates a VT_R4
		CVariant(double dblSrc, VARTYPE vtSrc = VT_R8) ; // Creates a VT_R8, or a VT_DATE
//		_variant_t(const CY& cySrc) throw();                                // Creates a VT_CY
//		_variant_t(const _bstr_t& bstrSrc) ;               // Creates a VT_BSTR
		CVariant(const wchar_t *pSrc) ;                  // Creates a VT_BSTR
		CVariant(const char* pSrc) ;                     // Creates a VT_BSTR
//		_variant_t(IDispatch* pSrc, bool fAddRef = true) throw();           // Creates a VT_DISPATCH
		CVariant(bool boolSrc) throw();                                   // Creates a VT_BOOL
//		_variant_t(IUnknown* pSrc, bool fAddRef = true) throw();            // Creates a VT_UNKNOWN
//		_variant_t(const DECIMAL& decSrc) throw();                          // Creates a VT_DECIMAL
		CVariant(BYTE bSrc) throw();                                      // Creates a VT_UI1

		CVariant(char cSrc) throw();                                      // Creates a VT_I1
		CVariant(unsigned short usSrc) throw();                           // Creates a VT_UI2
		CVariant(unsigned long ulSrc) throw();                            // Creates a VT_UI4
		CVariant(int iSrc) throw();                                       // Creates a VT_INT
		CVariant(unsigned int uiSrc) throw();                             // Creates a VT_UINT
// #if (_WIN32_WINNT >= 0x0501)
// 		_variant_t(__int64 i8Src) throw();                                  // Creates a VT_I8
// 		_variant_t(unsigned __int64 ui8Src) throw();                        // Creates a VT_UI8
// #endif

	protected:
//		 m_Var;

	public:

	};


	CComVariant VariantCalc(char op, const CComVariant &lhs, const CComVariant &rhs);

	


}

#endif // __VARIANT_H__
