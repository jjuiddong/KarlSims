//------------------------------------------------------------------------
// Name:    PropertyItemAdapter.h
// Author:  jjuiddong
// Date:    3/10/2013
// 
// using wxWidgets Property library
// help property class
//------------------------------------------------------------------------
#pragma once

#include "../visualizer/VisualizerGlobal.h"
#include <wx/propgrid/property.h>


class wxPGProperty;
namespace visualizer
{
	class CPropertyItemAdapter
	{
	public:
		enum PROPERTY_TYPE {
			PROPERTY_STRING,
			//PROPERTY_PARENT,
			PROPTYPE_CATEGORY,
			PROPTYPE_ENUM,
			PROPTYPE_POINTER,
		};

		CPropertyItemAdapter();
		CPropertyItemAdapter( wxPGProperty *pProperty );
		CPropertyItemAdapter( std::string label,  PROPERTY_TYPE type = PROPERTY_STRING, DWORD ptr=0, std::string strVal="" );
		CPropertyItemAdapter( const std::string &valueName, 
			const visualizer::SSymbolInfo &symbol, _variant_t value );

		virtual ~CPropertyItemAdapter();

		void SetProperty(wxPGProperty* prop);
		wxPGProperty* GetProperty();
		void SetValue(const wxVariant &var);
		void SetVariant(const _variant_t &var);
		void SetExpanded(bool expand);
		void SetModifiedStatus(bool modify);
		void Enable(bool enable);
		bool IsEnabled();
		void AddChoice(const std::string &name, const int value=wxPG_INVALID_VALUE );
		string GetValueName() const;
		string GetValue() const;
		string GetValueType() const;


	protected:
		bool CreateProperty( const std::string &valueName, const SSymbolInfo &symbol, _variant_t value );


	protected:
		wxPGProperty *m_pProperty;
		string m_ValueType;
		string m_ValueName;
		string m_Value;
	};


	inline void CPropertyItemAdapter::SetProperty(wxPGProperty* prop) { m_pProperty = prop; }
	inline wxPGProperty* CPropertyItemAdapter::GetProperty() { return m_pProperty; }
	inline string CPropertyItemAdapter::GetValueName() const { return m_ValueName; }
	inline string CPropertyItemAdapter::GetValue() const { return m_Value; }
	inline string CPropertyItemAdapter::GetValueType() const { return m_ValueType; }
}
