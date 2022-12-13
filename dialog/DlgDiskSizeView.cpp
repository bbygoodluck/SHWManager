///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "../ginc.h"
#include "DlgDiskSizeView.h"
#include "../img/drvlist.xpm"

#include "../view/VolumeSizeView.h"
#include "../interface/DirInfoThread.h"
#include "DlgSearchDir.h"
///////////////////////////////////////////////////////////////////////////
DlgDiskSizeView::DlgDiskSizeView( wxWindow* parent, const wxSize& sz)
	: wxDialog( parent, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DISK_SIZE_VIEW")), wxDefaultPosition, sz, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	SetIcon(wxIcon("wxwin"));
	CenterOnScreen();

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_treeCtrlDriveList = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_FULL_ROW_HIGHLIGHT|wxTR_ROW_LINES|wxTR_SINGLE|wxTR_TWIST_BUTTONS );
	bSizer2->Add( m_treeCtrlDriveList, 1, wxEXPAND, 5 );


	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_pVolumeSizeView = new CVolumeSizeView(this, wxSize(-1, 23));
	bSizer3->Add( m_pVolumeSizeView, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5 );

	m_treeLstSize = new wxTreeListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_DEFAULT_STYLE | wxTL_SINGLE);

	InitImageList();
	Init();

	bSizer3->Add( m_treeLstSize, 1, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT, 5 );
/*
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_staticReadDirectory = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticReadDirectory->Wrap( -1 );

	bSizer6->Add( m_staticReadDirectory, 0, wxALL|wxEXPAND|wxFIXED_MINSIZE, 5 );
	bSizer3->Add( bSizer6, 0, wxEXPAND|wxFIXED_MINSIZE, 5 );
*/
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

//	m_btnCancel = new wxButton( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_CANCEL")), wxDefaultPosition, wxDefaultSize, 0 );

//	m_btnCancel->SetBitmap( wxArtProvider::GetBitmap( wxART_CLOSE, wxART_OTHER ) );
//	bSizer17->Add( m_btnCancel, 0, 0, 5 );

	m_btnClose = new wxButton( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_CLOSE")), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnClose->SetBitmap( wxArtProvider::GetBitmap( wxART_QUIT, wxART_BUTTON ) );

	bSizer17->Add( m_btnClose, 0, wxBOTTOM|wxRIGHT, 5 );
	bSizer3->Add( bSizer17, 0, wxALIGN_RIGHT|wxALL, 5 );
	bSizer1->Add( bSizer3, 3, wxEXPAND, 5 );
	bSizer15->Add( bSizer1, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer15 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DlgDiskSizeView::OnClose ) );
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgDiskSizeView::OnInitDialog ) );
	m_treeCtrlDriveList->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( DlgDiskSizeView::OnTreeSelChanged ), NULL, this );
	m_btnClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDiskSizeView::OnBtnClose ), NULL, this );

	m_treeLstSize->Connect( wxEVT_TREELIST_ITEM_EXPANDED, wxTreeListEventHandler( DlgDiskSizeView::OnTreeListItemExpanded ), NULL, this );
}

DlgDiskSizeView::~DlgDiskSizeView()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DlgDiskSizeView::OnClose ) );
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgDiskSizeView::OnInitDialog ) );
	m_treeCtrlDriveList->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( DlgDiskSizeView::OnTreeSelChanged ), NULL, this );
	m_btnClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDiskSizeView::OnBtnClose ), NULL, this );

	m_treeLstSize->Disconnect( wxEVT_TREELIST_ITEM_EXPANDED, wxTreeListEventHandler( DlgDiskSizeView::OnTreeListItemExpanded ), NULL, this );

	wxDELETE(m_imageList);
}

