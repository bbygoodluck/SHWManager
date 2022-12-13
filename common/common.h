#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/snglinst.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>
#include <wx/aui/aui.h>
#include <wx/display.h>
#include <wx/splitter.h>
#include <wx/tokenzr.h>
#include <wx/volume.h>
#include <wx/regex.h>
#include <wx/thread.h>
#include <wx/clipbrd.h>

#include <memory>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <queue>
#include <list>

#ifdef __WXMSW__
	#include <wx/msw/private.h>
	#include <windows.h>
	#include <CommCtrl.h>
	#include <WinUser.h>
	#include <imm.h>
	#include <shobjidl.h>
	#include <shlobj.h>
	#include <Dbt.h>
#endif

//버전정보
#include "Version.h"
//define
#include "defs.h"
//Lock변수
#include "Lock.h"
//열거형
#include "genum.h"
//뷰 관리자
#include "../interface/ContextManager.h"
//메뉴이벤트
#include "../MenuEvent.h"
//extern 변수
#include "global.h"
//공통함수 모음
#include "Utility.h"
//메세지 관리
#include "MsgManager.h"
//환경설정
#include "JsonConfig.h"
//북마크(즐겨찾기)
#include "JsonBookmark.h"
//확장자 정보
#include "Extinfo.h"
//화면분할
#include "../SplitterImpl.h"
//이벤트 목록
#include "CustomEvent.h"
//디렉토리 관리기
#include "../interface/SHCD.h"
//시스템 Usage
#include "../system/SystemResourceInfo.h"

//공용함수
#define theUtility   	    (CUtility::Get())
//환경설정
#define theJsonConfig       (CJsonConfig::Get())
//Message정의
#define theMsgManager	    (CMsgManager::Get())
//북마크(즐겨찾기)
#define theBookmark         (CJsonBookmark::Get())
//확장자 정의
#define theExtInfo          (CExtInfo::Get())
//분할윈도우 관리
#define theSplitterManager  (CSplitterImpl::Get())
//리소스 Usage
#define theSystem           (CSystemResourceInfo::Get())

#ifdef __WXMSW__
    #include "../interface/msw/DriveInfo.h"
    #include "JsonFileIconInfo.h"
	#include "../interface/msw/MSWCopyMoveClipboard.h"

    #define theIconInfo      (CJsonFileIconInfo::Get())
    #define theDriveInfo     (CDriveInfo::Get())
    #define theClipboard     (CClipboard::Get())
#endif // __WXMSW__

//디렉토리 트리관리
#define theSHCD              (CDirTree::Get())

//압축관련
#include "../compress/CompressRelease.h"
#include "../compress/DeCompressDupCheck.h"

#define theCompressRelease  (CCompressRelease::Get())
#define theDeCompressDupChk  (CDeCompressDupCheck::Get())

#endif // COMMON_H_INCLUDED
