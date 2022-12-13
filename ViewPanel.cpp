#include "ginc.h"
#include "ViewPanel.h"
#include "TabManager.h"
#include "./view/PathView.h"

#ifdef __WXMSW__
	#include "./view/local/msw/LocalFileListView.h"
#endif

wxBEGIN_EVENT_TABLE(CViewPanel, wxPanel)
	EVT_ERASE_BACKGROUND(CViewPanel::OnErase)
	EVT_MY_CUSTOM_COMMAND(wxEVT_CHANGE_DIRECTORY, wxID_ANY, CViewPanel::OnChangeDirectory)
wxEND_EVENT_TABLE()

CViewPanel::CViewPanel(wxWindow* parent, int iID, const wxString& strPath, bool bRemote)
	: wxPanel(parent, iID, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxCLIP_CHILDREN | wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE)
	, m_Mainsizer(nullptr)
	, m_bRemote(bRemote)
	, m_strPath(strPath)
	, m_pTabManager((CTabManager *)parent)
{
    CreatePanelItems(m_bRemote);
}

CViewPanel::~CViewPanel()
{
	wxDELETE(m_pPathView);
	wxDELETE(m_pFileListView);
}

void CViewPanel::CreatePanelItems(bool bRemote)
{
    m_Mainsizer = new wxBoxSizer(wxVERTICAL);

	//패스뷰 등록
	m_pPathView = new CPathView(this, wxPATHVIEW_ID++, wxSize(wxDefaultSize.GetWidth(), 20));
    //파일 리스트뷰 생성
	m_pFileListView = new CLocalFileListView(this, wxFILFELIST_VIEWID++, wxDefaultSize);

	//생성한 뷰 Sizer등록
	m_Mainsizer->Add(m_pPathView, 0, wxEXPAND, 0);
	m_Mainsizer->Add(m_pFileListView, wxSizerFlags(1).Expand().Border(wxALL, 0));

	SetSizer(m_Mainsizer);
	m_Mainsizer->SetSizeHints(this);

	_gContextManager->AddFileListViewItem(m_pFileListView);

	m_pFileListView->SetRemote(bRemote);
	m_pFileListView->LoadDirectory(m_strPath);
}

CFileListView* CViewPanel::GetFileListView()
{
	return (CFileListView *)m_pFileListView;
}

CPathView* CViewPanel::GetPathView()
{
	return m_pPathView;
}

CTabManager* CViewPanel::GetTabManager()
{
	return m_pTabManager;
}

void CViewPanel::OnErase(wxEraseEvent& event)
{

}

void CViewPanel::OnChangeDirectory(wxCommandEvent& event)
{
	wxString strPath(event.GetString());
	int iFileListToPathFlag = event.GetInt();

	if(iFileListToPathFlag == SEND_FILELIST_TO_PATH)
		m_pPathView->AddPath(strPath);
	else
		m_pFileListView->LoadDirectory(strPath);

	m_pTabManager->SetTabName(strPath);
}
