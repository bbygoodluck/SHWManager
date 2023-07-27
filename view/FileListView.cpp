#include "../ginc.h"
#include "FileListView.h"
#include "SHWTooltipView.h"

#ifdef __WXMSW__
	#include "../interface/msw/MSWContextMenuHandler.h"
#else
#endif


wxBEGIN_EVENT_TABLE(CFileListView, wxWindow)
	EVT_CHAR_HOOK(CFileListView::OnCharHook)
    EVT_CHAR(CFileListView::OnChar)
    EVT_SET_FOCUS(CFileListView::OnSetFocus)
	EVT_KILL_FOCUS(CFileListView::OnKillFocus)
    EVT_PAINT(CFileListView::OnPaint)
	EVT_ERASE_BACKGROUND(CFileListView::OnErase)
	EVT_SIZE(CFileListView::OnSize)
	EVT_LEFT_DOWN(CFileListView::OnMouseLBottonDown)
	EVT_LEFT_DCLICK(CFileListView::OnMouseLButtonDBClick)
	EVT_RIGHT_DOWN(CFileListView::OnMouseRButtonDown)
	EVT_RIGHT_UP(CFileListView::OnMouseRButtonUp)
	EVT_MOTION(CFileListView::OnMouseMove)
	EVT_MOUSEWHEEL(CFileListView::OnMouseWheel)
	EVT_MENU_RANGE(COMPRESS_START_ID, COMPRESS_END_ID, CFileListView::OnCompressMenuExecute)

	//Custom Event
	EVT_MY_CUSTOM_COMMAND(wxEVT_ITEM_RENAME, wxID_ANY, CFileListView::OnItemRename)
	EVT_MENU_RANGE(EXTERNAL_PROGRAM_START_ID, EXTERNAL_PROGRAM_END_ID, CFileListView::OnMenuFileEditProcess)
	EVT_MY_CUSTOM_COMMAND(wxEVT_FILE_EDIT, wxID_ANY, CFileListView::OnMenuFileEdit)
	EVT_MY_CUSTOM_COMMAND(wxEVT_VIEW_CONTEXTMENU, wxID_ANY, CFileListView::OnViewContextMenu)
	EVT_MY_CUSTOM_COMMAND(wxEVT_CHANGE_VIEWCOLUMN, wxID_ANY, CFileListView::OnChangeViewColumn)
	EVT_MY_CUSTOM_COMMAND(wxEVT_CHANGE_FILELIST, wxID_ANY, CFileListView::OnChangeFileList)
	EVT_MY_CUSTOM_COMMAND(wxEVT_CHANGE_SORTING, wxID_ANY, CFileListView::OnChangeSorting)
	EVT_MY_CUSTOM_COMMAND(wxEVT_VIEW_BOOKMARK, wxID_ANY, CFileListView::OnShowBookmark)
	EVT_MY_CUSTOM_COMMAND(wxEVT_EXEC_MENU_CLICK, wxID_ANY, CFileListView::OnExecMenuClick)
	EVT_MY_CUSTOM_COMMAND(wxEVT_SHOW_COMPRESS_MENU, wxID_ANY, CFileListView::OnShowCompressMenu)

	//2023.05.26 Add Start *********************************************************
	EVT_MY_CUSTOM_COMMAND(wxEVT_VIEW_DIR_NUM, wxID_ANY, CFileListView::OnViewDirNumbering)
	//2023.05.26 Add End   *********************************************************
wxEND_EVENT_TABLE()

CFileListView::CFileListView(wxWindow* parent, const int nID, const wxSize& sz)
    : wxWindow(parent, nID, wxDefaultPosition, sz, FILELISTVIEW_STYLE)
    , m_pParent(parent)
{
    //상위폴더 이동 이미지
	m_icoUpDir = wxArtProvider::GetIcon(wxART_GO_DIR_UP, wxART_OTHER, wxSize(16, 16));

	m_colDefault = theJsonConfig->GetDefaultColor();
	m_colDrive   = theJsonConfig->GetDriveColor();
	m_colDir     = theJsonConfig->GetDirColor();
	m_colSize    = theJsonConfig->GetDefaultColor();
	m_colTime    = theJsonConfig->GetDefaultColor();
	m_colAttr    = theJsonConfig->GetAttributeColor();
	m_colType    = theJsonConfig->GetDescriptionColor();
	m_colMatch   = theJsonConfig->GetMatchColor();

	m_iFontSize = theJsonConfig->GetFontSize();
	m_strFontName = theJsonConfig->GetFontName();

	m_viewFont = *_gViewFont;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    m_pImageList = std::make_unique<CImageList>();
    m_pImageList->Attach(_gImageList);

    m_pSelectedItemView = new CSHWTooltipView(this);
	m_pSelectedItemView->SetSize(60, 20);
	m_pSelectedItemView->Show(false);

	m_pRenameTooltip = new CSHWTooltipView(this);
	m_pRenameTooltip->SetSize(60, 20);
	m_pRenameTooltip->Show(false);

	m_pKeyInputToolTip = new CSHWTooltipView(this);
	m_pKeyInputToolTip->SetSize(60, 20);
	m_pKeyInputToolTip->Show(false);

	//이름변경 컨트롤
	m_pTxtCtrlForRename = std::make_unique<wxTextCtrl>(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_THEME);
	m_pTxtCtrlForRename->SetBackgroundColour(wxColour(220, 220, 220));
	m_pTxtCtrlForRename->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_pTxtCtrlForRename->SetFont(m_viewFont);
	m_pTxtCtrlForRename->Show(false);

	m_pTxtCtrlForRename->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CFileListView::OnKeyDownTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CFileListView::OnEnterTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(CFileListView::OnKillFocusTxtCtrl), NULL, this);
}

CFileListView::~CFileListView()
{
	m_bImageListReadStarted = false;
	StopThreadFileImageListRead();

	wxDELETE(m_pSelectedItemView);
	wxDELETE(m_pRenameTooltip);
	wxDELETE(m_pKeyInputToolTip);

	m_pTxtCtrlForRename->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(CFileListView::OnKeyDownTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CFileListView::OnEnterTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(CFileListView::OnKillFocusTxtCtrl), NULL, this);
}

void CFileListView::Clear()
{
	m_itemList.clear();
	m_dispNameInfoMap.clear();
	m_hashSelectedItem.clear();
}

void CFileListView::Initialize()
{
	Clear();

	//아이템 전체수
	m_nTotalItems = 0;
	//화면표시 시작 인덱스
	m_nStartIndex = 0;
	// 디렉토리수
	m_nDirCount   = 0;
	// 파일수
	m_nFileCount  = 0;
	//파일 사이즈 초기화
	m_dblFileSizeInDir = 0.0;
	//현재 인덱스
	m_nCurrentItemIndex = 0;
//	//페이지당 표시 아이템 수
//	m_nDisplayItemInView = 0;
	//현재 디렉토리
	m_strCurrentPath = wxT("");
	//Max Name
	m_strMaxName     = wxT("");
	//Max Type Name
	m_strMaxTypeName = wxT("");
	//키 입력값
	m_strKeyInput    = wxT("");

	//디렉토리 Load 플래그
	m_bDirLoaded = false;
	//화면 변경 플래그
	m_bSizeOrColumnChanged = false;
	//2023.05.26 Add Start *********************************************************
//	if(m_bDirectoryNumbering)
//		_gContextManager->ViewDirectoryNumbering();
	m_bDirectoryNumbering = false;
	//2023.05.26 Add End   *********************************************************
	m_pSelectedItemView->SetTooltipText(wxT(""));
	m_pSelectedItemView->Show(false);

	m_pRenameTooltip->SetTooltipText(wxT(""));
	m_pRenameTooltip->Show(false);

	m_pKeyInputToolTip->SetTooltipText(wxT(""));
	m_pKeyInputToolTip->Show(false);
}

void CFileListView::OnSetFocus(wxFocusEvent& event)
{
	m_bSetFocus = true;
	theUtility->RefreshWindow(this, m_viewRect);
}

void CFileListView::OnKillFocus(wxFocusEvent& event)
{
	m_bSetFocus = false;
	theUtility->RefreshWindow(this, m_viewRect);
}

void CFileListView::OnErase(wxEraseEvent& event)
{

}

void CFileListView::OnCharHook(wxKeyEvent& event)
{
	bool bControlDown = wxIsCtrlDown();
	bool bAltDown     = wxIsAltDown();
	bool bShiftDown   = wxIsShiftDown();

	if(bShiftDown)
	{
		if(bAltDown || bControlDown)
		{
			event.Skip();
			return;
		}

		if (theJsonConfig->IsShift_IME_KOR_MoveDrive())
			theUtility->SetImeModeToEnglish(this);
	}

	event.Skip();
}

void CFileListView::OnChar(wxKeyEvent& event)
{
	int nKeyCode = event.GetKeyCode();

#ifndef NDEBUG
	theUtility->LogDisplay("OnChar");
#endif

	std::unordered_map<int, int>::const_iterator cIterFind = _gmapAllowProcessKey.find(nKeyCode);
	if(cIterFind != _gmapAllowProcessKey.end())
	{
		if(ProcessKeyEvent(nKeyCode))
			return;
	}

	PreTranslateKeyEvent(event);
	event.Skip();
}

