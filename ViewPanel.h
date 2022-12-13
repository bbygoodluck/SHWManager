#ifndef VIEWPANEL_H_INCLUDED
#define VIEWPANEL_H_INCLUDED

class CPathView;
class CLocalFileListView;
class CTabManager;

class CViewPanel : public wxPanel
{
	friend class CLocalListView;

public:
	explicit CViewPanel(wxWindow* parent, int iID, const wxString& strPath, bool bRemote = false);
	virtual ~CViewPanel();

	CFileListView* GetFileListView();
	CPathView* GetPathView();
	CTabManager* GetTabManager();

protected:
	CTabManager* m_pTabManager   = nullptr;

private:
    void CreatePanelItems(bool bRemote = false);

private:
	wxBoxSizer* m_Mainsizer;
    bool m_bRemote;

    wxString m_strPath = wxT("");

    //패스정보
	CPathView* m_pPathView = nullptr;
	// 폴더/파일 리스트뷰
	CLocalFileListView* m_pFileListView = nullptr;

private:
	void OnErase(wxEraseEvent& event);
	void OnChangeDirectory(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};
#endif // VIEWPANEL_H_INCLUDED
