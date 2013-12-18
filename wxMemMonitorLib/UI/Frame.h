//------------------------------------------------------------------------
// Name:    Frame.h
// Author:  jjuiddong
// Date:    3/5/2013
// 
// Basic Frame class
//------------------------------------------------------------------------
#pragma once

#include <wx/aui/aui.h>

namespace visualizer { struct SSymbolInfo; }

namespace memmonitor
{
	class CFrame : public wxFrame 
	{
	public:
		typedef std::list<wxMiniFrame*> PropertyFrames;

		CFrame(wxWindow* parent);
		virtual ~CFrame();

		bool AddPropertyWindow(const wxString &symbolName );
		bool AddGraphWindow(const string &symbolName, const string &varName, const visualizer::SSymbolInfo &symbol );
		void UpdatePaneSize(wxWindow *pWindow, int w, int h);

		MemTreePtr GetMemTree() const;
		LogWindowPtr GetLogWindow() const;
		PropWindowPtr GetPropWindow() const;
		const PropertyFrames& GetPropFrames() const;

	protected:
		void CreateMenuBar();

		// Event Handler
		DECLARE_EVENT_TABLE()
		void OnMenuOpenAutoExp(wxCommandEvent& event);
		void OnMenuReloadAutoExp(wxCommandEvent& event);
		void OnMenuExit(wxCommandEvent& event);
		void OnMenuHelp(wxCommandEvent& event);
		void OnPropertyFrameClose(wxCloseEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		
	private:
		wxAuiManager m_mgr;
		MemTreePtr m_pMemTree;
		LogWindowPtr m_pLogWnd;
		PropWindowPtr m_pPropWnd;
		PropertyFrames m_PropFrames;
	};

	inline MemTreePtr CFrame::GetMemTree() const { return m_pMemTree; }
	inline LogWindowPtr CFrame::GetLogWindow() const { return m_pLogWnd; }
	inline PropWindowPtr CFrame::GetPropWindow() const { return m_pPropWnd; }
	inline const CFrame::PropertyFrames& CFrame::GetPropFrames() const { return m_PropFrames; }
}
