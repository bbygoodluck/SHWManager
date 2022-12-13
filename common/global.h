#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

class CTabManager;
#ifdef __WXMSW__
	typedef HIMAGELIST SYSTEM_IMAGE_LIST;
#else
	typedef wxImageList* SYSTEM_IMAGE_LIST;
#endif

extern wxFrame* _gMainFrame;               // 메인윈도우
extern SYSTEM_IMAGE_LIST _gImageList;      // 이미지리스트
extern wxFont* _gViewFont;                 // 전역뷰폰트
extern wxIcon _gNormalFolderIco;           // Default 폴더 이미지
extern wxIcon _gNormalFileIco;             // Default 파일 이미지
extern wxBitmap _gExternalPG;              // 외부편집프로그램 메뉴 이미지
extern wxString _gSystemLang;              // System language

//외부편집프로그램 목록
extern std::unordered_map<int, EXTERNAL_EDIT_PROGRAM> _gExternalPGList;
// 파일유형 캐쉬
extern std::unordered_map<wxString, wxString> _gCacheFileType;

// 파일속성 인덱스
extern unsigned int _gAttrIndex;
//뷰관리자
extern CContextManager* _gContextManager;
//Active Tab
extern CTabManager* _gActivatedTab;
//컬럼보기 목록
extern const COLUMN_VIEW_OPTION _gColumnViewOPEnum[];
//화면이동에 대한 허용키
extern std::unordered_map<int, int> _gmapAllowProcessKey;

// 디렉토리 탐색시 제외 항목
extern wxVector<wxString> _gVecIgnore;
//파일리스트 보기 옵션
extern const FILE_VIEW_OPTION _gFileViewOPEnum[];
//메뉴 이벤트
extern CMenuEvent* _gMenuEvent;
#endif // GLOBAL_H_INCLUDED
