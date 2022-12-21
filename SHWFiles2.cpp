#include "ginc.h"
#include "SHWFiles2.h"
#include "MainFrame.h"
#include <wx/fontenum.h>

wxIMPLEMENT_APP(CSHWFiles2);

CSHWFiles2::CSHWFiles2()
{

}

CSHWFiles2::~CSHWFiles2()
{

}

bool CSHWFiles2::OnInit()
{
	if (!wxApp::OnInit())
	{
		wxMessageBox(wxT("Windows Initialize failed"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

#ifdef __WXMSW__
	::OleInitialize(0);
#endif

#ifdef NDEBUG
	m_pSingleInstance = new wxSingleInstanceChecker;
	if (m_pSingleInstance->IsAnotherRunning())
	{
		delete m_pSingleInstance;
		m_pSingleInstance = NULL;

		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_ALREADY_EXEC_PROCESS")), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return false;
	}
#endif

	if (!Initialize())
		return false;

	CMainFrame* pMainFrame = new CMainFrame(m_strTitle);
	pMainFrame->Show();

	_gMainFrame = pMainFrame;
	this->SetTopWindow(pMainFrame);

	return true;
}

int CSHWFiles2::OnExit()
{
#ifdef NDEBUG
	wxDELETE(m_pSingleInstance);
#endif

#ifdef __WXMSW__
	::OleUninitialize();

	if (_gImageList != NULL)
		ImageList_Destroy(_gImageList);

	if (m_bAddFontRC)
	{
		wxString strCurrentDir(theUtility->GetWorkDir());

		wxString strFontPath(wxT(""));
		strFontPath = strCurrentDir + SLASH + wxT("rc") + SLASH + wxT("hvgafix.fon");// wxT("FSEX300.ttf");
		BOOL bRemove = RemoveFontResource(strFontPath);
		if (bRemove == FALSE)
			wxMessageBox(wxT("Error Resource Remove"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
	}

#else
	_gImageList->RemoveAll();

#endif // __WXMSW__

	_gImageList = nullptr;
	_gmapAllowProcessKey.clear();

	wxDELETE(_gViewFont);
	wxDELETE(_gContextManager);
	return 0;
}

bool CSHWFiles2::Initialize()
{
	_gContextManager = new CContextManager();

	_gmapAllowProcessKey.insert(std::make_pair(WXK_F1, WXK_F1));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F2, WXK_F2));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F3, WXK_F3));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F4, WXK_F4));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F5, WXK_F5));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F6, WXK_F6));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F7, WXK_F7));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F8, WXK_F8));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F9, WXK_F9));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F10, WXK_F10));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F11, WXK_F11));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F12, WXK_F12));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F13, WXK_F13));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F14, WXK_F14));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F15, WXK_F15));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F16, WXK_F16));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F17, WXK_F17));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F18, WXK_F18));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F19, WXK_F19));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F20, WXK_F20));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F21, WXK_F21));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F22, WXK_F22));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F23, WXK_F23));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F24, WXK_F24));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_NUMLOCK, WXK_NUMLOCK));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_F4, WXK_F4));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_REVERSE_SLASH, WXK_REVERSE_SLASH));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_TAB, WXK_TAB));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_SPACE, WXK_SPACE));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_INSERT, WXK_INSERT));
	_gmapAllowProcessKey.insert(std::make_pair(wxKEY_DOT, wxKEY_DOT));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_BACK, WXK_BACK));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_RETURN, WXK_RETURN));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_LEFT, WXK_LEFT));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_RIGHT, WXK_RIGHT));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_UP, WXK_UP));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_DOWN, WXK_DOWN));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_PAGEDOWN, WXK_PAGEDOWN));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_PAGEUP, WXK_PAGEUP));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_HOME, WXK_HOME));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_END, WXK_END));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_LEFT_M_G, WXK_LEFT_M_G));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_RIGHT_M_G, WXK_RIGHT_M_G));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_V_LINE, WXK_V_LINE));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_WINDOWS_KEY, WXK_WINDOWS_KEY));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_ESCAPE, WXK_ESCAPE));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_WINDOWS_LEFT, WXK_WINDOWS_LEFT));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_WINDOWS_RIGHT, WXK_WINDOWS_RIGHT));
	_gmapAllowProcessKey.insert(std::make_pair(WXK_WINDOWS_MENU, WXK_WINDOWS_MENU));

	//Read System Language
	_gSystemLang = theUtility->GetSystemLang();

	if (!InitConfig())
		return false;

	//확장자 명령 설정 셋팅
	wxString strCurrentDir(theUtility->GetWorkDir());

	//이미지리스트
	theUtility->LoadImageList();

