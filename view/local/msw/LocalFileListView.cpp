#include "../../../ginc.h"
#include "LocalFileListView.h"

#include "../../../engine/msw/SHWLocalFileSystemWatcher.h"
wxBEGIN_EVENT_TABLE(CLocalFileListView, CFileListView)
	EVT_MY_CUSTOM_COMMAND(wxEVT_FS_WATCHER, wxID_ANY, CLocalFileListView::OnFileSystemWatcher)
	EVT_MY_CUSTOM_COMMAND(wxEVT_DRIVE_RELOAD, wxID_ANY, CLocalFileListView::OnDriveReload)
	EVT_MY_CUSTOM_COMMAND(wxEVT_DRIVE_SPACE_UPDATE, wxID_ANY, CLocalFileListView::OnDriveSpaceUpdate)
wxEND_EVENT_TABLE()

CLocalFileListView::CLocalFileListView(wxWindow* parent, const int nID, const wxSize& sz)
	: CFileListView(parent, nID, sz)
{
	m_pFileSystemWatcher = std::make_unique<CSHWLocalFileSystemWatcher>(this);
}

CLocalFileListView::~CLocalFileListView()
{
}

void CLocalFileListView::LoadDirectory(const wxString& strPath)
{
	wxBusyCursor wait;

	//쓰레드가 실행중이면 정지
	StopThreadFileImageListRead();

	//감시디렉토리 설정
//	m_pFileSystemWatcher->AddPath(strPath);

	Initialize();

	CLocalFileSystem localFileSys;
	if (!localFileSys.BeginFindFiles(strPath, false))
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_SEARCH_DIR_INIT_ERR")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	//이미지 리스트 쓰레드 준비
	ReadyThreadFileImageList();

    wxCommandEvent evt(wxEVT_CHANGE_DIRECTORY);
    evt.SetString(strPath);
    evt.SetInt(SEND_FILELIST_TO_PATH);

    wxPostEvent(GetViewParent(), evt);

    _HISTORY::const_iterator fIter = m_hashHistory.find(strPath);
	if(fIter != m_hashHistory.end())
	{
		_HistoryItem historyItem = fIter->second;

		m_nCurrentItemIndex = historyItem.iCurrentIndex;
		m_nStartIndex = historyItem.iStartIndex;
	}

    m_strCurrentPath = strPath;
	m_nPathDepth = theUtility->GetDepth(strPath);

	wxString strVolume = strPath.Left(1);
	if(m_strVolume.IsEmpty() || m_strVolume.CmpNoCase(strVolume) != 0)
	{
		m_strVolume = m_strCurrentPath.Left(1) != SLASH ? strVolume : m_strCurrentPath;
		SetDriveSpace(m_strVolume);
	}

	wxString strDesc(wxT(""));
	wxString strName(wxT(""));
	wxString strExt(wxT(""));
	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	while (localFileSys.GetNextFile(strName, &isDir, &lattr, &llSize, &dt))
	{
		/**************************************************************************
		 * 윈도우 11에서는 .. 의 속성이 HIDDEN을 포함
		 *    아래의 경우해당
		 *       C:\                     - 해당없음
		 *       C:\Windows              - 해당(최상위 + 1 단계의 경우만 해당함)
		 *       C:\Program Files\폴더명 - 해당없음
		 * > 상위폴더 이동표시는 모든 하위 폴더에서 표시가 되어야 하므로 강제 표시
		 **************************************************************************/
		if(strName.CmpNoCase(wxT("..")) != 0)
		{
			if (!theJsonConfig->IsViewAllFile())
			{
				if(theUtility->IsAllowAttr(lattr))
					continue;
			}
		}

		CDirData dirItem;
		dirItem.SetName(strName);

		if (isDir)
		{
			dirItem.SetType(CDirData::item_type::dir);
			m_nDirCount++;
			strDesc = theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION"));
		}
		else
		{
			dirItem.SetType(CDirData::item_type::file);

			m_nFileCount++;
			strExt = theUtility->GetExt(strName);
			strDesc = theExtInfo->GetExtInfo(strExt, m_strCurrentPath + (m_nPathDepth != 1 ? SLASH + strName : strName));

			m_dblFileSizeInDir += llSize.ToDouble();

			int iIconIndex = 0;
			int iOverlay = 0;

			if(theIconInfo->GetFileIconInfo(strExt, iIconIndex, iOverlay))
			{
				dirItem.m_iIconIndex = iIconIndex;
				dirItem.m_iOvelayIndex = iOverlay;
				dirItem.m_bIconImageSet = true;
			}
		}

		dirItem.SetAttribute(lattr);
		dirItem.SetSize(llSize);
		dirItem.SetDateTime(dt);
		dirItem.SetPath(m_strCurrentPath);
		dirItem.SetIconIndex(0);
		dirItem.SetMatch(false);
		dirItem.SetExt(strExt);
		dirItem.SetTypeName(strDesc);

		m_itemList.push_back(dirItem);

		m_strMaxName = theUtility->GetMaxData(this, m_viewFont, strName, m_strMaxName);
		m_strMaxTypeName = theUtility->GetMaxData(this, m_viewFont, strDesc, m_strMaxTypeName);

		strName = wxT("");
		strDesc = wxT("");
		strExt = wxT("");

		isDir = false;
		lattr = 0;
		llSize = 0;
		dt = 0;
	}

	localFileSys.EndFindFiles();
	/********************************************************************************
	// 2022.11.12
	// 데이터가 없을경우 SortStart() 함수 호출시 에러가 발생함.
	********************************************************************************/
	//Sorting 시작
	if(m_itemList.size() > 0)
		SortStart();

	//드라이브 추가
	AddDrive();

	m_nTotalItems = m_itemList.size();
	//쓰레드 생성이 정상적이면 이미지리스트 쓰레드 시작
	RunThreadFileImageListRead();
	//디렉토리 감시 시작
	m_pFileSystemWatcher->StartWatch(strPath);

	m_bDirLoaded = true;
	theUtility->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::AddDrive()
{
	int nDriveCount = theDriveInfo->GetDriveCount();
	for (int nIndex = 0; nIndex < nDriveCount; nIndex++)
	{
		CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(nIndex);
		if(pDriveItem == nullptr)
			continue;

		CDirData dirItem;
		wxString strDriveName = pDriveItem->GetDriveName();
		wxString strDisplyName = pDriveItem->GetDisplayName();
		wxString strDriveSpace = pDriveItem->GetDriveSpace();
		wxString strVolume     = pDriveItem->GetVolume();
		wxFSVolumeKind fsvDriveKind = pDriveItem->GetDriveKind();

		dirItem.SetVolume(strVolume);
		dirItem.SetName(strDisplyName);
		dirItem.SetDriveName(strDriveName);
		dirItem.SetSize(0);
		dirItem.SetDriveSpaceInfo(strDriveSpace);
		dirItem.SetType(CDirData::item_type::drive);
		dirItem.m_iIconIndex = pDriveItem->GetIconIndex();
		dirItem.m_iOvelayIndex = pDriveItem->GetOverlayIconIndex();
		dirItem.m_bIconImageSet = true;
		dirItem.SetDriveType(fsvDriveKind);

		m_itemList.push_back(dirItem);
	}
}

void CLocalFileListView::RemoveDrive()
{
	wxVector<CDirData>::iterator iter = m_itemList.end() - 1;
	for (iter; iter != m_itemList.begin(); --iter)
	{
		if (!iter->IsDrive())
			break;

		m_itemList.pop_back();
	}
}

void CLocalFileListView::Render(wxDC* pDC)
{
    if(m_bSizeOrColumnChanged)
        ApplyChangedViewSize();

	// 컬럼계산
	CalcColumn(pDC);
	// 컬럼그리기
	DrawColumn(pDC);
	//정보표시창
	DrawInfoArea(pDC);
	//디렉토리 정보표시
	DisplayDirInfo(pDC);
	//데이터 표시
	DisplayItems(pDC);

    m_bSizeOrColumnChanged = false;
    m_bDirLoaded = false;
}

void CLocalFileListView::ApplyChangedViewSize()
{
    m_viewDirInfo.SetLeft(m_viewRect.GetLeft());
	m_viewDirInfo.SetTop(m_viewRect.GetTop() - 1);
	m_viewDirInfo.SetRight(m_viewRect.GetRight());
	m_viewDirInfo.SetBottom(m_viewRect.GetTop() + 20);

	m_viewRectDetail.SetLeft(m_viewRect.GetLeft());
	m_viewRectDetail.SetTop(m_viewRect.GetBottom() - 21);
	m_viewRectDetail.SetRight(m_viewRect.GetRight());
	m_viewRectDetail.SetBottom(m_viewRect.GetBottom());

	m_viewRectDisp.SetLeft(m_viewRect.GetLeft());
	m_viewRectDisp.SetTop(m_viewRect.GetTop() + 20);
	m_viewRectDisp.SetRight(m_viewRect.GetRight());
	m_viewRectDisp.SetBottom(m_viewRect.GetBottom() - 20);
}

void CLocalFileListView::OnFileSystemWatcher(wxCommandEvent& event)
{
	CWatcherItem* pItem = (CWatcherItem *)event.GetClientData();

	switch(pItem->m_Action)
	{
		case FS_WATCHER_CREATE:
			DoCreate(pItem);
			break;

		case FS_WATCHER_MODIFY:
			DoModify(pItem);
			break;

		case FS_WATCHER_DELETE:
			DoDelete(pItem);
			break;

		case FS_WATCHER_RENAME:
			DoRename(pItem);
			break;

		default:
			return;
	}

	theUtility->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::DoCreate(CWatcherItem* pItem)
{
	wxString strName(pItem->m_strNew);
	wxString strFullPath = theUtility->MakeFullPathName(m_strCurrentPath, strName);

	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);
	wxString strExt(wxT(""));
	wxString strDesc(wxT(""));

	FILE_TYPE ftype = FTYPE_UNKNOWN;
	ftype = CLocalFileSystem::GetFileType(strFullPath);

	if(ftype != FTYPE_UNKNOWN)
	{
		CDirData dirItem;
		dirItem.SetName(strName);
		dirItem.SetType(CDirData::item_type::file);

		if(!CLocalFileSystem::GetAttribute(strFullPath, &isDir, &lattr, &llSize, &dt))
			return;

		if (!theJsonConfig->IsViewAllFile())
		{
			if(theUtility->IsAllowAttr(lattr))
				return;
		}

		strDesc = wxT("");
		strExt = wxT("");
		if (isDir)
		{
			m_nDirCount++;
			dirItem.SetType(CDirData::item_type::dir);
			strDesc = theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION"));
		}
		else
		{
			m_nFileCount++;
			dirItem.SetType(CDirData::item_type::file);

			strExt = theUtility->GetExt(strName);
			strDesc = theExtInfo->GetExtInfo(strExt, strFullPath);

			m_dblFileSizeInDir += llSize.ToDouble();
		}
		dirItem.SetAttribute(lattr);
		dirItem.SetSize(llSize);

		dirItem.SetDateTime(dt);
		dirItem.SetPath(m_strCurrentPath);
		dirItem.SetExt(strExt);
		dirItem.SetTypeName(strDesc);

		m_itemList.push_back(dirItem);
		m_nTotalItems = wx_static_cast(int, m_itemList.size());

		if(ftype != FTYPE_UNKNOWN)
		{
			m_strMaxName = theUtility->GetMaxData(this, m_viewFont, strName, m_strMaxName);
			m_strMaxTypeName = theUtility->GetMaxData(this, m_viewFont, strDesc, m_strMaxTypeName);

			SetFileImage(m_nTotalItems - 1);
		}

		SortStart();
		m_bSizeOrColumnChanged = true;
	}
}

void CLocalFileListView::DoModify(CWatcherItem* pItem)
{
	wxString strName(pItem->m_strNew);
	wxString strFullPath = theUtility->MakeFullPathName(m_strCurrentPath, strName);

	FILE_TYPE ftype = FTYPE_UNKNOWN;
	ftype = CLocalFileSystem::GetFileType(strFullPath);

	if(ftype != FTYPE_UNKNOWN)
	{
		//아이템이 존재하는지 체크
		bool bExist = false;
		wxVector<CDirData>::iterator iter = GetItemExist(strName, bExist);
		if(iter == m_itemList.end())
			return;

		bool isDir = false;
		unsigned long lattr = 0;
		wxLongLong llSize(0);
		wxDateTime dt(0.0);

		if(!CLocalFileSystem::GetAttribute(strFullPath, &isDir, &lattr, &llSize, &dt))
			return;

		if (!theJsonConfig->IsViewAllFile())
		{
			if(theUtility->IsAllowAttr(lattr))
				return;
		}

		CDirData* pData = iter;

		if(pData->IsFile())
		{
			if(pData->GetSize().ToDouble() != 0)
				m_dblFileSizeInDir -= pData->GetSize().ToDouble();

			m_dblFileSizeInDir += llSize.ToDouble();
		}

		pData->SetAttribute(lattr);
		pData->SetSize(llSize);
		pData->SetDateTime(dt);

		UpdateLastAccessTime(wxDateTime::Now());

		int nSortType = theJsonConfig->GetSortType();

		if(nSortType == CFileListView::VIEW_SORT::VIEW_SORT_SIZE)
			SortStart();

		//용량 업데이트
		_gContextManager->UpdateDriveSpace(m_strVolume);
	}
}

void CLocalFileListView::DoDelete(CWatcherItem* pItem)
{
	wxString strName(pItem->m_strNew);

	bool bExist = false;

	wxVector<CDirData>::iterator iter = GetItemExist(strName, bExist);
	if(iter == m_itemList.end())
		return;

	wxString strDesc = iter->GetTypeName();

	m_strMaxName = theUtility->GetMaxData(this, m_viewFont, strName, m_strMaxName);
	m_strMaxTypeName = theUtility->GetMaxData(this, m_viewFont, strDesc, m_strMaxTypeName);

	if (iter->IsDir())
		m_nDirCount--;
	else
	{
		m_dblFileSizeInDir -= iter->GetSize().ToDouble();
		m_nFileCount--;
	}

	m_itemList.erase(iter);

	m_nTotalItems = wx_static_cast(int, m_itemList.size());
	if(m_nCurrentItemIndex > m_nTotalItems)
	{
		m_nStartIndex = 0;
		m_nCurrentItemIndex = 0;
	}

	m_dispNameInfoMap.clear();
	UpdateLastAccessTime(wxDateTime::Now());

	m_bSizeOrColumnChanged = true;

#ifdef __WXMSW__
	//용량 업데이트
	_gContextManager->UpdateDriveSpace(m_strVolume);
#endif // __WXMSW__
}

void CLocalFileListView::DoRename(CWatcherItem* pItem)
{
	wxString strNewName(pItem->m_strNew);
	wxString strOldName(pItem->m_strOld);

	wxString strFullPath = theUtility->MakeFullPathName(m_strCurrentPath, strNewName);

	bool bExist = true;
	wxVector<CDirData>::iterator iter = GetItemExist(strOldName, bExist);
	if(bExist)
	{
		CWatcherItem* pWatcherItem = new CWatcherItem();
		pWatcherItem->m_strNew = pItem->m_strOld;

		DoDelete(pWatcherItem);
		wxDELETE(pWatcherItem);

		DoCreate(pItem);
	}

//	SortStart();
	UpdateLastAccessTime(wxDateTime::Now());
	m_bSizeOrColumnChanged = true;
}

void CLocalFileListView::OnDriveReload(wxCommandEvent& event)
{
	RemoveDrive();
	AddDrive();

	m_nTotalItems = m_itemList.size();

	ReCalcPage();

	m_bSizeOrColumnChanged = true;
	theUtility->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::SetDriveSpace(const wxString& strVolume)
{
	CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(strVolume);

	wxFSVolumeKind fsk = pDriveItem->GetDriveKind();
	if ( (fsk != wxFS_VOL_CDROM) &&
		 (fsk != wxFS_VOL_DVDROM))
	{
		wxString strTotalSpace(wxT(""));
		wxString strFreeSpace(wxT(""));

		pDriveItem->GetDriveSpace(m_dblTotalSpace, m_dblFreeSpace);

		double dblFreePrecent = m_dblFreeSpace * 100.0 / m_dblTotalSpace;

		strTotalSpace = wxString::Format(wxT("%04.2f GB"), (m_dblTotalSpace / GIGABYTE));
		strFreeSpace = wxString::Format(wxT("%04.2f GB"), (m_dblFreeSpace / GIGABYTE));

		m_strDriveSpace = pDriveItem->GetDriveName();
		m_strDriveSpace += wxT(" ");
		m_strDriveSpace += strFreeSpace;
		m_strDriveSpace += wxString::Format(wxT("(%.2f%) "), dblFreePrecent);
		m_strDriveSpace += theMsgManager->GetMessage(wxT("MSG_DISK_FREE"));
		m_strDriveSpace += wxT(" (");
		m_strDriveSpace += theMsgManager->GetMessage(wxT("MSG_DISK_TOTAL"));
		m_strDriveSpace += wxT(" ");
		m_strDriveSpace += strTotalSpace;
		m_strDriveSpace += wxT(")");
	}
	else
		m_strDriveSpace = wxT("");
}

void CLocalFileListView::OnDriveSpaceUpdate(wxCommandEvent& event)
{
	wxString strUpdatedVolume(event.GetString());
	if(theUtility->Compare(m_strVolume, strUpdatedVolume) == 0)
		SetDriveSpace(strUpdatedVolume);

	wxVector<CDirData>::iterator iter = m_itemList.begin() + m_nCurrentItemIndex;
	wxString strVolume = iter->GetVolume();
	if(iter->IsDrive() && (theUtility->Compare(strUpdatedVolume, strVolume) == 0))
	{
		CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(strVolume);
		iter->SetDriveSpaceInfo(pDriveItem->GetDriveSpace());

		theUtility->RefreshWindow(this, m_viewRect);
	}
}