void CFileListView::PreTranslateKeyEvent(wxKeyEvent& event)
{
	//EventType
//	wxEventType evtType = event.GetEventType();
	int nKeyCode = event.GetKeyCode();
	if(nKeyCode == WXK_ESCAPE)
	{
		SelectedItemsClear();
		InitKeyInputTooltip();

		//2023.05.26 Add Start **************************************************************
		if(m_bDirectoryNumbering)
		{
			wxCommandEvent evt(wxEVT_VIEW_DIR_NUM);
			wxPostEvent(this, evt);
		}
		//2023.05.26 Add End   **************************************************************
		return;
	}

	bool bControlDown = wxIsCtrlDown();
	bool bAltDown     = wxIsAltDown();
	bool bShiftDown   = wxIsShiftDown();

	wxString strKeyName(theUtility->GetKeyName(event));
	if(strKeyName.CmpNoCase(wxT("UnKnown")) == 0)
		return;

	if (bShiftDown)
	{
		if(bControlDown)
			return;

		if(m_strKeyInput.IsEmpty())
		{
			wxString strDriveName = strKeyName.MakeUpper() + wxT(":") + SLASH;
			if (theDriveInfo->IsDriveExist(strKeyName))
			{
				if (!CLocalFileSystem::IsDirReadable(strDriveName))
					return;

				LoadDirectory(strDriveName);
				return;
			}
		}
	}

	if(bAltDown)
	{
		return;
	}

	if(bControlDown)
	{
		return;
	}

	int nLenKeyInput = 0;
	bool bRefresh = false;
	if(nKeyCode == WXK_BACK)
	{
		if(!m_strKeyInput.IsEmpty())
		{
			nLenKeyInput = m_strKeyInput.Len();
			m_strKeyInput = m_strKeyInput.Left(nLenKeyInput - 1);
		}
	}
	else
	{
		if(m_bDirectoryNumbering && !isdigit(nKeyCode))
			return;

		m_strKeyInput += strKeyName;
		nLenKeyInput = m_strKeyInput.Len();
	}

	if(nLenKeyInput > 0)
	{
		if(!m_strKeyInput.IsEmpty())
		{
			DoMatchClear();

			wxClientDC dc(this);

			wxSize sztip = dc.GetTextExtent(m_strKeyInput);
			wxSize szTooltip;

			wxRect rcWindow =this->GetClientRect();
			wxPoint ptTooltip(rcWindow.GetRight() / 2 - sztip.GetWidth(), rcWindow.GetHeight() / 2);

			szTooltip.SetWidth(sztip.GetWidth() + 10);
			szTooltip.SetHeight(sztip.GetHeight() + 5);

			m_pKeyInputToolTip->SetTooltipText(m_strKeyInput);
			m_pKeyInputToolTip->SetThemeEnabled(true);
			m_pKeyInputToolTip->SetPosition(ptTooltip);
			m_pKeyInputToolTip->SetSize(szTooltip);
			m_pKeyInputToolTip->Show(true);

			if(!m_bDirectoryNumbering)
				FindMatchItems();

			bRefresh = true;
		}
		else
		{
			InitKeyInputTooltip();
			bRefresh = true;
		}
	}
	else
		InitKeyInputTooltip();


	if(bRefresh)
		theUtility->RefreshWindow(this, m_viewRect);
}

bool CFileListView::ProcessKeyEvent(int nKeyCode)
{
	switch (nKeyCode)
	{
		case WXK_F4:
			ShowBookmark();
			break;

		case WXK_REVERSE_SLASH:
			GotoRoot();
			break;

		case WXK_TAB:
			if(m_bDirectoryNumbering)
				m_bDirectoryNumbering = false;

			_gContextManager->ChangeTab();
			break;

		case WXK_SPACE:
			if(!SetSelectedItem())
				return true;

			m_nCurrentItemIndex++;
			MoveRightAndDownKey();
			break;

		case wxKEY_DOT:
		case WXK_BACK:
		{
			if(!m_strKeyInput.IsEmpty())
				return false;

			//최상위 Path
			if(m_nPathDepth == 1)
				return true;
			//상위 폴더 이동을 위해서 강제로 ..을 선택하도록 한다.
			m_nCurrentItemIndex = 0;
			if (!ProcessEnterKey(nKeyCode))
				return true;
		}
			break;

		case WXK_RETURN: //Enter key
			//2023.05.28 Add Start ******************************************************************
			if(m_bDirectoryNumbering && !m_strKeyInput.IsEmpty())
			{
				int nDirNum = 0;
				m_strKeyInput.ToInt(&nDirNum);

				if(nDirNum <= 0)
					return false;

				m_nCurrentItemIndex = nDirNum - 1;
			}
			//2023.05.28 Add End   ******************************************************************
			ProcessEnterKey(nKeyCode);
			break;

		case WXK_LEFT:
			if (m_nDispColumn > 1)
			{
				m_nCurrentItemIndex -= m_nItemCountInColumn;
				MoveLeftAndUpKey();
			}

			break;

		case WXK_RIGHT:
			if (m_nDispColumn > 1)
			{
				m_nCurrentItemIndex += m_nItemCountInColumn;
				MoveRightAndDownKey();
			}

			break;

		case WXK_UP:
			if (wxGetKeyState(WXK_SHIFT))
			{
				if (!SetSelectedItem())
					return true;
			}

			m_nCurrentItemIndex--;
			MoveLeftAndUpKey();

			break;

		case WXK_DOWN:
			if (wxGetKeyState(WXK_SHIFT))
			{
				if (!SetSelectedItem())
					return true;
			}

			m_nCurrentItemIndex++;
			MoveRightAndDownKey();

			break;

		case WXK_PAGEDOWN:
			m_nCurrentItemIndex += m_nDisplayItemInView;

			if ((m_nStartIndex + (m_nItemCountInColumn * m_nDispColumn)) < m_nTotalItems)
				m_nStartIndex += (m_nItemCountInColumn * m_nDispColumn);

			if (m_nCurrentItemIndex >= (m_nTotalItems - 1))
				m_nCurrentItemIndex = m_nTotalItems - 1;

			break;

		case WXK_PAGEUP:
			m_nCurrentItemIndex -= (m_nItemCountInColumn * m_nDispColumn);
			m_nStartIndex -= (m_nItemCountInColumn * m_nDispColumn);

			if (m_nStartIndex <= 0)
				m_nStartIndex = 0;

			if (m_nCurrentItemIndex <= 0)
				m_nCurrentItemIndex = 0;

			break;

		case WXK_HOME:
			m_nCurrentItemIndex = 0;
			m_nStartIndex = 0;

			break;

		case WXK_END:
			if (m_nTotalItems > m_nDisplayItemInView)
			{
				int iMod = m_nTotalItems % m_nDisplayItemInView;
				m_nStartIndex = m_nTotalItems - iMod;

				if (m_nStartIndex <= 0)
					m_nStartIndex = 0;
			}

			m_nCurrentItemIndex = m_nTotalItems - 1;

			break;
		case WXK_WINDOWS_KEY:
			return true;
			break;

		default:
			return false;
	}

	theUtility->RefreshWindow(this, m_viewRect);
	return true;
}

void CFileListView::MoveLeftAndUpKey()
{
	if (!((m_nCurrentItemIndex >= m_nStartIndex) && (m_nCurrentItemIndex <= (m_nStartIndex + m_nDisplayItemInView))))
		m_nStartIndex -= m_nItemCountInColumn;

	if (m_nCurrentItemIndex <= 0)
	{
		m_nCurrentItemIndex = 0;
		m_nStartIndex = 0;
	}
}

void CFileListView::MoveRightAndDownKey()
{
	if ((m_nStartIndex + m_nDisplayItemInView) <= (m_nTotalItems - 1))
	{
		if (m_nCurrentItemIndex > ((m_nStartIndex + m_nDisplayItemInView) - 1))
			m_nStartIndex += m_nItemCountInColumn;
	}

	if (m_nCurrentItemIndex >= (m_nTotalItems - 1))
		m_nCurrentItemIndex = m_nTotalItems - 1;
}

void CFileListView::GotoRoot()
{
	wxString strPath(m_strVolume);
#ifdef __WXMSW__
	if(!m_bRemote)
	{
		strPath += wxT(":");
		strPath += SLASH;
	}

	if(!CLocalFileSystem::IsDirReadable(strPath))
	{
		wxString strMsg;
		strMsg = strPath + theMsgManager->GetMessage(wxT("MSG_IS")) + wxT(" ") + theMsgManager->GetMessage(wxT("MSG_DIREDTORY_READ_FAIL"));
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return;
	}
	//루트로 이동할 경우 기존 히스토리는 모두 삭제한다.
	wxArrayString arrKey;
	for(auto const& [key, value] : m_hashHistory)
	{
		wxString strTmpPath = key;
		if(theUtility->Compare(strTmpPath, strPath) != 0)
			arrKey.Add(key);
	}

	for(size_t nIndex = 0; nIndex < arrKey.size(); nIndex++)
	{
		wxString strKey = arrKey.Item(nIndex);
		m_hashHistory.erase(strKey);
	}

	if (strPath.CmpNoCase(m_strCurrentPath) == 0)
		return;

	LoadDirectory(strPath);
#else

#endif // __WXMSW__
}

