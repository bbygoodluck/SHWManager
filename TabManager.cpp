#include "ginc.h"
#include "TabManager.h"
#include "ViewPanel.h"
#include "./engine/LocalFileSystem.h"

wxBEGIN_EVENT_TABLE(CTabManager, wxAuiNotebook)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, OnPageClose)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, OnPageChanged)
wxEND_EVENT_TABLE()

CTabManager::CTabManager(wxWindow* parent, long style, bool bLTabManager)
	: wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style)
	, m_bLTabManager(bLTabManager)
{
	m_PageBMP = wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_OTHER, wxSize(16, 16));
	this->SetArtProvider(new wxAuiGenericTabArt);

	Init();
}

CTabManager::~CTabManager()
{

}

void CTabManager::OnPageClose(wxAuiNotebookEvent& event)
{
	int iPageCount = this->GetPageCount();
	if (iPageCount == 1)
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_DELETE_TAB")), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		event.Veto();
		return;
	}

	//탭 페이지 삭제시 ViewPanel의 데이터를 가지고 ViewManager에서 Item을 삭제처리 함
	int iSel = event.GetSelection();
	wxWindow* pViewPanel = this->GetPage(iSel);
	_gContextManager->DeleteFileListViewItem(pViewPanel);
}

void CTabManager::Init()
{
	if(_gActivatedTab == nullptr)
		_gActivatedTab = this;

	m_bLoaded = false;
	m_strDefaultPath = m_bLTabManager ? theJsonConfig->GetLeftPath() : theJsonConfig->GetRightPath();

	AddTab();

	m_bLoaded = true;
}

void CTabManager::AddTab(bool bRemote)
{
	if(!bRemote)
	{
		if( !CLocalFileSystem::IsDirReadable(m_strDefaultPath) )
			m_strDefaultPath = wxT("C:\\");
	}

	AddPage(new CViewPanel(this, ++wxTABWindowID, m_strDefaultPath, bRemote), wxT(""), false, m_PageBMP);
	int nPageCount = GetPageCount();
	this->SetSelection(nPageCount - 1);
}

void CTabManager::SetTabName(const wxString& strPath)
{
	wxString::size_type n = strPath.Find(SLASH);
	wxString strTarget = (n == wxString::npos ? strPath + SLASH : theUtility->GetPathName(strPath));

	int nPageCount = GetPageCount();
	int iSel = this->GetSelection();
	if(nPageCount < 0)
		iSel = 0;

	SetPageToolTip(iSel, strPath);
	this->SetPageText(iSel, strTarget);
}

void CTabManager::OnPageChanged(wxAuiNotebookEvent& event)
{
	int nPageCount = GetPageCount();
	if(nPageCount > 1)
	{
		int iSel = event.GetSelection();
		this->SetSelection(iSel);

#ifndef NDEBUG
	theUtility->LogDisplay(wxT("OnPageChanged"));
#endif // NDEBUG
	if(_gActivatedTab)
		_gContextManager->ActivateView();
	}
}
