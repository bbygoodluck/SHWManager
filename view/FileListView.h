#ifndef LISTVIEW_H_INCLUDED
#define LISTVIEW_H_INCLUDED

#include "../interface/MemoryDC.h"
#include "../interface/PositionInfo.h"
#include "../interface/DirData.h"
#include "../interface/Sorting.h"
#include "../interface/ImageList.h"

#include "../engine/LocalFileSystem.h"

struct _HistoryItem
{
	int iCurrentIndex;
	int iStartIndex;
};

WX_DECLARE_STRING_HASH_MAP(_HistoryItem, _HISTORY);

class CSHWTooltipView;
class CFileListView : public wxWindow, public wxThreadHelper
{
protected:
	enum VIEW_SORT {
		VIEW_SORT_DEFAULT = 0,
		VIEW_SORT_TIME,
		VIEW_SORT_SIZE,
	};

	class CColumnPoint
	{
	public:
		wxPoint pt1;
		wxPoint pt2;
	};

public:
    explicit CFileListView(wxWindow* parent, const int nID, const wxSize& sz);
	virtual ~CFileListView();

	void SetRemote(bool IsRemote) {
		m_bRemote = IsRemote;
	}

	bool IsRemote() { return m_bRemote; }
	virtual void LoadDirectory(const wxString& strPath) = 0;

	wxString GetCurrentPath() { return m_strCurrentPath; }

	void GetFileOperationSrc(std::list<wxString>& lstSrc, bool IsCut = false);
	void SelectedItemsClear(bool bDeleted = false);
	void GetSelectedCompressedItems(std::vector<wxString>& vecItems);

	wxPoint GetSelectedItemPoint();

#ifdef __WXMSW__
	virtual void SetDriveSpace(const wxString& strVolume) = 0;
#endif // __WXMSW__

	void AllItemSelectOrRelease(_MENU_EVENT_TYPE mType);
	wxWindow* GetViewParent() { return m_pParent; }

	//2023.05.26 Add Start **************************************************************
	bool IsShowDirectoryNumbering() const { return m_bDirectoryNumbering; }
	//2023.05.26 Add End   **************************************************************
public:
	typedef struct _SELITEM_INFO {
		int m_iSelIndex;
		wxString m_strFullPath;
		bool m_bFile;
	} SELITEM_INFO;

protected:
	wxWindow* m_pParent               = nullptr;

    const int ICON_WIDTH              = 16;
	const int ICON_HEIGHT             = 16;
	const int ICON_SELECTED_WIDTH_GAP = 10;
	const int GAP_WIDTH               = 2;
	const int GAP_HEIGHT              = 2;

	wxIcon   m_icoUpDir;
    wxColour m_colDefault;
	wxColour m_colDrive;
	wxColour m_colDir;
	wxColour m_colSize;
	wxColour m_colTime;
	wxColour m_colAttr;
	wxColour m_colType;
	wxColour m_colMatch;

	CMemoryDC m_memDC;

	// 데이터
	wxVector<CDirData> m_itemList;
	//화면폰트
    wxFont m_viewFont = wxNullFont;
    //화면 사이즈
    wxRect m_viewRect = wxRect(0, 0, 0, 0);
    //디렉토리 정보 표시영역
	wxRect m_viewDirInfo      = wxRect(0, 0, 0, 0);
	//상세보기 영역
	wxRect m_viewRectDetail   = wxRect(0, 0, 0, 0);
	//화면 표시 영역
	wxRect m_viewRectDisp     = wxRect(0, 0, 0, 0);

