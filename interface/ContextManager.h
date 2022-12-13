#ifndef VIEWMANAGERIMPL_H_INCLUDED
#define VIEWMANAGERIMPL_H_INCLUDED

class CTabManager;
class CViewPanel;
class CPathView;
class CFileListView;
class CMainFrame;

class CContextManager// : public wxEvtHandler
{
public:
	CContextManager();
	~CContextManager();

public:
	void SetMainFrame(CMainFrame* pMainFrame);
	void ClearSelectedItemsOfActiveView();

	void AddFileListViewItem(wxWindow *pFileListView);
	void DeleteFileListViewItem(wxWindow* pViewPanel);

	wxWindow* GetActivateViewItem();
	wxWindow* GetNotActivateViewItem();

	void SetChangeDirectory(wxCommandEvent& event);
	void ChangeTab();
	void CheckEqualAndChangeTab(wxWindow* pWindow);

	void ActivateView();
	wxWindow* GetActivatePanel(CTabManager* pTabManager);

#ifdef __WXMSW__
	void UpdateDriveInformation();
	void UpdateDriveSpace(const wxString& strVolume);
#endif // __WXMSW__

	void ChangeColumn();
	void ChangeFileList();
	void ChangeFileListSort();
	void GotoBookmarkPath(const wxString& strPath);
	void ShowBookmarkWhenStatusbarClick();

private:
	CTabManager* GetNotActivateTab();
	void SendViewChangeEvent(wxEventType evtType);

private:
	CMainFrame* m_pMainFrame = nullptr;
	wxVector<wxWindow *> m_vecViewItems;
/*
private:
	wxDECLARE_EVENT_TABLE();
*/

};

#endif // VIEWMANAGERIMPL_H_INCLUDED
