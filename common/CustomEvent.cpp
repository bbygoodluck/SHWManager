#include "common.h"
#include "CustomEvent.h"

//디렉토리 Load
wxDEFINE_EVENT(wxEVT_CHANGE_DIRECTORY, wxCommandEvent);
//이름변경
wxDEFINE_EVENT(wxEVT_ITEM_RENAME, wxCommandEvent);
//파일편집
wxDEFINE_EVENT(wxEVT_FILE_EDIT, wxCommandEvent);
//파일 시스템 Watcher
wxDEFINE_EVENT(wxEVT_FS_WATCHER, wxCommandEvent);
//컨택스트 메뉴 보기
wxDEFINE_EVENT(wxEVT_VIEW_CONTEXTMENU, wxCommandEvent);
//즐겨찾기 보기
wxDEFINE_EVENT(wxEVT_VIEW_BOOKMARK, wxCommandEvent);
#ifdef __WXMSW__
//드라이브 재로드하기
wxDEFINE_EVENT(wxEVT_DRIVE_RELOAD, wxCommandEvent);
//드라이브 용량 갱신
wxDEFINE_EVENT(wxEVT_DRIVE_SPACE_UPDATE, wxCommandEvent);
//디렉토리 관리자 새로고침
wxDEFINE_EVENT(wxEVT_DIRMANAGER_REFRESH_ALL, wxCommandEvent);
//컬럼 변경
wxDEFINE_EVENT(wxEVT_CHANGE_VIEWCOLUMN, wxCommandEvent);
//파일리스트 변경
wxDEFINE_EVENT(wxEVT_CHANGE_FILELIST, wxCommandEvent);
//Sorting 변경
wxDEFINE_EVENT(wxEVT_CHANGE_SORTING, wxCommandEvent);
#endif // __WXMSW__
//CPU /memory / pagefile 모니터링
wxDEFINE_EVENT(wxEVT_RESOURCE_MONITORING, wxCommandEvent);
//메뉴클릭 실행
wxDEFINE_EVENT(wxEVT_EXEC_MENU_CLICK, wxCommandEvent);
//압축메뉴 보기
wxDEFINE_EVENT(wxEVT_SHOW_COMPRESS_MENU, wxCommandEvent);
//압축하기
wxDEFINE_EVENT(wxEVT_COMPRESS_EXEC, wxCommandEvent);
//압축체크
wxDEFINE_EVENT(wxEVT_DECOMPRESS_DUP_CHECK, wxCommandEvent);