	//화면 사이즈 변경 플래그
    bool m_bSizeOrColumnChanged = false;
	//디렉토리 Load 플래그
	bool m_bDirLoaded           = false;
	// 화면에 표시가능한 아이템
	bool m_bDispFlag[5] = {false, };
	// 쓰레드 시작플래그
	bool m_bImageListReadStarted = false;
	//윈도우 포커스 플래그
	bool m_bSetFocus    = false;
	//리모트 여부
	bool m_bRemote      = false;
	//이름변경플래그
	bool m_bRename      = false;
	//마우스 클릭&아이템찾기
	bool m_bMouseClickItemFound = false;
	//2023.05.26 Add Start *********************************************************
	bool m_bDirectoryNumbering  = false;
	//2023.05.26 Add End   *********************************************************
	// 전체아이템수
	int	m_nTotalItems         = 0;
    //Path Depth
	int m_nPathDepth          = 1;
	// 디렉토리수
	int m_nDirCount           = 0;
	// 파일수
	int m_nFileCount          = 0;
	// 컬럼수
	int	m_nDispColumn         = 0;
	// 문자 높이
	int m_iCharHeight         = 0;
	// 컬럼당 표시가능한 아이템수
	int m_nItemCountInColumn  = 0;
	// 컬럼끝 포인트
	int	m_nDispColumnEndPoint = 0;
	// 화면에 표시가능한 아이템수
	int	m_nDisplayItemInView  = 0;
	// 화면표시 아이템 시작위치
	int m_nStartIndex         = 0;
	//전체 포지션 카운터
	int m_iTotalPositionCnt   = 0;
	// 현재커서위치
	int m_nCurrentItemIndex   = 0;
	//선택 파일수
	int m_nSelFileCnt         = 0;
	//선택 디렉토리 수
	int m_nSelDirCnt          = 0;

	//폴더 사이즈
	double m_dblFileSizeInDir = 0.0f;

#ifdef __WXMSW__
	double m_dblTotalSpace      = 0.0f;
	double m_dblFreeSpace       = 0.0f;
	wxString m_strVolume        = wxT("");
	wxString m_strDriveSpace    = wxT("");
#endif // __WXMSW__
	// 가장긴 이름
	wxString m_strMaxName     = wxT("");
	// 가장긴 파일유형
	wxString m_strMaxTypeName = wxT("");
	//현재 패스
	wxString m_strCurrentPath = wxT("");
	//키 입력값
	wxString m_strKeyInput    = wxT("");
	//이름변경
	wxString m_strItemToRename = wxT("");
	//표시컬럼
	COLUMN_VIEW_OPTION m_enumColumnViewOption;
	// 컬럼 라인 정보
	wxVector<CColumnPoint> m_ptList;
	// 포지션정보
	wxVector<CPositionInfo>	m_posList;

	//이미지리스트
	std::unique_ptr<CImageList> m_pImageList = nullptr;
	// 화면표시이름
	std::unordered_map<wxString, wxString> m_dispNameInfoMap;
	//선택 아이템
	std::unordered_map<int, SELITEM_INFO> m_hashSelectedItem;

	//디렉토리방문이력
	_HISTORY m_hashHistory;
	//선택아이템 정보
	CSHWTooltipView* m_pSelectedItemView = nullptr;
	//이름변경시 입력제한 툴팁
	CSHWTooltipView* m_pRenameTooltip = nullptr;
	//키 입력
	CSHWTooltipView* m_pKeyInputToolTip = nullptr;

	// 이름변경 텍스트컨트롤
	std::unique_ptr<wxTextCtrl> m_pTxtCtrlForRename = nullptr;

	//FileSystemWatcher Queue
//	CWatcherQueue*  m_pWatcherQueue = nullptr;

	//Match 아이템 정보
	wxVector<int> m_matchItems;

private:
    int m_iFontSize = 10;
    wxString m_strFontName = wxT("Segoe UI");

private:
	void DisplaySelectedItemInfo(const wxString& strMsg, int xPos, int yPos);
	void DisplayRenameTooltip(const wxString& strMsg, int xPos, int yPos);

	//편집 가능여부 체크
	void CheckCanFileEdit();
	//외부프로그램 실행
	void DoFileEdit(int iIndex);

protected:
	virtual void Render(wxDC* pDC) = 0;
    virtual void ApplyChangedViewSize() = 0;

	void Initialize();
	void SortStart();
	void Clear();

	virtual void CalcColumn(wxDC* pDC);
	virtual bool CalcAutoColumn(wxDC* pDC, const wxRect& viewRect);
	virtual void CalcPosition(wxDC* pDC);
	virtual void ReCalcPage();
	//문자 ...표시
	wxString CalcStringEllipse(wxDC* pDC, const wxString& strSrc, bool IsDrive);

