#ifndef MENUEVENT_H_INCLUDED
#define MENUEVENT_H_INCLUDED

class CMainFrame;
class CExecuteMenuOperation;
class CMenuEvent : public wxEvtHandler
{
public:
	CMenuEvent(CMainFrame* _pMainFrame);
	~CMenuEvent();

	void SetMainFrame(CMainFrame* pMainFrame);
	void InitToolbarItems(wxAuiToolBar* pAuiToolbar);
	void LoadBookmarkToMenu(bool IsFromFavoriteMangaer = false);
	void SetBookmarkMenuEvent(int nMenuID);

	void DoMenuOperation(int nMenuID);
private:
	void Init();

private:
	CMainFrame* m_pMainFrame;

	std::unordered_map<int, _MENU_EVENT_TYPE> m_mapMenu;
	CExecuteMenuOperation* m_pMenuOperation;

	std::unordered_map<int, wxString> m_mapToolBarMenu;
	wxAuiToolBar* m_pToolBar = nullptr;

private:
	void OnMenuEvent(wxCommandEvent& event);
	void OnMenuEventUpdate(wxUpdateUIEvent& event);
	void OnToolbarEvent(wxCommandEvent& event);
	void OnDropdownBookmark(wxAuiToolBarEvent& event);
	void OnCompressExecute(wxCommandEvent& event);
	void OnSelectedBookmark(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif // MENUEVENT_H_INCLUDED
