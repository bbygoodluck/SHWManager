#include "common.h"

// 메인윈도우
wxFrame* _gMainFrame          = nullptr;

// 이미지리스트
SYSTEM_IMAGE_LIST _gImageList = nullptr;

// 전역뷰폰트
wxFont* _gViewFont            = nullptr;

// Default 폴더 이미지
wxIcon _gNormalFolderIco      = wxNullIcon;

// Default 파일 이미지
wxIcon _gNormalFileIco        = wxNullIcon;

// 외부편집프로그램 메뉴 이미지
wxBitmap _gExternalPG         = wxNullBitmap;

// System language
wxString _gSystemLang         = wxT("");

//외부편집프로그램 목록
std::unordered_map<int, EXTERNAL_EDIT_PROGRAM> _gExternalPGList;

// 파일유형 캐쉬
std::unordered_map<wxString, wxString> _gCacheFileType;

//화면이동에 대한 허용키
std::unordered_map<int, int> _gmapAllowProcessKey;

//파일속성처리 인덱스
unsigned int _gAttrIndex = 0;

//Context관리자
CContextManager* _gContextManager = nullptr;

//Active Tab
CTabManager* _gActivatedTab = nullptr;

//컬럼보기 목록
const COLUMN_VIEW_OPTION _gColumnViewOPEnum[] = {
	VIEW_COLUMN_AUTO,
	VIEW_COLUMN_ONE,
	VIEW_COLUMN_TWO,
	VIEW_COLUMN_THREE,
	VIEW_COLUMN_FOUR,
	VIEW_COLUMN_FIVE,
	VIEW_COLUMN_SIX,
	VIEW_COLUMN_SEVEN,
	VIEW_COLUMN_EIGHT,
	VIEW_COLUMN_NINE
};

//파일리스트 보기 옵션
const FILE_VIEW_OPTION _gFileViewOPEnum[] = {FILE_VIEW_OP_DEFAULT ,	FILE_VIEW_OP_HIDDEN, FILE_VIEW_OP_ALL};
// 디렉토리 탐색시 제외 항목
wxVector<wxString> _gVecIgnore;
//메뉴이벤
CMenuEvent* _gMenuEvent = nullptr;