	virtual void DrawColumn(wxDC* pDC);
	virtual void DrawInfoArea(wxDC* pDC);
	virtual void DisplayItems(wxDC* pDC);
	virtual void DisplayDirInfo(wxDC* pDC);
	virtual void DisplayDetailInfo(wxDC* pDC, int nIndex);

	void DrawItemImage(wxDC* pDC, int x, int y, int flags, int nIconIndex = 0, int nOverlayIndex = 0);

	//파일 이미지 읽기 쓰레드
	bool ReadyThreadFileImageList();
	bool RunThreadFileImageListRead();
	void StopThreadFileImageListRead();
	void SetFileImage(int nIndex);
	void SetFileImage(CDirData* pData);

	wxThread::ExitCode Entry() override;

	//마우스이벤트 처리
	void DoMouseProcess(const wxPoint& pt, bool bDblClick = false);
	bool FindItemInMousePoint(const wxPoint& pt, bool IsMouseMove = false);

	//Key Event 처리
	virtual void PreTranslateKeyEvent(wxKeyEvent& event);
	bool ProcessKeyEvent(int nKeyCode);
	void MoveLeftAndUpKey();
	void MoveRightAndDownKey();
	void GotoRoot();
	bool ProcessEnterKey(int nKeyCode);

	bool SetSelectedItem();
	virtual void ShowBookmark();

	//이름변경
	void RenamePrepare();
	void RenameOn(const wxString& strRename);
	void RenameStart(const wxString& strOldPathName, const wxString& strNewPathName);

	//아이템존재여부 확인
	wxVector<CDirData>::iterator GetItemExist(const wxString& strName, bool& bExist);
	//마지막 접근 시간 갱신
	void UpdateLastAccessTime(const wxDateTime& dt);

	//콘텍스트 메뉴 보기
	void DisplayContextMenu(const wxPoint& pt);

	//이아템 매칭처리
	void FindMatchItems();
	void DoMatchClear();
	void InitKeyInputTooltip();

protected:
    void OnCharHook(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);
	void OnSetFocus(wxFocusEvent& event);
	void OnKillFocus(wxFocusEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);

	//마우스 이벤트
	void OnMouseLBottonDown(wxMouseEvent& event);
	void OnMouseLButtonDBClick(wxMouseEvent& event);
	void OnMouseRButtonDown(wxMouseEvent& event);
	void OnMouseRButtonUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);

	//Custom Event
	void OnItemRename(wxCommandEvent &event);
	//이름변경 이벤트
	void OnEnterTextCtrl(wxCommandEvent& event);
	void OnKeyDownTextCtrl(wxKeyEvent& event);
	void OnKillFocusTxtCtrl(wxFocusEvent& event);
	//파일편집(외부편집프로그램이 여러개인경우)
	void OnMenuFileEditProcess(wxCommandEvent& event);
	void OnMenuFileEdit(wxCommandEvent& event);

	//컨택스트 메뉴보기
	void OnViewContextMenu(wxCommandEvent& event);
	//즐겨찾기 메뉴보기
	void OnShowBookmark(wxCommandEvent& event);
	//컬럼 변경
	void OnChangeViewColumn(wxCommandEvent& event);
	//파일리스트 변경
	void OnChangeFileList(wxCommandEvent& event);
	//Sorting 변경
	void OnChangeSorting(wxCommandEvent& event);
	//FileSystem Watcher
	virtual void OnFileSystemWatcher(wxCommandEvent& event) = 0;
	//메뉴 클릭 이벤트 실행
	void OnExecMenuClick(wxCommandEvent& event);
	//압축메뉴 보기
	void OnShowCompressMenu(wxCommandEvent& event);
	void OnCompressMenuExecute(wxCommandEvent& event);

	// 2023.05.26 Add Start *****************************************
	//디렉토리 넘버링
	void OnViewDirNumbering(wxCommandEvent& event);
	// 2023.05.26 Add End   *****************************************
    wxDECLARE_EVENT_TABLE();
};

#endif // LISTVIEW_H_INCLUDED
