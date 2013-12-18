
#include "stdafx.h"
#include "PropertyItemAdapter.h"
#include "../dia/DiaWrapper.h"
#include <wx/propgrid/property.h>
#include <wx/propgrid/advprops.h>
#include "../Control/Global.h"

using namespace memmonitor;
using namespace visualizer;

CPropertyItemAdapter::CPropertyItemAdapter() :
	m_pProperty(NULL)
{
}

CPropertyItemAdapter::CPropertyItemAdapter( std::string label,  PROPERTY_TYPE type, DWORD ptr, std::string strVal ) :
	m_pProperty(NULL) // ptr = 0, strVal = ""
{
	switch (type)
	{
	case PROPERTY_STRING:
		m_pProperty = new wxStringProperty(label, wxPG_LABEL, strVal );
		m_Value = strVal;
		break;
	case PROPTYPE_CATEGORY:
		m_pProperty = new wxPropertyCategory(label, wxPG_LABEL );
		break;
	case PROPTYPE_ENUM:
		m_pProperty = new wxEnumProperty(label, wxPG_LABEL );	
		break;
	case PROPTYPE_POINTER:
		m_pProperty = new wxIntProperty( label, wxPG_LABEL, (int)ptr);
		m_Value = format("0x%x", ptr);
		break;
	}

	m_ValueName = label;
}

CPropertyItemAdapter::CPropertyItemAdapter(wxPGProperty *pProperty) :
	m_pProperty(pProperty)
{

}

CPropertyItemAdapter::CPropertyItemAdapter( const std::string &valueName, 
	const visualizer::SSymbolInfo &symbol, _variant_t value )
{
	CreateProperty(valueName, symbol, value );
}

CPropertyItemAdapter::~CPropertyItemAdapter()
{
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CPropertyItemAdapter::CreateProperty( const std::string &valueName, 
	const visualizer::SSymbolInfo &symbol, _variant_t value )
{
	// static 변수는 프로세스끼리 공유할 수없으므로 0으로 출력한다.
	bool IsNotPrint = false;
	LocationType locType;
	dia::GetSymbolLocation(symbol.pSym, &locType);
	if (LocIsStatic == locType)
	{
		value = 0;
		IsNotPrint  = true;
	}

	wxPGProperty *pProp = NULL;
	switch (value.vt)
	{
	case VT_I1:
	case VT_I2:
	case VT_I4:
	case VT_INT:
		pProp = new wxIntProperty( valueName, wxPG_LABEL, value);
		break;

	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
	case VT_UINT:
		pProp = new wxUIntProperty( valueName, wxPG_LABEL, value);
		break;

	case VT_R4:
	case VT_R8:
		pProp = new wxFloatProperty( valueName, wxPG_LABEL, value );
		break;

	case VT_BOOL:
		pProp = new wxBoolProperty( valueName, wxPG_LABEL, value );
		break;

	default:
		break;
	}

	RETV(!pProp, false);

	if (IsNotPrint)
		pProp->Enable(false);

	m_ValueName = valueName;
	m_ValueType = dia::GetSymbolTypeName(symbol.pSym);
	if (m_ValueType == "NoType") m_ValueType = "";
	m_Value = pProp->GetValueAsString();	
	m_pProperty = pProp;
	return true;
}


//------------------------------------------------------------------------
// SetValue with _variant_t value
//------------------------------------------------------------------------
void CPropertyItemAdapter::SetVariant(const _variant_t &var)
{
	RET(!m_pProperty);
	wxVariant wxVar = memmonitor::Variant2wxVariant(var);
	
	// bool 형일 때, enum 형태의 값으로 바꿔주어야 한다. 
	if (var.vt == VT_BOOL)
		wxVar = wxVariant((int)(var.bVal? true : false)); // bool 값은 widgets에서는 0/1 값이어야 한다.

	if (m_pProperty->GetChoices().GetCount()) // enum value
	{
		if (m_pProperty->GetChoices().GetCount() > (u_int)var)
			m_pProperty->SetValue( wxVar );
	}
	else
	{
		m_pProperty->SetValue( wxVar );
	}
}


//------------------------------------------------------------------------
// wrapper functions
//------------------------------------------------------------------------
void CPropertyItemAdapter::SetExpanded(bool expand)
{
	RET(!m_pProperty);
	m_pProperty->SetExpanded(expand);
}
void CPropertyItemAdapter::SetModifiedStatus(bool modify)
{
	RET(!m_pProperty);
	m_pProperty->SetModifiedStatus(modify);
}
void CPropertyItemAdapter::Enable(bool enable)
{
	RET(!m_pProperty);
	m_pProperty->Enable(enable);
}
void CPropertyItemAdapter::AddChoice(const std::string &name, const int value ) // value = wxPG_INVALID_VALUE
{
	RET(!m_pProperty);
	m_pProperty->AddChoice(name, value);
}
void CPropertyItemAdapter::SetValue(const wxVariant &var)
{
	RET(!m_pProperty);
	m_pProperty->SetValue(var);
}
bool CPropertyItemAdapter::IsEnabled()
{
	RETV(!m_pProperty, false);
	return m_pProperty->IsEnabled();
}

