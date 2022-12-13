#include "../ginc.h"
#include "../MainFrame.h"
#include "../TabManager.h"
#include "../ViewPanel.h"
#include "../view/FileListView.h"
#include "../view/PathView.h"
/*
wxBEGIN_EVENT_TABLE(CContextManager, wxEvtHandler)
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Custom event
	////////////////////////////////////////////////////////////////////////////////////////////////////////
//	EVT_MY_CUSTOM_COMMAND(wxEVT_CHANGE_DIRECTORY, wxID_ANY, CContextManager::OnChangeDirectory)
wxEND_EVENT_TABLE()
*/
CContextManager::CContextManager()
{

}

CContextManager::~CContextManager()
{
	m_vecViewItems.clear();
}

void CContextManager::SetMainFrame(CMainFrame* pMainFrame)
{
	m_pMainFrame = pMainFrame;
}

void CContextManager::AddFileListViewItem(wxWindow *pFileListView)
{
	m_vecViewItems.push_back(pFileListView);
}

void CContextManager::DeleteFileListViewItem(wxWindow* pViewPanel)
{
	wxWindow* pDelWindow = ((CViewPanel *)pViewPanel)->GetFileListView();
	if(!pDelWindow)
		return;

	wxVector<wxWindow *>::iterator iter = m_vecViewItems.begin();
	for(iter; iter != m_vecViewItems.end(); ++iter)
	{
		if(*iter == pDelWindow)
			break;
	}

	if(iter != m_vecViewItems.end())
		m_vecViewItems.erase(iter);
}

wxWindow* CContextManager::GetActivateViewItem()
{
	CViewPanel* pActiveViewPanel = (CViewPanel *)GetActivatePanel(_gActivatedTab);
	return pActiveViewPanel->GetFileListView();
}

wxWindow* CContextManager::GetNotActivateViewItem()
{
	CTabManager* pTabManager = GetNotActivateTab();
	CViewPanel* pNonActiveViewPanel = (CViewPanel *)GetActivatePanel(pTabManager);

	return pNonActiveViewPanel->GetFileListView();
}

void CContextManager::ActivateView()
{
	CViewPanel* pActiveViewPanel = (CViewPanel *)GetActivatePanel(_gActivatedTab);
	pActiveViewPanel->GetFileListView()->SetFocus();
}

CTabManager* CContextManager::GetNotActivateTab()
{
	CTabManager* pTabManager = nullptr;
	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
	{
		CTabManager* tabWnd1 = (CTabManager *)theSplitterManager->GetSplitter()->GetWindow1();
		CTabManager* tabWnd2 = (CTabManager *)theSplitterManager->GetSplitter()->GetWindow2();

		pTabManager = _gActivatedTab == tabWnd1 ? tabWnd2 : tabWnd1;
	}

	return pTabManager;
}

void CContextManager::ChangeTab()
{
	CTabManager* pTabManagerPtr = GetNotActivateTab();

	if(pTabManagerPtr)
	{
		_gActivatedTab = pTabManagerPtr;

		CViewPanel* pActiveViewPanel = (CViewPanel *)GetActivatePanel(_gActivatedTab);
		pActiveViewPanel->GetFileListView()->SetFocus();
	}
}

void CContextManager::CheckEqualAndChangeTab(wxWindow* pWindow)
{
	CTabManager* pTab = (CTabManager *)pWindow;
	if(pTab != _gActivatedTab)
	{
		ChangeTab();
		return;
	}

	ActivateView();
}

wxWindow* CContextManager::GetActivatePanel(CTabManager* pTabManager)
{
	int nSelectionPageNum = pTabManager->GetSelection();
	if(nSelectionPageNum < 0)
		return nullptr;

	return pTabManager->GetPage(nSelectionPageNum);
}

void CContextManager::ClearSelectedItemsOfActiveView()
{
	CViewPanel* pActiveViewPanel = (CViewPanel *)GetActivatePanel(_gActivatedTab);
	pActiveViewPanel->GetFileListView()->SelectedItemsClear();
}

#ifdef __WXMSW__
void CContextManager::UpdateDriveInformation()
{
	wxVector<wxWindow *>::iterator iter = nullptr;

	for(iter = m_vecViewItems.begin(); iter != m_vecViewItems.end(); ++iter)
	{
		wxWindow* pListView = *iter;

		wxCommandEvent evt(wxEVT_DRIVE_RELOAD);
		wxPostEvent(pListView, evt);
	}
}

void CContextManager::UpdateDriveSpace(const wxString& strVolume)
{
	theDriveInfo->SetDriveSpace(strVolume);

	wxVector<wxWindow *>::iterator iter = nullptr;

	for(iter = m_vecViewItems.begin(); iter != m_vecViewItems.end(); ++iter)
	{
		wxWindow* pWindow = *iter;
		if(!pWindow)
			continue;

		wxCommandEvent evt(wxEVT_DRIVE_SPACE_UPDATE);
		evt.SetString(strVolume);
		wxPostEvent(pWindow, evt);
	}
}
#endif // __WXMSW__

void CContextManager::ChangeColumn()
{
	SendViewChangeEvent(wxEVT_CHANGE_VIEWCOLUMN);
}

void CContextManager::ChangeFileList()
{
	SendViewChangeEvent(wxEVT_CHANGE_FILELIST);
}

void CContextManager::ChangeFileListSort()
{
	SendViewChangeEvent(wxEVT_CHANGE_SORTING);
}

void CContextManager::SendViewChangeEvent(wxEventType evtType)
{
	wxVector<wxWindow *>::iterator iter = nullptr;
	for(iter = m_vecViewItems.begin(); iter != m_vecViewItems.end(); ++iter)
	{
		wxWindow* pWindow = *iter;
		if(!pWindow)
			continue;

		wxCommandEvent evt(evtType);
		wxPostEvent(pWindow, evt);
	}
}

void CContextManager::GotoBookmarkPath(const wxString& strPath)
{
	CViewPanel* pActiveViewPanel = (CViewPanel *)GetActivatePanel(_gActivatedTab);
	pActiveViewPanel->GetFileListView()->LoadDirectory(strPath);
}

void CContextManager::ShowBookmarkWhenStatusbarClick()
{
	wxCommandEvent evt(wxEVT_VIEW_BOOKMARK);
	CViewPanel* pActiveViewPanel = (CViewPanel *)GetActivatePanel(_gActivatedTab);

	if(pActiveViewPanel)
		wxPostEvent(pActiveViewPanel->GetFileListView(), evt);
}