bool CFileListView::ProcessEnterKey(int nKeyCode)
{
	CDirData dirData = m_itemList.at(m_nCurrentItemIndex);
	wxString strName(dirData.GetName());
	wxString strSearchDir(wxT(""));

	bool bReadDirectory = true;
	if (dirData.IsDir())
	{
		if( (strName.CmpNoCase(wxT("..")) == 0) || (nKeyCode == wxKEY_DOT) )
		{
			strSearchDir = theUtility->GetParentPath(dirData.GetPath());
			if (strSearchDir.IsEmpty())
				return false;

			//상위 폴더로의 이동시 현재 디렉토리의 히스토리는 삭제한다.
			_HISTORY::iterator fIter = m_hashHistory.find(m_strCurrentPath);
			_HistoryItem historyItem;
			if(fIter != m_hashHistory.end())
				m_hashHistory.erase(m_strCurrentPath);
		}
		else
		{
			strSearchDir = m_strCurrentPath + (m_nPathDepth == 1 ? strName : SLASH + strName);
			//접근여부 체크
			if (!CLocalFileSystem::IsDirReadable(strSearchDir))
				return false;

			//디렉토리 방문 히스토리 저장
			_HISTORY::iterator fIter = m_hashHistory.find(m_strCurrentPath);
			_HistoryItem historyItem;
			if(fIter != m_hashHistory.end())
				historyItem = fIter->second;

			historyItem.iCurrentIndex = m_nCurrentItemIndex;
			historyItem.iStartIndex = m_nStartIndex;

			m_hashHistory[m_strCurrentPath] = historyItem;
		}
	}
	else if(dirData.IsFile())
	{
		theUtility->LaunchAndExec(dirData.GetFullPath(), dirData.GetPath());//m_strCurrentPath);
		bReadDirectory = false;
	}
	else
	{
		strSearchDir = dirData.GetDriveName();

		if(strSearchDir.Left(1) != SLASH)
		{
			//접근여부 체크
			if (!CLocalFileSystem::IsDirReadable(strSearchDir))
				return false;
		}
		else
		{
			//네티웍 드라이브 처리(예정)
			wxMessageBox(wxT("Network drive - Comming soon"), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
			return false;
		}
	}

	if(bReadDirectory)
		LoadDirectory(strSearchDir);

	return true;
}

bool CFileListView::SetSelectedItem()
{
	int iTotalItem = m_itemList.size();
	if(m_nCurrentItemIndex >= iTotalItem - 1)
		return false;

	wxVector<CDirData>::iterator fIter = m_itemList.begin() + m_nCurrentItemIndex;
	if( (theUtility->Compare(fIter->GetName(), wxT("..")) != 0) && !fIter->IsDrive() )
	{
		//선택이 되어 있는 상태면 선택이 안된상태로 변경
		bool bNewSelected = fIter->IsItemSelected() ? false : true;
		fIter->SetItemSelected(bNewSelected);

		if (bNewSelected) //아이템 선택
		{
			SELITEM_INFO _Info;

			_Info.m_iSelIndex = m_nCurrentItemIndex;
			_Info.m_bFile = fIter->IsFile();
			_Info.m_strFullPath = fIter->GetFullPath();

			std::unordered_map<int, SELITEM_INFO>::value_type valsel(m_nCurrentItemIndex, _Info);
			m_hashSelectedItem.insert(valsel);

			if(fIter->IsFile())
				m_nSelFileCnt++;
			else
				m_nSelDirCnt++;
		}
		else
		{
			std::unordered_map<int, SELITEM_INFO>::const_iterator findKey = m_hashSelectedItem.find(m_nCurrentItemIndex);
			if (findKey != m_hashSelectedItem.end())
			{
				SELITEM_INFO _Info = findKey->second;
				if(_Info.m_bFile)
					m_nSelFileCnt--;
				else
					m_nSelDirCnt--;

				m_hashSelectedItem.erase(m_nCurrentItemIndex);
			}
		}

		if(m_hashSelectedItem.size() > 0)
		{
			wxString strSelString(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_SELINFO")));
			wxString strSelItems = strSelString + wxString::Format(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_ITEM_SELECT")), m_nSelDirCnt, m_nSelFileCnt);

			DisplaySelectedItemInfo(strSelItems, 0, 0);
		}
		else
		{
			m_pSelectedItemView->Show(false);
			m_nSelDirCnt = 0;
			m_nSelFileCnt = 0;
		}
	}

	return true;
}

void CFileListView::DisplaySelectedItemInfo(const wxString& strMsg, int xPos, int yPos)
{
	m_pSelectedItemView->Show(false);

	wxClientDC dc(this);
	wxSize sztip = dc.GetTextExtent(strMsg);
	wxSize szTooltip;

	szTooltip.SetWidth(sztip.GetWidth() + 10);
	szTooltip.SetHeight(sztip.GetHeight() + 5);

	xPos = m_viewRectDisp.GetRight() - szTooltip.GetWidth();
	yPos = m_viewRectDisp.GetBottom() - szTooltip.GetHeight();

	m_pSelectedItemView->SetTooltipText(strMsg);
	m_pSelectedItemView->SetThemeEnabled(true);
	m_pSelectedItemView->SetPosition(wxPoint(xPos, yPos));
	m_pSelectedItemView->SetSize(szTooltip);
	m_pSelectedItemView->Show(true);
}

void CFileListView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

    m_viewRect = GetClientRect();
	wxMemoryDC* pMemDC = m_memDC.CreateMemoryDC(&dc, m_viewRect);

    pMemDC->SetFont(m_viewFont);
	Render(pMemDC);

	dc.Blit(0, 0, m_viewRect.GetWidth(), m_viewRect.GetHeight(), pMemDC, 0, 0);
}

void CFileListView::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();
	m_memDC.ChangeViewSize(size);

	m_bSizeOrColumnChanged = true;
    theUtility->RefreshWindow(this, m_viewRect);
}

void CFileListView::SortStart()
{
	int iSortType = theJsonConfig->GetSortType();
	wxVector<CDirData>::const_iterator iter = m_itemList.begin();
	wxString strName = iter->GetName();
	int iSortStartIndex = 0;

	if(strName.Cmp(wxT("..")) == 0)
		iSortStartIndex = 1;

	switch(iSortType)
	{
		case VIEW_SORT_DEFAULT:
			std::sort(m_itemList.begin() + iSortStartIndex, m_itemList.end()/* - iSortEndIndex */, CSorting::DirSortOfName);
			break;

		case VIEW_SORT_TIME:
			std::sort(m_itemList.begin() + iSortStartIndex, m_itemList.end()/* - iSortEndIndex */, CSorting::DirSortOfTime);
			break;

		case VIEW_SORT_SIZE:
			std::sort(m_itemList.begin() + iSortStartIndex, m_itemList.end()/* - iSortEndIndex */, CSorting::DirSortOfSize);
			break;

		default:
			break;
	}
}

void CFileListView::DrawInfoArea(wxDC* pDC)
{
	wxPen pen(wxColour(0, 0, 0));
	wxBrush brush(wxColour(240, 240, 240));

	pDC->SetPen(pen);
	pDC->SetBrush(brush);

	pDC->DrawRectangle(m_viewDirInfo);
	pDC->DrawRectangle(m_viewRectDetail);
}

void CFileListView::ReCalcPage()
{
	int nStartIndex = 0;
	while (1)
	{
		if ((m_nCurrentItemIndex >= nStartIndex) && (m_nCurrentItemIndex <= (nStartIndex + m_nDisplayItemInView)))
			break;

		nStartIndex += m_nDisplayItemInView;
		if (nStartIndex > m_nTotalItems)
		{
			nStartIndex = m_nTotalItems - m_nDisplayItemInView;
			break;
		}
	}

	m_nStartIndex = nStartIndex;
	if (m_nStartIndex < 0)
		m_nStartIndex = 0;

	if (m_nCurrentItemIndex < 0)
		m_nCurrentItemIndex = 0;
}

void CFileListView::CalcColumn(wxDC* pDC)
{
	if (m_nTotalItems <= 0)
		return;

	if(!m_bSizeOrColumnChanged && !m_bDirLoaded)
		return;

	bool bPosReset = false;
	m_nDispColumn = theJsonConfig->GetColumnCount();
	COLUMN_VIEW_OPTION enumColumnView = _gColumnViewOPEnum[m_nDispColumn];
	if(m_enumColumnViewOption != enumColumnView)
	{
		m_enumColumnViewOption = enumColumnView;
		bPosReset = true;
	}

	if(m_bSizeOrColumnChanged)
		bPosReset = true;

	m_iCharHeight = pDC->GetCharHeight() + 2;

	m_nItemCountInColumn = (m_viewRectDisp.GetHeight() / m_iCharHeight);
	if (m_nItemCountInColumn == 0)
		return;

	if(m_enumColumnViewOption == VIEW_COLUMN_AUTO)
	{
		bPosReset = true;
		//자동컬럼인경우
		if (!CalcAutoColumn(pDC, m_viewRectDisp))
			return;
	}

	//화면안에 표시될 아이템 수
	m_nDisplayItemInView = m_nItemCountInColumn * m_nDispColumn;
	int nWidth = m_viewRectDisp.GetWidth();
	int nBottom = m_viewRectDisp.GetBottom();
	if(bPosReset)
	{
		m_ptList.clear();

		int nDispWidth = static_cast<int>(nWidth / m_nDispColumn);
		if (m_nDispColumn > 1)
		{
			//컬럼구분선 좌표
			wxPoint pt1(0, 0);
			wxPoint pt2(0, 0);
			//컬럼 좌표 설정
			for (int nColumnCount = 0; nColumnCount < (m_nDispColumn - 1); nColumnCount++)
			{
				CColumnPoint colPoint;
				colPoint.pt1.x = nDispWidth * (nColumnCount + 1);
				colPoint.pt1.y = 0;

				colPoint.pt2.x = nDispWidth * (nColumnCount + 1);
				colPoint.pt2.y = nBottom;

				m_ptList.push_back(colPoint);
			}
		}

		m_nDispColumnEndPoint = nDispWidth;
	}

	//아이템 표시좌표 계산
	CalcPosition(pDC);
}

bool CFileListView::CalcAutoColumn(wxDC* pDC, const wxRect& viewRect)
{
	//전체건수 / 컬럼당 아이템수 <= 0 이면 컬럼은 1
	m_nDispColumn = m_nTotalItems / m_nItemCountInColumn;
	if (m_nDispColumn <= 0)
		m_nDispColumn = 1;
	else
	{	//컬럼당 표시아이템수 > 전체 아이템수 ==> 컬럼은 1
		if (m_nItemCountInColumn > m_nTotalItems)
			m_nDispColumn = 1;
		else
		{	//표시가능 컬럼수
			int iAvalibleDispItem = 0;

			wxSize szEng = pDC->GetTextExtent(wxString("A"));
			wxSize szHan = pDC->GetTextExtent(wxString("가"));
			//한글과 영문의 평균 Width
			int iAvgCharWidth = (szEng.GetWidth() + szHan.GetWidth()) / 2;
			//최대 표시가능 Width
			int iMaxDispWidth = viewRect.GetWidth();
			//자동컬럼시 표시글자수의 최대값은 16
			int iMaxDispCharWidth = iAvgCharWidth * 16;
			int iDispColumn = 0;

			while (1)
			{	//전체 표시가능 Width > 최대 표시가능 Width
				if (iMaxDispWidth > iMaxDispCharWidth)
				{	//표시컬럼 증가
					iDispColumn++;
					//표시가능 아이템수 증가
					iAvalibleDispItem += m_nItemCountInColumn;
					//전체 표시가능 Width -= 최대표시가능Width
					iMaxDispWidth -= iMaxDispCharWidth;

					if (iAvalibleDispItem >= m_nTotalItems)
						break;
				}
				else
					break;
			}

			//자동 컬럼 선택시 최대 9개의 컬럼
			m_nDispColumn = iDispColumn >= 9 ? 9 : iDispColumn;
		}
	}

	if (m_nDispColumn <= 0)
		m_nDispColumn = 1;

	return true;
}

void CFileListView::CalcPosition(wxDC* pDC)
{
	size_t iposItemCnt = m_posList.size();
	if(iposItemCnt == 0 || iposItemCnt > 200)
		m_posList.reserve(200);

	m_posList.clear();

	m_bDispFlag[0] = false;
	m_bDispFlag[1] = false;
	m_bDispFlag[2] = false;
	m_bDispFlag[3] = false;
	m_bDispFlag[4] = false;

	wxString strAttr(wxT("RHSA"));
	wxString strTime(wxT("0000-00-00 00:00"));
	wxString strSize(wxT("123.45 MB"));
	wxString strFileSize(wxT("123.45 "));
	wxString strFileSizeType(wxT("MB"));

	wxSize sizeMaxName = pDC->GetTextExtent(m_strMaxName);
	wxSize sizeMaxTypeName = pDC->GetTextExtent(m_strMaxTypeName);
	wxSize sizeAttr = pDC->GetTextExtent(strAttr);
	wxSize sizeTime = pDC->GetTextExtent(strTime);
	wxSize sizeSize = pDC->GetTextExtent(strSize);
	wxSize sizeFileSize = pDC->GetTextExtent(strFileSize);
	wxSize szFileSizeType = pDC->GetTextExtent(strFileSizeType);

	int iNameWidth = sizeMaxName.GetWidth();
	int iSizeWidth = sizeSize.GetWidth();
	int iTimeWidth = sizeTime.GetWidth();
	int iAttrWidth = sizeAttr.GetWidth();
	int iTypeWidth = sizeMaxTypeName.GetWidth();

	unsigned int arrWidth[5] = { 0, };
	arrWidth[0] = iNameWidth;
	arrWidth[1] = iSizeWidth;
	arrWidth[2] = iTimeWidth;
	arrWidth[3] = iAttrWidth;
	arrWidth[4] = iTypeWidth;

	int iSizeSum = 0;
	int iAvaliableWidth = m_nDispColumnEndPoint - (ICON_SELECTED_WIDTH_GAP + (GAP_WIDTH * 21));

	for (int i = 0; i < 5; i++)
	{
		iSizeSum += arrWidth[i];
		if (iAvaliableWidth > iSizeSum)
			m_bDispFlag[i] = true;
	}

	m_bDispFlag[0] = true;
	if (!m_bDispFlag[1])
		iAvaliableWidth = m_nDispColumnEndPoint - (ICON_SELECTED_WIDTH_GAP + (GAP_WIDTH * 12));

	int iTempColumn = 0;
	int iDispIndex = 0;
	int iStartX = 1;
	int iStartY = m_viewRectDisp.GetTop();

	int iDisp_x1 = 0;
	int iDisp_y1 = 0;
	int iDisp_x2 = 0;
	int iDisp_y2 = 0;

	//2019.04.17 ADD
	int iGapWidth = GAP_WIDTH * 2;

	for (int nIndex = 0; nIndex < m_nDisplayItemInView; nIndex++)
	{
		if (nIndex > (m_nItemCountInColumn - 1))
		{
			if ((nIndex % m_nItemCountInColumn) == 0)
			{
				iTempColumn++;
				iDispIndex = 0;
			}
		}

		int x1 = iStartX + (iTempColumn * m_nDispColumnEndPoint);
		int y1 = (iStartY + 1) + (m_iCharHeight * iDispIndex);
		int x2 = m_nDispColumnEndPoint - 1;
		int y2 = m_iCharHeight;

		wxRect rect(x1, y1, x2, y2);

		CPositionInfo posInfo;
		posInfo.m_mainRect = rect;

		int iIcon_x1 = x1 + ICON_SELECTED_WIDTH_GAP + GAP_WIDTH;
		int iIcon_y1 = y1 + 2;
		int iIcon_x2 = ICON_WIDTH;
		int iIcon_y2 = y2;

		wxRect rcIcon(iIcon_x1, iIcon_y1, iIcon_x2, iIcon_y2);
		posInfo.m_iconRect = rcIcon;

		if (m_bDispFlag[0])
		{
			iDisp_x1 = rcIcon.GetRight() + (GAP_WIDTH * 3);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = m_bDispFlag[1] == true ? iNameWidth : iAvaliableWidth;
			iDisp_y2 = y2;

			wxRect rcName(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_nameRect = rcName;
		}

		//사이즈 표시
		if (m_bDispFlag[1])
		{
			iDisp_x1 = posInfo.m_nameRect.GetRight() + iGapWidth;
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iSizeWidth;
			iDisp_y2 = y2;

			wxRect rcSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_sizeRect = rcSize;

			int iFileSizeWidth = sizeFileSize.GetWidth();

			iDisp_x1 = posInfo.m_sizeRect.GetLeft();
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iFileSizeWidth;
			iDisp_y2 = y2;

			wxRect rcFileSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_sizeRectFile = rcFileSize;

			int iSizeTypeWidth = szFileSizeType.GetWidth();

			iDisp_x1 = rcFileSize.GetRight() + iGapWidth;
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iSizeTypeWidth;
			iDisp_y2 = y2;

			wxRect rcFileSizeType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_sizeRectType = rcFileSizeType;
		}

		//수정시간
		if (m_bDispFlag[2])
		{
			iDisp_x1 = posInfo.m_sizeRect.GetRight() + (iGapWidth * 2);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iTimeWidth;
			iDisp_y2 = y2;

			wxRect rcTime(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_timeRect = rcTime;
		}

		//속성
		if (m_bDispFlag[3])
		{
			iDisp_x1 = posInfo.m_timeRect.GetRight() + (iGapWidth * 2);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iAttrWidth;
			iDisp_y2 = y2;

			wxRect rcAttr(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_attrRect = rcAttr;
		}

		//설명
		if (m_bDispFlag[4])
		{
			iDisp_x1 = posInfo.m_attrRect.GetRight() + (iGapWidth * 2);
			iDisp_y1 = y1;
			iDisp_x2 = iTypeWidth;
			iDisp_y2 = y2;

			wxRect rcType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_typeNameRect = rcType;
		}

		m_posList.push_back(posInfo);
		iDispIndex++;
	}

	m_iTotalPositionCnt = m_posList.size();
}

void CFileListView::DrawColumn(wxDC* pDC)
{
	pDC->SetPen(wxPen(theJsonConfig->GetColumnColor()));

	for (auto &colItem : m_ptList)
		pDC->DrawLine(colItem.pt1, colItem.pt2);
}

void CFileListView::DisplayItems(wxDC* pDC)
{
	if (m_nTotalItems <= 0)
		return;

	int nDisplayItemCount = m_nStartIndex + m_nDisplayItemInView;
	if (nDisplayItemCount >= m_nTotalItems)
		nDisplayItemCount = m_nTotalItems;

	if (nDisplayItemCount <= 0)
		return;

	int nPosIndex = 0;
	wxColour dispColor;

	wxString strName(wxT(""));
	wxString strSrcName(wxT(""));
	wxString strFullPathName(wxT(""));
	wxString strSpace(wxT(""));

	for (int nIndex = m_nStartIndex; nIndex < nDisplayItemCount; nIndex++)
	{
		if (nIndex >= m_nTotalItems)
			nIndex = m_nTotalItems - 1;

		if (nPosIndex >= m_iTotalPositionCnt)
			nPosIndex = m_iTotalPositionCnt - 1;

		CPositionInfo posInfo = m_posList.at(nPosIndex);

		wxVector<CDirData>::iterator iter = m_itemList.begin() + nIndex;
		strName = iter->GetName();

		bool isDrive = iter->IsDrive() ? true : false;
		bool isDir = iter->IsDir() ? true : false;
		bool isFile = iter->IsFile() ? true : false;
		bool isCut = iter->IsCut() ? true : false;

		bool bSelected = iter->IsItemSelected();
		bool bMatched = iter->IsMatch();

		//표시 색상
		if (isDrive)
		{
			if(iter->GetDriveType() == wxFS_VOL_DISK)
				strName = strName + wxT("   ") + iter->GetDriveSpaceInfo();

			dispColor = m_colDrive;
		}
		else if (isDir)  dispColor = m_colDir;
		else
		{
			wxString strExt(theUtility->GetExt(strName).Upper());
			dispColor = theJsonConfig->GetExtColor(strExt);
		}

		if (bMatched)
			dispColor = m_colMatch;

		if (bSelected)
			dispColor = wxColour(255, 255, 80);

		if (m_nCurrentItemIndex == nIndex)
		{
			DisplayDetailInfo(pDC, m_nCurrentItemIndex);

			wxRect rcFillRect(posInfo.m_mainRect);
			rcFillRect.SetBottom(rcFillRect.GetBottom() + 1);

			wxPen   pen;
			wxBrush brush;

			//포커스가 없는경우
			if (!m_bSetFocus)
			{
				pen = wxPen(wxColour(30, 30, 30), 1);
				brush = wxColour(30, 30, 30);
			}
			else
			{
				pen = dispColor;
				brush = dispColor;
			}

			pDC->SetPen(pen);
			pDC->SetBrush(brush);

			pDC->DrawRectangle(rcFillRect);
			//포커스가 없는경우
			if (!m_bSetFocus)
				dispColor = wxColour(90, 90, 90);
			else
				dispColor = wxColour(0, 0, 0);
		}
		else
		{
			if (bSelected || bMatched)
			{
				wxRect rcFillRect(posInfo.m_mainRect);
				rcFillRect.SetBottom(rcFillRect.GetBottom() + 1);

				wxPen   pen(wxColour(30, 30, 30));
				wxBrush brush(wxColour(19, 102, 142));

				pDC->SetPen(pen);
				pDC->SetBrush(brush);

				pDC->DrawRectangle(rcFillRect);

				pDC->SetPen(wxNullPen);
				pDC->SetBrush(wxNullBrush);
			}
		}

		wxColour colSelected = (m_nCurrentItemIndex == nIndex) ? dispColor : m_colDefault;
		wxString strDisp = CalcStringEllipse(pDC, strName, isDrive);

		pDC->SetTextForeground(dispColor);
		pDC->DrawLabel(strDisp, posInfo.m_nameRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

		if(!isDrive)
		{
			if (m_bDispFlag[1])
			{
				if (isDir)
					pDC->DrawLabel(theMsgManager->GetMessage(wxT("MSG_DIR_FILESIZE_POS")), posInfo.m_sizeRect, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				else
				{
					wxString strFileSize(iter->GetSizeString());
					wxString strFileSizeType(iter->GetSizeTypeString());
					wxColour colorType(iter->GetSizeColor());
					wxColour colDispColor;

					if (iter->GetAttribute() & ATTR_RDONLY)
					{
						colDispColor = *wxYELLOW;
						colorType = *wxYELLOW;
					}
					else
						colDispColor = m_colDefault;

					pDC->SetTextForeground(m_nCurrentItemIndex == nIndex ? dispColor : colDispColor);

					if (strFileSizeType.Cmp(_T("")) == 0)	//사이즈가 KB이하인경우
						pDC->DrawLabel(strFileSize, posInfo.m_sizeRect, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
					else
					{	//사이즈가 KB이상인경우
						pDC->DrawLabel(strFileSize, posInfo.m_sizeRectFile, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
						pDC->SetTextForeground(m_nCurrentItemIndex == nIndex ? dispColor : colorType);
						pDC->DrawLabel(strFileSizeType, posInfo.m_sizeRectType, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
					}
				}
			}

			//시간정보
			if (m_bDispFlag[2])
			{
				wxString strTime = iter->GetDateTimeToString();
				pDC->SetTextForeground(colSelected);
				pDC->DrawLabel(strTime, posInfo.m_timeRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}
			//속성정보
			if (m_bDispFlag[3])
			{
				wxString strAttr = iter->GetAttributeToString();
				pDC->SetTextForeground(colSelected);
				pDC->DrawLabel(strAttr, posInfo.m_attrRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}

			//type정보
			if (m_bDispFlag[4])
			{
				wxString strDesc = iter->GetTypeName();
				pDC->SetTextForeground(m_nCurrentItemIndex == nIndex ? dispColor : m_colType);
				pDC->DrawLabel(strDesc, posInfo.m_typeNameRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}
		}

		if (bSelected)
		{	//아이템이 선택되었을때 선택표시를 Polygon으로 처리(▶)
			wxPoint ptSel[3];

			ptSel[0].x = 0;
			ptSel[0].y = 0;

			ptSel[1].x = 0;
			ptSel[1].y = 0;

			ptSel[2].x = 0;
			ptSel[2].y = 0;

			//Polygon 보정
			wxRect rcMain(posInfo.m_mainRect);

			ptSel[0].x = rcMain.GetLeft() + 3;
			ptSel[0].y = rcMain.GetTop() + 3;

			ptSel[1].x = rcMain.GetLeft() + 9;
			ptSel[1].y = rcMain.GetTop() + 9;

			ptSel[2].x = rcMain.GetLeft() + 3;
			ptSel[2].y = rcMain.GetBottom() - 2;

			wxPen penSel(wxColour(225, 113, 0));
			wxBrush brushSel(wxColour(225, 113, 0));

			pDC->SetPen(penSel);
			pDC->SetBrush(brushSel);

			pDC->DrawPolygon(3, ptSel, -1);

			pDC->SetPen(wxNullPen);
			pDC->SetBrush(wxNullBrush);
		}

		if (theUtility->Compare(iter->GetName(), wxT("..")) == 0)
			pDC->DrawIcon(m_icoUpDir, wxPoint(posInfo.m_iconRect.GetLeft(), posInfo.m_iconRect.GetTop()));
		else
		{
			int iImageFlag = 0;
#ifdef __WXMSW__
			iImageFlag = ILD_NORMAL | ILC_MASK;

			if (isDir || isFile)
			{
				if (isCut)
					iImageFlag |= ILD_BLEND25;

				if (iter->GetAttribute() & ATTR_HIDDEN)
					iImageFlag |= ILD_BLEND25;
			}
#else
#endif
			int iImageX = posInfo.m_iconRect.GetLeft();
			int iImageY = posInfo.m_iconRect.GetTop();

			DrawItemImage(pDC, iImageX, iImageY, iImageFlag, iter->m_iIconIndex, iter->m_iOvelayIndex);
		}

		//2023.05.26 Add Start **************************************************************
		if(m_bDirectoryNumbering && isDir)
		{
			wxString strDirNum = wxString::Format("%d", nIndex + 1);
			int nDirNumX1 = 0;
			int nDirNumY1 = 0;
			int nDirNumX2 = 0;
			int nDirNumY2 = 0;

			nDirNumX1 = posInfo.m_nameRect.GetRight() - (10 * strDirNum.Len());
			nDirNumY1 = posInfo.m_nameRect.GetTop();
			nDirNumX2 = posInfo.m_nameRect.GetRight();
			nDirNumY2 = posInfo.m_nameRect.GetBottom();

			wxRect rcDirNum(wxPoint(nDirNumX1, nDirNumY1), wxPoint(nDirNumX2, nDirNumY2));
			pDC->SetTextForeground(wxColour(255, 255, 0));
			pDC->DrawLabel(strDirNum, rcDirNum, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
		//2023.05.26 Add End   **************************************************************

		nPosIndex++;
	}
}

wxString CFileListView::CalcStringEllipse(wxDC* pDC, const wxString& strSrc, bool IsDrive)
{
	wxString strKey(strSrc);

#ifdef __WXMSW__
		strKey.MakeLower();
#endif

	CPositionInfo posInfo = m_posList.at(0);

	wxString strName(strSrc);
	wxString strDisp(wxT(""));

	wxSize szNameSize = pDC->GetTextExtent(strName);

	int nDispWidth = IsDrive ? posInfo.m_mainRect.GetWidth() - 35 : posInfo.m_nameRect.GetWidth();
	int nNameWidth = szNameSize.GetWidth();

	int nLen = strName.Len();

	if (nNameWidth > nDispWidth)
	{
		std::unordered_map<wxString, wxString>::const_iterator fIter = m_dispNameInfoMap.find(strKey);
		if (fIter != m_dispNameInfoMap.end())
			return fIter->second;

		for (int nIndex = 0; nIndex < nLen; nIndex++)
		{
			strDisp = strName.Left(nIndex + 1);
			wxSize sizeText = pDC->GetTextExtent(wxString(strDisp + wxT("...")));

			if ((sizeText.GetWidth()) > nDispWidth)
			{
				strDisp = strName.Left(nIndex);
				break;
			}
		}

		strDisp += wxT("...");
		m_dispNameInfoMap.insert(std::make_pair(strKey, strDisp));
	}
	else
		strDisp = strName;

	return strDisp;
}

void CFileListView::DisplayDirInfo(wxDC* pDC)
{
	wxFont fontNormal;
	wxFont fontBold;

	fontNormal.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Segoe UI"));
	fontBold.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, _T("Segoe UI"));

	wxString strDirCount = wxString::Format(wxT("%d"), m_nDirCount);
	wxString strFileCount = wxString::Format(wxT("%d"), m_nFileCount);
	wxString strDirName(wxT(" ") + theMsgManager->GetMessage(wxT("MSG_DIR")) + wxT(", "));
	wxString strFileName(wxT(" ") + theMsgManager->GetMessage(wxT("MSG_FILE_DESCRIPTION")) + wxT(" "));

	int xPos = m_viewDirInfo.GetLeft() + 5;
	int yPos = m_viewDirInfo.GetTop() + 3;

	pDC->SetFont(fontBold);
	pDC->SetTextForeground(wxColour(0, 0, 255));
	pDC->DrawText(strDirCount, xPos, yPos);

	wxSize sizeInfo = pDC->GetTextExtent(strDirCount);
	xPos += sizeInfo.GetWidth();

	pDC->SetFont(fontNormal);
	pDC->SetTextForeground(wxColour(0, 0, 0));
	pDC->DrawText(strDirName, xPos , yPos);

	sizeInfo = pDC->GetTextExtent(strDirName);
	xPos += sizeInfo.GetWidth();

	pDC->SetFont(fontBold);
	pDC->SetTextForeground(wxColour(0, 0, 255));
	pDC->DrawText(strFileCount, xPos, yPos);

	sizeInfo =pDC->GetTextExtent(strFileCount);
	xPos += sizeInfo.GetWidth();

	pDC->SetFont(fontNormal);
	pDC->SetTextForeground(wxColour(0, 0, 0));
	pDC->DrawText(strFileName, xPos, yPos);

	sizeInfo = pDC->GetTextExtent(strFileName);
	xPos += sizeInfo.GetWidth();

	wxString strFileSize(wxT(""));
	wxString strFileSizeType(wxT(""));

	wxColour colorType(wxColour(192, 192, 192));

	theUtility->GetSizeInfo(m_dblFileSizeInDir, strFileSize, strFileSizeType, colorType);
	if (strFileSizeType.IsEmpty())
		strFileSizeType = wxT("Bytes");

	wxString strSizeInfo = wxString::Format(wxT("(%s %s)"), strFileSize, strFileSizeType);
	pDC->SetFont(fontBold);
	pDC->DrawText(strSizeInfo, xPos, yPos);

	pDC->SetFont(fontNormal);
	sizeInfo = pDC->GetTextExtent(m_strDriveSpace);

	xPos = m_viewDirInfo.GetRight() - sizeInfo.GetWidth();
	pDC->DrawText(m_strDriveSpace, xPos - 5, yPos);

	wxRect rcDiskSpace;
	rcDiskSpace.SetLeft(xPos - 150);
	rcDiskSpace.SetTop(m_viewDirInfo.GetTop() + 2);
	rcDiskSpace.SetRight(xPos - 10);
	rcDiskSpace.SetBottom(m_viewDirInfo.GetBottom() - 2);

	wxPen penDisk(wxColour(100, 100, 100));
	wxBrush brushDisk(wxColour(120, 120, 120));

	pDC->SetPen(penDisk);
	pDC->SetBrush(brushDisk);
	pDC->DrawRectangle(rcDiskSpace);

	double dblTotalWidth = rcDiskSpace.GetWidth() * 1.0;
	double dblUsed = m_dblTotalSpace - m_dblFreeSpace;

	int iUsedWidth = (int)((dblUsed * dblTotalWidth) / m_dblTotalSpace);

	wxPen penUsed(wxColour(100, 100, 100));
	wxBrush brushUsed(wxColour(44, 184, 65));
	wxRect rcDiskSpaceUsed(rcDiskSpace.GetLeft() + 1, rcDiskSpace.GetTop() + 1, iUsedWidth, rcDiskSpace.GetHeight() - 1);

	int iRed = 44 + (211 * iUsedWidth / dblTotalWidth);
	int iGreen = 150 - (105 * iUsedWidth / dblTotalWidth);
	int iBlue = 150 -  (105 * iUsedWidth / dblTotalWidth);

	iRed = iRed > 255 ? 255 : iRed;
	iGreen = iGreen < 0 ? 0 : iGreen;
	iBlue = iBlue < 0 ? 0 : iBlue;

	pDC->GradientFillLinear(rcDiskSpaceUsed, wxColour(44, 184, 65), wxColour(iRed , iGreen , iBlue));

	wxPen penLine(wxColour(255, 255, 255));
	pDC->SetPen(penLine);

	pDC->DrawLine(rcDiskSpace.GetLeft(), rcDiskSpace.GetBottom(), rcDiskSpace.GetRight() + 1, rcDiskSpace.GetBottom());
	pDC->DrawLine(rcDiskSpace.GetRight(), rcDiskSpace.GetTop(), rcDiskSpace.GetRight(), rcDiskSpace.GetBottom());

	wxString strUsed = wxString::Format(wxT("%5.1f GB"), (dblUsed / GIGABYTE));
	double dblPercent = (dblUsed * 100.0f / m_dblTotalSpace);

	pDC->SetTextForeground(wxColour(255, 255, 255));
	pDC->DrawLabel(strUsed, rcDiskSpace, wxALIGN_CENTER);

	wxPen penDetail(wxColour(0, 0, 0));
	wxBrush brushDetail(wxColour(192, 192, 192));

	pDC->SetPen(penDetail);
	pDC->SetBrush(brushDetail);

	pDC->DrawRectangle(m_viewRectDetail);

	pDC->SetPen(wxNullPen);
	pDC->SetBrush(wxNullBrush);
	pDC->SetFont(m_viewFont);
}

void CFileListView::DisplayDetailInfo(wxDC* pDC, int nIndex)
{
	wxVector<CDirData>::const_iterator iter = m_itemList.begin() + nIndex;

	int nImageFlag = 0;
#ifdef __WXMSW__
		nImageFlag = ILD_NORMAL | ILC_MASK;
#endif // __WXMSW__;

	DrawItemImage(pDC, m_viewRectDetail.GetLeft() + 3, m_viewRectDetail.GetTop() + 3, nImageFlag, iter->m_iIconIndex, iter->m_iOvelayIndex);

	wxString strDetailInfo = iter->GetName();
	strDetailInfo.append(wxT("  |  "));
#ifdef __WXMSW__
	if(iter->IsDrive())
	{
		wxString strVolume = iter->GetVolume();
		CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(strVolume);
		if(pDriveItem)
			strDetailInfo += pDriveItem->GetDriveSpace();
	}
	else
	{
#endif
		if(iter->IsFile())
		{
			strDetailInfo += theUtility->SetComma(iter->GetSize().ToString());
			strDetailInfo += wxT(" Bytes");
			strDetailInfo.append(wxT("  |  "));
		}

		wxString strAttr = iter->GetAttributeToString();

		strDetailInfo.append(strAttr);
		strDetailInfo.append(wxT("  |  "));


		wxString strTime = iter->GetDateTimeToString();
		strDetailInfo.append(strTime);
		strDetailInfo.append(wxT("  |  "));
		strDetailInfo.append(iter->GetTypeName());
#ifdef __WXMSW__
	}
#endif

	wxFont font;
	font.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Segoe UI"));

	pDC->SetFont(font);
	pDC->SetTextForeground(wxColour(0, 0, 0));
	pDC->DrawText(strDetailInfo, m_viewRectDetail.GetLeft() + 22, m_viewRectDetail.GetTop() + 3);

	pDC->SetFont(m_viewFont);
}

bool CFileListView::ReadyThreadFileImageList()
{
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
    {
        wxLogError("Could not create the worker thread!");
        return false;
    }

    return true;
}

bool CFileListView::RunThreadFileImageListRead()
{
	m_bImageListReadStarted = true;
	if (GetThread()->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not run the worker thread!");
		return false;
	}

	return true;
}

void CFileListView::StopThreadFileImageListRead()
{
	m_bImageListReadStarted = false;
	if(GetThread() && GetThread()->IsRunning())
		GetThread()->Wait();
}

void CFileListView::SetFileImage(int nIndex)
{
	if(!m_bImageListReadStarted)
		return;

	wxVector<CDirData>::iterator iter = m_itemList.begin() + nIndex;
#ifdef __WXMSW__
	bool isDrive = iter->IsDrive();
	//드라이브 || 이미지정보가 있음
	if(isDrive || iter->m_bIconImageSet)
		return;

	SetFileImage(iter);
#else
#endif
}

void CFileListView::SetFileImage(CDirData* pData)
{
	wxString strFullPath(pData->GetFullPath());
	wxString strName(pData->GetName());

	int nIconIndex = 0;
	int nOverlayIconIndex = 0;
	bool bIconInfoReadOK = false;

	if(pData->IsFile())
	{
		wxString strExt = theUtility->GetExt(strName);
		bIconInfoReadOK = theIconInfo->GetFileIconInfo(strExt, nIconIndex, nOverlayIconIndex);
	}

	if(!bIconInfoReadOK)
		theUtility->GetIconIndex(strFullPath, nIconIndex, nOverlayIconIndex);

	pData->m_iIconIndex = nIconIndex;
	pData->m_iOvelayIndex = nOverlayIconIndex;
	pData->m_bIconImageSet = true;
}

void CFileListView::DrawItemImage(wxDC* pDC, int x, int y, int flags, int nIconIndex, int nOverlayIndex)
{
#ifdef __WXMSW__
	flags |= INDEXTOOVERLAYMASK(nOverlayIndex + 1);
#endif // __WXMSW__

	m_pImageList->Draw(nIconIndex, pDC, x, y, flags);
}

wxThread::ExitCode CFileListView::Entry()
{
	int nStartIndex = m_nStartIndex;
	int nPosIndex = 0;

	for(int i = 0; i < m_nTotalItems; i++)
	{
		if(!m_bImageListReadStarted)
			break;

		SetFileImage(i);

		if(nStartIndex != m_nStartIndex)
		{
			nPosIndex = m_nCurrentItemIndex % m_nDisplayItemInView;
			nStartIndex = m_nStartIndex;
		}

		int nPositionSize = m_posList.size();
		if((nPositionSize > 0) && (nPosIndex < nPositionSize))
		{
			CPositionInfo posInfo = m_posList.at(nPosIndex);
			theUtility->RefreshWindow(this, posInfo.m_iconRect);

			nPosIndex++;
		}
	}

	if(m_bImageListReadStarted)
		theUtility->RefreshWindow(this, m_viewRect);

	return (wxThread::ExitCode)0;
}

void CFileListView::ShowBookmark()
{
	if (theBookmark->GetBookmarkSize() == 0)
	{
		wxMessageBox(wxT("Registered bookmark item is nothing"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}

	int nItemPosition = m_nCurrentItemIndex - m_nStartIndex;
	CPositionInfo posInfo = m_posList.at(nItemPosition);

	wxRect rcPos(posInfo.m_nameRect);
	rcPos.SetLeft(posInfo.m_nameRect.GetLeft() + 10);
	rcPos.SetBottom(posInfo.m_nameRect.GetBottom() + 1);

	int nFavoriteRight = posInfo.m_nameRect.GetRight() + 3;
	int nViewRight = m_viewRect.GetRight();

	if (nFavoriteRight >= nViewRight)
		rcPos.SetLeft(rcPos.GetLeft() - 20);

	wxPoint pt(rcPos.GetLeft() , rcPos.GetTop() + rcPos.GetHeight());
	wxMenu menuFavorite;
	theBookmark->CreateBookmarkMenu(&menuFavorite);

	this->PopupMenu(&menuFavorite, pt);
}

wxVector<CDirData>::iterator CFileListView::GetItemExist(const wxString& strName, bool& bExist)
{
	auto it = std::find_if(m_itemList.begin(), m_itemList.end(), [&strName](const CDirData& data) {
		wxString strTgt(data.GetName());
		wxString strSrc(strName);

		return (strSrc.Cmp(strTgt) == 0);
	});

	if (it != m_itemList.end())
		bExist = true;

	return it;
}
///////////////////////////////////////////////// wxCommandEvent Start /////////////////////////////////////////////////////////
//이름변경
void CFileListView::OnItemRename(wxCommandEvent &event)
{
	RenamePrepare();
}

//이름변경
void CFileListView::RenamePrepare()
{
	wxVector<CDirData>::const_iterator fIter = m_itemList.begin() + m_nCurrentItemIndex;
	m_strItemToRename = fIter->GetName();

#ifdef __WXMSW__
	if(fIter->IsDrive())
		return;
#endif // __WXMSW__
	if((theUtility->Compare(wxT(".."), m_strItemToRename) == 0))
		return;

	m_bRename = true;
	RenameOn(m_strItemToRename);
}

void CFileListView::RenameOn(const wxString& strRename)
{
	int iCurrentPosition = m_nCurrentItemIndex % m_nDisplayItemInView;
	CPositionInfo posInfo = m_posList.at(iCurrentPosition);

	int iPosX1 = posInfo.m_mainRect.GetLeft();
	int iPosY1 = posInfo.m_mainRect.GetTop();
	int iPosX2 = posInfo.m_mainRect.GetWidth();
	int iPosY2 = posInfo.m_mainRect.GetHeight() + 1;

	int iLength = strRename.Len();
	wxString strExt = theUtility->GetExt(strRename);
	int iExtLen = strExt.Len();
	if(iExtLen != 0)
	{
		iExtLen += 1;
		iLength -= iExtLen;
	}

	m_pTxtCtrlForRename->SetSize(wxRect(iPosX1, iPosY1, iPosX2, iPosY2));
	m_pTxtCtrlForRename->SetLabelText(strRename);
	m_pTxtCtrlForRename->SetSelection(0, iLength);
	m_pTxtCtrlForRename->Show(true);
	m_pTxtCtrlForRename->SetFocus();

	wxString strDontUse(theMsgManager->GetMessage(wxT("MSG_INFO_RENAME_DONTUSE")) + theMsgManager->GetMessage(wxT("MSG_INFO_RENAME_DONTUSE_STRING")));
	DisplayRenameTooltip(strDontUse, iPosX1 + 30, iPosY1 + iPosY2);
}

void CFileListView::RenameStart(const wxString& strOldPathName, const wxString& strNewPathName)
{
	int iResult = wxRename(strOldPathName, strNewPathName);
	if(iResult != 0)
	{
		wxString strMsg = wxT("Could not rename ") + strOldPathName + wxT(" to ") + strNewPathName;
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
	}
}

void CFileListView::DisplayRenameTooltip(const wxString& strMsg, int xPos, int yPos)
{
	m_pRenameTooltip->Show(false);

	wxClientDC dc(this);
	wxSize sztip = dc.GetTextExtent(strMsg);
	wxSize szTooltip;

	szTooltip.SetWidth(sztip.GetWidth() + 10);
	szTooltip.SetHeight(sztip.GetHeight() + 5);

	m_pRenameTooltip->SetTooltipText(strMsg);
	m_pRenameTooltip->SetThemeEnabled(true);
	m_pRenameTooltip->SetPosition(wxPoint(xPos, yPos));
	m_pRenameTooltip->SetSize(szTooltip);
	m_pRenameTooltip->Show(true);
}

void CFileListView::OnKeyDownTextCtrl(wxKeyEvent& event)
{
	int iKeyCode = event.GetKeyCode();
	event.Skip();

	if (iKeyCode == WXK_ESCAPE)
	{
		m_bRename = false;
		m_pTxtCtrlForRename->SetLabelText(wxT(""));
		m_pTxtCtrlForRename->Show(false);

		m_pRenameTooltip->Show(false);
	}
}

void CFileListView::OnEnterTextCtrl(wxCommandEvent& event)
{
	m_bRename = false;
	wxString strNewRename = m_pTxtCtrlForRename->GetValue();

	wxString strOldPathName = theUtility->MakeFullPathName(m_strCurrentPath, m_strItemToRename);
	wxString strNewPathName = theUtility->MakeFullPathName(m_strCurrentPath, strNewRename);

	if(CLocalFileSystem::IsWritable(strOldPathName, 0 , false))
	{
		bool bExist = false;
		GetItemExist(strNewRename, bExist);
		if (bExist)
		{
			wxString strMsg = strNewPathName + theMsgManager->GetMessage(wxT("MSG_DLG_ADD_DIR_EXIST"));
			wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
			return;
		}

		if(!theUtility->CheckCreatable(strNewRename))
		{
			RenameOn(strNewRename);
			return;
		}

		RenameStart(strOldPathName, strNewPathName);
	}
	else
	{
		wxString strUseAnotherPG(theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_USE_ANOTHER_PG")));
		wxString strMsg = strOldPathName + strUseAnotherPG;

		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
	}

	m_pTxtCtrlForRename->SetValue(wxT(""));
	m_pTxtCtrlForRename->Show(false);

	m_pRenameTooltip->Show(false);
	event.Skip();
}

void CFileListView::OnKillFocusTxtCtrl(wxFocusEvent& event)
{
	event.Skip();

	m_pTxtCtrlForRename->SetLabelText(wxT(""));
	m_pTxtCtrlForRename->Show(false);

	m_pRenameTooltip->Show(false);
	this->SetFocus();
}

//파일편집
void CFileListView::CheckCanFileEdit()
{
	int iExternalPGCount = _gExternalPGList.size();
	if(iExternalPGCount == 0)
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EDIT_NOT_SET")), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}

	int iSelCount = m_hashSelectedItem.size();
	if(iSelCount == 0)
	{
		CDirData data = m_itemList.at(m_nCurrentItemIndex);
		if (data.IsDir() || data.IsDrive())
		{
			wxString strMsg = data.IsDrive() ? wxT("MSG_FILE_MENU_EDIT_NOT_SUPPORT_DRV") : wxT("MSG_FILE_MENU_EDIT_NOT_SUPPORT_DIR");
			wxMessageBox(theMsgManager->GetMessage(strMsg), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}
	}
	else
	{
		bool bIncludedDir = false;
		std::unordered_map<int, SELITEM_INFO>::const_iterator fIter = m_hashSelectedItem.begin();
		while(fIter != m_hashSelectedItem.end())
		{
			SELITEM_INFO _info = fIter->second;
			if(!_info.m_bFile)
			{
				bIncludedDir = true;
				break;
			}

			fIter++;
		}

		if(bIncludedDir)
		{
			wxMessageBox(wxT("The directory is included in the selection"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}
	}

	if(iExternalPGCount > 1)
	{
		int iItemPosition = m_nCurrentItemIndex - m_nStartIndex;
		CPositionInfo posInfo = m_posList.at(iItemPosition);

		wxPoint pt(posInfo.m_nameRect.GetLeft() + 5, posInfo.m_nameRect.GetTop() + ICON_HEIGHT + 5);
		wxMenu menu;

		for (int i = 0; i < iExternalPGCount; i++)
		{
			EXTERNAL_EDIT_PROGRAM extInfo = _gExternalPGList.at(i);
			wxMenuItem* pMenuItem = menu.Append(EXTERNAL_PROGRAM_START_ID + i, extInfo._strPGName);

			int iIconIndex;
			int iOverlayIndex;

			theUtility->GetIconIndex(extInfo._strPGPath, iIconIndex, iOverlayIndex);
			wxIcon ico;

#ifdef __WXMSW__
			HICON hIcon = ImageList_GetIcon(_gImageList, iIconIndex, ILD_NORMAL);
			ico.CreateFromHICON(hIcon);
#else

#endif

			if (ico.IsOk())
			{
				wxBitmap bmp;
				bmp.CopyFromIcon(ico);

				if(bmp.IsOk())
					pMenuItem->SetBitmap(bmp);
				else
					pMenuItem->SetBitmap(_gExternalPG);
			}
			else
				pMenuItem->SetBitmap(_gExternalPG);

			DestroyIcon(hIcon);
		}

		this->PopupMenu(&menu, pt);
	}
	else
	{
		//프로그램 실행
		DoFileEdit(0);
	}
}

void CFileListView::OnMenuFileEdit(wxCommandEvent& event)
{
	CheckCanFileEdit();
}

void CFileListView::OnMenuFileEditProcess(wxCommandEvent& event)
{
	int iID = event.GetId();
	int iIndex = iID - EXTERNAL_PROGRAM_START_ID;

	DoFileEdit(iIndex);
}

void CFileListView::DoFileEdit(int iIndex)
{
	EXTERNAL_EDIT_PROGRAM extInfo = _gExternalPGList.at(iIndex);

	wxString strFilePG = extInfo._strPGPath;
	wxString strArgs = extInfo._strPGArgs;
	if (strFilePG == wxT(""))
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EDIT_NOT_SET")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	if (!wxFileExists(strFilePG))
	{
		wxString strMsg = wxString::Format(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EDIT_NOT_EXIST_PROGRAM")), strFilePG);
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	std::unordered_map<int, SELITEM_INFO>::iterator iTer = m_hashSelectedItem.begin();
	CDirData data;
	if (iTer == m_hashSelectedItem.end())
	{
		data = m_itemList.at(m_nCurrentItemIndex);
		wxString strFullPath = data.GetFullPath();
		wxExecute(strFilePG + wxT(" ") + strArgs + wxT(" ") + wxT("\"") + data.GetFullPath() + wxT("\""));
	}
	else
	{
		while(iTer != m_hashSelectedItem.end())
		{
			SELITEM_INFO _Info = iTer->second;
			data = m_itemList.at(_Info.m_iSelIndex);
			wxExecute(strFilePG + wxT(" ") + strArgs + wxT(" ") + wxT("\"") + data.GetFullPath() + wxT("\""));

			iTer++;
		}

		SelectedItemsClear();
		theUtility->RefreshWindow(this, m_viewRect);
	}
}

void CFileListView::SelectedItemsClear(bool bDeleted)
{
	if(!bDeleted)
	{
		std::unordered_map<int, SELITEM_INFO>::iterator iTer = m_hashSelectedItem.begin();
		while(iTer != m_hashSelectedItem.end())
		{
			SELITEM_INFO _Info = iTer->second;
			CDirData* data = (CDirData *)&m_itemList.at(_Info.m_iSelIndex);

			data->SetItemSelected(false);
			iTer++;
		}

		theUtility->RefreshWindow(this, m_viewRect);
	}

	m_hashSelectedItem.clear();

	m_pSelectedItemView->SetTooltipText(wxT(""));
	m_pSelectedItemView->Show(false);

	m_nSelDirCnt = 0;
	m_nSelFileCnt = 0;
}

//컨택스트 메뉴 보기
void CFileListView::OnViewContextMenu(wxCommandEvent& event)
{
	int nItemPosition = m_nCurrentItemIndex % m_nDisplayItemInView;
	CPositionInfo posInfo = m_posList.at(nItemPosition);

	wxPoint pt(posInfo.m_nameRect.GetRight() / 4, posInfo.m_nameRect.GetTop() + ICON_HEIGHT);

	DisplayContextMenu(pt);
}

void CFileListView::DisplayContextMenu(const wxPoint& pt)
{
	int nSelectedItemIndex = m_nCurrentItemIndex;
	int nItemCount = m_itemList.size();

	if (nItemCount < m_nCurrentItemIndex)
		m_nCurrentItemIndex = nSelectedItemIndex;

	wxArrayString arrString;
	int nSelectedItems = m_hashSelectedItem.size();
	if (nSelectedItems != 0)
	{
		std::unordered_map<int, SELITEM_INFO>::const_iterator iter = m_hashSelectedItem.begin();
		while(iter != m_hashSelectedItem.end())
		{
			SELITEM_INFO _ItemInfo = iter->second;
			arrString.Add(_ItemInfo.m_strFullPath);

			iter++;
		}
	}
	else
	{
		wxString strContextPath = m_strCurrentPath;
		CDirData item = m_itemList.at(m_nCurrentItemIndex);
		if (item.GetName().Cmp(wxT("..")) == 0)
			return;
#ifdef __WXMSW__
		strContextPath = item.IsDrive() ? item.GetDriveName() : item.GetFullPath();
#else
		strContextPath = item.GetFullPath();
#endif

		arrString.Add(strContextPath);
	}

	CContextMenuHandler ctxMenu;
	ctxMenu.SetObject(arrString);
	ctxMenu.ShowContextMenu(this, pt);
}

///////////////////////////////////////////////// wxCommandEvent End /////////////////////////////////////////////////////////
void CFileListView::UpdateLastAccessTime(const wxDateTime& dt)
{
	if(m_nPathDepth == 1)
		return;

	wxVector<CDirData>::iterator iter = m_itemList.begin();
	iter->SetDateTime(dt);
}

void CFileListView::OnMouseLBottonDown(wxMouseEvent& event)
{
	DoMouseProcess(event.GetPosition());
	if(!m_bSetFocus)
		_gContextManager->CheckEqualAndChangeTab(m_pParent->GetParent());
}

void CFileListView::OnMouseLButtonDBClick(wxMouseEvent& event)
{
	DoMouseProcess(event.GetPosition(), true);
}

void CFileListView::OnMouseRButtonDown(wxMouseEvent& event)
{
	DoMouseProcess(event.GetPosition());
}

void CFileListView::OnMouseRButtonUp(wxMouseEvent& event)
{
	DisplayContextMenu(event.GetPosition());
}

void CFileListView::OnMouseMove(wxMouseEvent& event)
{
//	m_bMouseMoveAndFound = FindItemInMousePoint(event.GetPosition(), true);
	event.Skip();
}

void CFileListView::OnMouseWheel(wxMouseEvent& event)
{
	if(!m_bSetFocus)
		return;

	int iWheelRotation = event.GetWheelRotation();
	if (iWheelRotation < 0) //Down
	{
		m_nCurrentItemIndex++;
		if(m_nCurrentItemIndex > (m_nTotalItems - 1))
			m_nCurrentItemIndex = m_nTotalItems - 1;
	}
	else //Up
	{
		m_nCurrentItemIndex--;
		if(m_nCurrentItemIndex < 0)
			m_nCurrentItemIndex = 0;
	}

	theUtility->RefreshWindow(this, m_viewRect);
}

void CFileListView::DoMouseProcess(const wxPoint& pt, bool bDblClick)
{
	m_bMouseClickItemFound = false;
	if (FindItemInMousePoint(pt))
	{
		if (bDblClick)
			ProcessEnterKey(WXK_RETURN);

		m_bMouseClickItemFound = true;
		theUtility->RefreshWindow(this, m_viewRect);
	}
}

bool CFileListView::FindItemInMousePoint(const wxPoint& pt, bool IsMouseMove)
{
	bool bFoundOK = false;
	wxVector<CPositionInfo>::const_iterator fIter = m_posList.begin();

	int iClickPosIndex = 0;
	while(fIter != m_posList.end())
	{
		if(fIter->m_mainRect.Contains(pt))
		{	//아이템 시작인덱스 + 클릭인덱스 < 전체 아이템수
			bFoundOK = (m_nStartIndex + iClickPosIndex) < m_nTotalItems ? true : false;
			break;
		}

		iClickPosIndex++;
		fIter++;
	}

	if(bFoundOK)
	{
		if(!IsMouseMove)
			m_nCurrentItemIndex = m_nStartIndex + iClickPosIndex;
	}

	return bFoundOK;
}

wxPoint CFileListView::GetSelectedItemPoint()
{
	int nItemPosition = m_nCurrentItemIndex - m_nStartIndex;//% m_nDisplayItemInView;
	CPositionInfo posInfo = m_posList.at(nItemPosition);

	wxPoint pt(posInfo.m_nameRect.GetLeft() + 5, posInfo.m_nameRect.GetTop() + ICON_HEIGHT + 5);

	return pt;
}

void CFileListView::GetSelectedCompressedItems(std::vector<wxString>& vecItems)
{
	if(m_hashSelectedItem.size() == 0)
	{
		wxVector<CDirData>::iterator fIter = m_itemList.begin() + m_nCurrentItemIndex;
		if(fIter->IsDrive())
		{
			wxMessageBox(wxT("The selected item is drive and You cannnot will be compressison"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}

		vecItems.emplace_back(fIter->GetFullPath());
	}
	else
	{
		std::unordered_map<int, SELITEM_INFO>::const_iterator fIter = m_hashSelectedItem.begin();
		while(fIter != m_hashSelectedItem.end())
		{
			SELITEM_INFO _info = fIter->second;

			vecItems.emplace_back(_info.m_strFullPath);
			fIter++;
		}
	}
}

void CFileListView::GetFileOperationSrc(std::list<wxString>& lstSrc, bool IsCut)
{
	if(m_hashSelectedItem.size() == 0)
	{
		wxVector<CDirData>::iterator fIter = m_itemList.begin() + m_nCurrentItemIndex;
		if(fIter->IsDrive())
		{
			wxMessageBox(theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_NOT_SUPPORT_DRIVE")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
			return;
		}

		fIter->SetCut(false);
		if(IsCut)
			fIter->SetCut(true);

		lstSrc.push_back(fIter->GetFullPath());
	}
	else
	{
		std::unordered_map<int, SELITEM_INFO>::iterator iTer = m_hashSelectedItem.begin();
		while(iTer != m_hashSelectedItem.end())
		{
			SELITEM_INFO _Info = iTer->second;
			wxVector<CDirData>::iterator fIter = m_itemList.begin() + _Info.m_iSelIndex;

			fIter->SetCut(false);
			if(IsCut)
				fIter->SetCut(true);

			lstSrc.push_back(fIter->GetFullPath());
			iTer++;
		}
	}

	if(IsCut)
		theUtility->RefreshWindow(this, m_viewRect);
}

void CFileListView::FindMatchItems()
{
	m_matchItems.clear();

	wxString strKeyInput(m_strKeyInput);
#ifdef __WXMSW__
	strKeyInput.MakeLower();
#endif
	//문자열 포함
	wxString strRegEx(wxT("^.*(") + strKeyInput + wxT(").*"));
	wxRegEx _regex(strRegEx);

	wxString strName;
	for (int iIndex = 0; iIndex < m_nTotalItems; iIndex++)
	{
		wxVector<CDirData>::iterator iter = m_itemList.begin() + iIndex;
		strName = iter->GetName();

		if ((strName == wxT("..")) || iter->IsDrive())
			continue;

#ifdef __WXMSW__
		strName.MakeLower();
#endif
		if (!strKeyInput.IsEmpty())
		{
			if (_regex.Matches(strName))
			{
				iter->SetMatch(true);
				m_matchItems.push_back(iIndex);
			}
			else
				iter->SetMatch(false);
		}
		else
			iter->SetMatch(false);
	}

	if (m_matchItems.size() > 0)
	{
		m_nCurrentItemIndex = m_matchItems.at(0);
		ReCalcPage();
	}
}

void CFileListView::DoMatchClear()
{
	wxVector<int>::const_iterator fIter = m_matchItems.begin();
	while(fIter != m_matchItems.end())
	{
		wxVector<CDirData>::iterator iter = m_itemList.begin() + *fIter;
		iter->SetMatch(false);

		fIter++;
	}

	m_matchItems.clear();
}

void CFileListView::InitKeyInputTooltip()
{
	DoMatchClear();

	m_pKeyInputToolTip->SetTooltipText(wxT(""));
	m_pKeyInputToolTip->Show(false);
	m_strKeyInput = wxT("");
}

void CFileListView::AllItemSelectOrRelease(_MENU_EVENT_TYPE mType)
{
	m_hashSelectedItem.clear();
	m_nSelDirCnt = 0;
	m_nSelFileCnt = 0;

	bool bAllSelect = mType == _MENU_EDIT_ALL_SELECT ? true : false;
	wxVector<CDirData>::iterator fIter = m_itemList.begin();
	int iIndex = 0;
	while(fIter != m_itemList.end())
	{
		if(!fIter->IsDrive() && (fIter->GetName().Cmp(wxT("..")) != 0))
		{
			fIter->SetItemSelected(bAllSelect);

			if(bAllSelect)
			{
				if(fIter->IsDir())
					m_nSelDirCnt++;
				else
					m_nSelFileCnt++;

				SELITEM_INFO _Info;
				_Info.m_iSelIndex = iIndex;
				_Info.m_bFile = fIter->IsFile();
				_Info.m_strFullPath = fIter->GetFullPath();

				std::unordered_map<int, SELITEM_INFO>::value_type valsel(iIndex, _Info);
				m_hashSelectedItem.insert(valsel);
			}
		}

		fIter++;
		iIndex++;
	}

	if(bAllSelect)
	{
		wxString strSelString(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_SELINFO")));
		wxString strSelItems = strSelString + wxString::Format(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_ITEM_SELECT")), m_nSelDirCnt, m_nSelFileCnt);

		DisplaySelectedItemInfo(strSelItems, 0, 0);
	}
	else
		m_pSelectedItemView->Show(false);

	theUtility->RefreshWindow(this, m_viewRect);
}

//컬럼 변경
void CFileListView::OnChangeViewColumn(wxCommandEvent& event)
{
	m_bSizeOrColumnChanged = true;
	theUtility->RefreshWindow(this, m_viewRect);
}

void CFileListView::OnChangeFileList(wxCommandEvent& event)
{
	wxString strPath(m_strCurrentPath);

	LoadDirectory(strPath);
	theUtility->RefreshWindow(this, m_viewRect);
}

void CFileListView::OnChangeSorting(wxCommandEvent& event)
{
	int iSortType = theJsonConfig->GetSortType();
	wxVector<CDirData>::const_iterator iter = m_itemList.begin();
	wxString strName = iter->GetName();
	int iSortStartIndex = 0;

	if(strName.Cmp(wxT("..")) == 0)
		iSortStartIndex = 1;

	switch(iSortType)
	{
		case VIEW_SORT_DEFAULT:
			std::sort(m_itemList.begin() + iSortStartIndex, m_itemList.end()/* - iSortEndIndex */, CSorting::DirSortOfName);
			break;

		case VIEW_SORT_TIME:
			std::sort(m_itemList.begin() + iSortStartIndex, m_itemList.end()/* - iSortEndIndex */, CSorting::DirSortOfTime);
			break;

		case VIEW_SORT_SIZE:
			std::sort(m_itemList.begin() + iSortStartIndex, m_itemList.end()/* - iSortEndIndex */, CSorting::DirSortOfSize);
			break;

		default:
			break;
	}

	theUtility->RefreshWindow(this, m_viewRect);
}

void CFileListView::OnShowBookmark(wxCommandEvent& event)
{
	ShowBookmark();
}

void CFileListView::OnExecMenuClick(wxCommandEvent& event)
{
	int nMenuId = event.GetInt();
	int nKeyCode = nMenuId == _MENU_PATH_BACK ? wxKEY_DOT : WXK_REVERSE_SLASH;

	ProcessKeyEvent(nKeyCode);
}

void CFileListView::OnShowCompressMenu(wxCommandEvent& event)
{
	std::vector<wxString> vCompress = theCompressRelease->GetCompressList();

	int iItemPosition = m_nCurrentItemIndex - m_nStartIndex;
	CPositionInfo posInfo = m_posList.at(iItemPosition);

	wxPoint pt(posInfo.m_nameRect.GetLeft() + 5, posInfo.m_nameRect.GetTop() + ICON_HEIGHT + 5);
	wxMenu menu;

	int iID = 0;
	for (auto item : vCompress)
	{
		menu.Append(COMPRESS_START_ID + iID, item);
		iID++;
	}

	this->PopupMenu(&menu, pt);
}

void CFileListView::OnCompressMenuExecute(wxCommandEvent& event)
{
	wxVector<CDirData>::iterator iter = m_itemList.begin() + m_nCurrentItemIndex;

	int nSelCount = m_hashSelectedItem.size();
	wxString strFullPath(m_strCurrentPath);
	if(nSelCount == 0)
	{
		if (iter->IsDrive())
		{
			wxMessageBox(wxT("The selected item is drive and You cannnot will be compressison"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}

		strFullPath = iter->GetFullPath();
	}

	int nId = event.GetId();
	int nIndex = nId - COMPRESS_START_ID;

	wxString strCompressExt = theCompressRelease->GetCompressList().at(nIndex);
	if(strCompressExt.CmpNoCase(wxT("gz")) == 0)
	{
		if(nSelCount > 0)
		{
			wxMessageBox(strCompressExt + wxT(" can compress one file!"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}

		if(iter->IsDir())
		{
			wxMessageBox(strCompressExt + wxT(" can compress a file only!"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}
	}

	wxString strTmp = theUtility->GetPathName(strFullPath);
	wxString strCompressedFile(wxT(""));

	if(nSelCount != 0)
		strCompressedFile = m_nPathDepth == 1 ? m_strCurrentPath + m_strCurrentPath.Left(1) + wxT(".") + strCompressExt : m_strCurrentPath + SLASH + strTmp + wxT(".") + strCompressExt;
	else
	{
		wxString strTmpIncludeExt(theUtility->GetFileName(strFullPath, true));
		strTmp = theUtility->GetFileName(strFullPath, false);
		if(strCompressExt.CmpNoCase(wxT("gz")) == 0)
			strTmp = strTmpIncludeExt;

		strCompressedFile = m_nPathDepth == 1 ? m_strCurrentPath + strTmp + wxT(".") + strCompressExt : m_strCurrentPath + SLASH + strTmp + wxT(".") + strCompressExt;
	}

	wxCommandEvent evt(wxEVT_COMPRESS_EXEC);
	evt.SetId(nId);
	evt.SetString(strCompressedFile);

	wxPostEvent(_gMenuEvent, evt);
}


void CFileListView::OnViewDirNumbering(wxCommandEvent& event)
{
	m_bDirectoryNumbering = !m_bDirectoryNumbering;
	theUtility->RefreshWindow(this, m_viewRect);
}