void DlgDiskSizeView::OnInitDialog( wxInitDialogEvent& event )
{
#ifdef __WXMSW__
	m_DiskSizeViewFont.Create(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Segoe UI"));
#else
	m_DiskSizeViewFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
#endif

	m_treeCtrlDriveList->SetFont(m_DiskSizeViewFont);
	CreateTreeImages(16);
	wxTreeItemId rootId = m_treeCtrlDriveList->AddRoot(theMsgManager->GetMessage(wxT("MSG_DISK_LIST")));
	m_treeCtrlDriveList->SetItemImage(rootId, TreeCtrlIcon_Root);

	m_rootItem = rootId;
#ifdef __WXMSW__
	int iDriveCount = theDriveInfo->GetDriveCount();
	for(int i = 0; i < iDriveCount; i++)
	{
		CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(i);
		if(pDriveItem == nullptr)
			continue;

		wxString strDisplyName = pDriveItem->GetDisplayName();
		m_treeCtrlDriveList->AppendItem(rootId, strDisplyName, TreeCtrlIcon_Item + i, -1, new CVolumeInfo(pDriveItem->GetVolume()));
	}
#endif // __WXMSW__
	m_treeCtrlDriveList->ExpandAll();
}

void DlgDiskSizeView::InitImageList()
{
	wxSize iconSize = wxArtProvider::GetSizeHint(wxART_LIST, this);
    if ( iconSize == wxDefaultSize )
        iconSize = FromDIP(wxSize(16, 16));

    m_imageList = new wxImageList(iconSize.x, iconSize.y);

    // The order should be the same as for the enum elements.
    static const wxString icons[] =
    {
        wxART_NORMAL_FILE,
        wxART_FOLDER,
        wxART_FOLDER_OPEN
    };

    for ( unsigned n = 0; n < WXSIZEOF(icons); n++ )
        m_imageList->Add(wxArtProvider::GetIcon(icons[n], wxART_LIST, iconSize));

#ifdef __WXMSW__
	int iDriveCount = theDriveInfo->GetDriveCount();
	for(int i = 0; i < iDriveCount; i++)
	{
		CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(i);
		if(pDriveItem == nullptr)
			continue;

		HICON hIcon = ImageList_GetIcon(_gImageList, pDriveItem->GetIconIndex(), ILD_NORMAL | ILC_MASK);
		wxIcon icon;
		icon.SetHandle((WXHANDLE)hIcon);

		wxBitmap bmp(icon);
		bmp.SetMask(new wxMask(bmp, *wxLIGHT_GREY));

		m_imageList->Add(bmp.ConvertToImage().Rescale(iconSize.x, iconSize.y));
	}
#endif // __WXMSW__
}

void DlgDiskSizeView::Init()
{
	m_treeLstSize->SetImageList(m_imageList);

	m_treeLstSize->AppendColumn(wxT("이름"),
                       m_treeLstSize->WidthFor("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                    "),
                       wxALIGN_LEFT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);

    m_treeLstSize->AppendColumn(wxT("파일"),
                       m_treeLstSize->WidthFor("1,000,000"),
                       wxALIGN_RIGHT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);

    m_treeLstSize->AppendColumn(wxT("폴더"),
                       m_treeLstSize->WidthFor("1,000,000 KiB"),
                       wxALIGN_RIGHT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);

	m_treeLstSize->AppendColumn(wxT("파일 용량"),
                       m_treeLstSize->WidthFor("1,000,000,000,000,000,000 GB"),
                       wxALIGN_RIGHT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);

	m_treeLstSize->SetItemComparator(&m_comparator);
}

void DlgDiskSizeView::CreateTreeImages(int size)
{
	wxImageList* images = new wxImageList(size, size, true);
	wxBitmap bmpDriveList = wxBitmap(drvlist);
	bmpDriveList.SetMask(new wxMask(bmpDriveList, *wxLIGHT_GREY));
	images->Add(bmpDriveList);

#ifdef __WXMSW__
	int iDriveCount = theDriveInfo->GetDriveCount();
	for(int i = 0; i < iDriveCount; i++)
	{
		CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(i);
		if(pDriveItem == nullptr)
			continue;

		HICON hIcon = ImageList_GetIcon(_gImageList, pDriveItem->GetIconIndex(), ILD_NORMAL | ILC_MASK);
		wxIcon icon;
		icon.SetHandle((WXHANDLE)hIcon);

		wxBitmap bmp(icon);
		bmp.SetMask(new wxMask(bmp, *wxLIGHT_GREY));

		images->Add(bmp.ConvertToImage().Rescale(size, size));
	}
#endif // __WXMSW__

	m_treeCtrlDriveList->AssignImageList(images);
}

void DlgDiskSizeView::OnTreeSelChanged( wxTreeEvent& event )
{
	wxTreeItemId treeItemId = event.GetItem();
	if(m_rootItem == treeItemId)
		return;

	CVolumeInfo* pVolume = (CVolumeInfo *)m_treeCtrlDriveList->GetItemData(treeItemId);

	m_pVolumeSizeView->SetVolumeInfo(pVolume->_strVolume);
	m_pVolumeSizeView->RefreshVolumeInfo();

	ReadDriveInfo(pVolume);
}

void DlgDiskSizeView::ReadDriveInfo(CVolumeInfo* pVolumeInfo)
{
	m_treeLstSize->DeleteAllItems();

	wxString strVolume(pVolumeInfo->_strVolume);
	double dblTotalSpace = 0.0f;
	double dblFreeSpace  = 0.0f;

	CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(strVolume);
	pDriveItem->GetDriveSpace(dblTotalSpace, dblFreeSpace);

	double dblUsed = dblTotalSpace - dblFreeSpace;

	wxString strUsed = wxString::Format(wxT("%5.1f GB"), (dblUsed / GIGABYTE)) + theMsgManager->GetMessage(wxT("MSG_DISK_USED"));
	wxString strDisp = strUsed + wxT("        ") + (strVolume.Left(1).CmpNoCase(SLASH) == 0 ? strVolume : strVolume + wxT(":") + SLASH);

	_root = m_treeLstSize->GetRootItem();
	wxTreeListItem item = m_treeLstSize->AppendItem(_root, strDisp, Icon_FolderClosed, Icon_FolderOpened);

	DlgSearchDir dlg(_gMainFrame);
	m_pThread = new CDirInfoThread(&dlg);
	m_pThread->SetViewInfo(this, item, strVolume);

	if (m_pThread->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError(wxT("Can't create thread!"));
		wxDELETE(m_pThread);
	}

	dlg.ShowModal();
	dlg.Destroy();

	m_pThread->Delete();
	wxDELETE(m_pThread);
}

void DlgDiskSizeView::OnTreeListItemExpanded( wxTreeListEvent& event )
{
	wxTreeListItem item = event.GetItem();
	ChageFileSize(&item);
}

void DlgDiskSizeView::ChageFileSize(wxTreeListItem* pTreeListItem)
{
	wxTreeListItem child = m_treeLstSize->GetFirstChild(*pTreeListItem);
	wxTreeListItem sibiling = child;

	double dblSize = 0.0f;

	wxString strDispFileSize(wxT(""));
	wxString strDispFileSizeType(wxT(""));
	wxColor col;

	while(true)
	{
		if(!child.IsOk())
			break;

		dblSize = 0.0f;

		wxString strFileSize = m_treeLstSize->GetItemText(child, 3);
		if(strFileSize.Right(1).CmpNoCase(wxT("B")) != 0)
		{
			strFileSize.ToDouble(&dblSize);

			if(dblSize != 0.0)
			{
				theUtility->GetSizeInfo(dblSize, strDispFileSize, strDispFileSizeType, col);
				strDispFileSize += strDispFileSizeType;

				m_treeLstSize->SetItemText(child, 3, strDispFileSize);
			}
		}

		sibiling = m_treeLstSize->GetNextSibling(child);
		child = sibiling;
	}
}

void DlgDiskSizeView::OnClose( wxCloseEvent& event )
{
	ProcessClose();
}

void DlgDiskSizeView::OnBtnClose( wxCommandEvent& event )
{

	ProcessClose();
}

void DlgDiskSizeView::ProcessClose()
{
	EndDialog(wxID_CLOSE);
}
