/**
 @filename GraphWindow.h
 
 
*/
#pragma once


namespace memmonitor
{
	class CGraphWindow : public wxPanel
	{
	public:
		CGraphWindow(wxWindow *parent);
		virtual ~CGraphWindow();
		void UpdateSymbol( const wxString &symbolName );


	protected:


	private:
		wxString	m_CurrentSymbolName;
		wxTimer	m_Timer;
	};
}