#ifdef __WXMSW__
	//디렉토리 트리 탐색시 무시 항목리스트
	_gVecIgnore.push_back("");
	_gVecIgnore.push_back(".");
	_gVecIgnore.push_back("..");
	_gVecIgnore.push_back(".git");
	_gVecIgnore.push_back("winsxs");
	_gVecIgnore.push_back("WinSxS");

	theDriveInfo->Init();

	wxString strFontName(wxT(""));
	int nFontSize = 0;

	strFontName = theJsonConfig->GetFontName();
	nFontSize = theJsonConfig->GetFontSize();

	//폰트 설정
	wxString strFixedsysFontPath(wxT(""));
	strFixedsysFontPath = strCurrentDir + SLASH + wxT("rc") + SLASH + wxT("hvgafix.fon");// wxT("FSEX300.ttf");

	//등록하고자 하는 폰트가 존재하는지 체크
	wxArrayString arrFontList = wxFontEnumerator::GetFacenames();
	bool bFontExist = false;
	for (auto item : arrFontList)
	{
		if (item.Cmp(wxT("Fixedsys")) == 0)
		{
			bFontExist = true;
			break;
		}
	}

	if (!bFontExist)
	{
		int iErr = AddFontResource(CONVSTR(strFixedsysFontPath));
		if (iErr != 0)
			m_bAddFontRC = true;
	}

	_gViewFont = new wxFont(nFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, strFontName);
#else
	_gViewFont = new wxFont(wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT));
#endif // __WXMSW__

	_gNormalFolderIco = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, wxSize(16, 16));
	_gNormalFileIco   = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));
	_gExternalPG      = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 16));

	//메뉴읽기
	wxXmlResource::Get()->InitAllHandlers();
	wxString strMenuXRC = wxT("menus_en.xrc");

	if (_gSystemLang.Cmp(wxT("ko")) == 0)
		strMenuXRC = wxT("menus_ko.xrc");
	else
		strMenuXRC = wxT("menus_en.xrc");

	wxString strXrcPath = strCurrentDir + SLASH + wxT("xrc") + SLASH + strMenuXRC;
	bool bXRCLoaded = wxXmlResource::Get()->Load(strXrcPath);

	if (!bXRCLoaded)
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_XRC_MENU_LOAD_FAILED")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	//디렉토리의 경우 유형은 "파일 폴더" 로 고정
	std::unordered_map<wxString, wxString>::const_iterator typeIter = _gCacheFileType.find(wxT("dir"));
	if (typeIter == _gCacheFileType.end())
	{
		std::unordered_map<wxString, wxString>::value_type val(wxT("dir"), theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION")));
		_gCacheFileType.insert(val);
	}

	m_strTitle = PROGRAM_FULL_NAME + ENV_OS_VER;

#ifndef NDEBUG
	m_strTitle += wxT(" - Debug Version");
#endif

	m_strTitle += wxT(" Build-Date : 2022-12-21");

	_gAttrIndex = 0;
	if(theJsonConfig->IsViewHiddenFile())
		_gAttrIndex = 1;

	theSystem->Init();
	return true;
}

bool CSHWFiles2::InitConfig()
{
	if (!theJsonConfig->Load())
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_INITFAIL_WINDOW_SETTING")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	if (!theMsgManager->Load())
	{
		wxMessageBox(wxT("The Message config load failed"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	if (!theBookmark->Load())
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_INITFAIL_WINDOW_BOOKMARK")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	if (!theExtInfo->Load())
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_INITFAIL_WINDOW_EXTINFO")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

#ifdef __WXMSW__
	if(!theIconInfo->Load())
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("Read error of FileIconInfo jsonfile")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}
#endif // __WXMSW__

	return true;
}
