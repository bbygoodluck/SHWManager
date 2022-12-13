#ifndef CMAINFRAME_H_INCLUDED
#define CMAINFRAME_H_INCLUDED

class CSHWStatusBar;

class CMainFrame : public wxFrame
{
public:
	CMainFrame(const wxString& strTitle);
	virtual ~CMainFrame();

	wxAuiToolBar* GetAuiToolbar() { return m_pAuiToolbar.get(); }

private:
	void Initialize();
	void RegisterEventHandler();
	void CreateToolBar();
	void CreateCustomStatusBar();

#ifdef __WXMSW__
	virtual WXLRESULT MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
	void OnDeviceChange(WXWPARAM nEventType, WXLPARAM dwData);
#endif

private:
	wxAuiManager	m_mgr;
	std::unique_ptr<wxAuiToolBar>   m_pAuiToolbar = nullptr;

    bool m_bMaximize    = false;
    bool m_bInitialized = false;

#ifdef __WXMSW__
	HDEVNOTIFY m_hNotifyDev;
#endif

	std::unique_ptr<CSHWStatusBar> m_pStatusBar;

private:
   	void OnClose(wxCloseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMaximized(wxMaximizeEvent& event);
	void OnIconized(wxIconizeEvent& event);

	//마우스 이벤트
	void OnMouseLBottonDown(wxMouseEvent& event);

    wxDECLARE_EVENT_TABLE();
};

#endif // CMAINFRAME_H_INCLUDED
