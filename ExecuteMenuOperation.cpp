#include "ginc.h"
#include "ExecuteMenuOperation.h"
#include "MenuEvent.h"
#include "TabManager.h"
#include "ViewPanel.h"
#include "./interface/ContextManager.h"
#include "./view/FileListView.h"
#include "./view/PathView.h"

#include "./dialog/DlgAddDir.h"
#include "./dialog/DlgFind.h"
#include "./dialog/GenericDirDialog.h"
#include "./dialog/DirectoryManagerDlg.h"
#include "./dialog/DlgCompress.h"
#include "./dialog/DlgDeCompress.h"
#include "./dialog/DlgFavoriteManager.h"
#include "./dialog/DlgEnv.h"
#include "./dialog/wxMozillaLikeAboutBoxDialog.h"
#include "./dialog/header_png.h"
#include "./dialog/DlgDiskSizeView.h"
#include "./dialog/DlgResourceUsage.h"

CExecuteMenuOperation::CExecuteMenuOperation(CMenuEvent* pMenuEvent)
	: m_pMenuEvent(pMenuEvent)
{
	Init();
}

CExecuteMenuOperation::~CExecuteMenuOperation()
{

}

void CExecuteMenuOperation::Init()
{
	typedef struct stMenuList {
		wxString strMenuID;
		std::function<void()> funcPtr;
	} MENU_FNPTRLIST;

	MENU_FNPTRLIST stMenufnList[] = {
		{ wxT("m_fMenu_Execute"),	                std::bind(&CExecuteMenuOperation::FileMenu_Execution            , this) },
		{ wxT("m_fMenu_NewTab"),	                std::bind(&CExecuteMenuOperation::FileMenu_NewTab               , this) },
		{ wxT("m_fMenu_Copy"),	                    std::bind(&CExecuteMenuOperation::FileMenu_Copy	                , this) },
		{ wxT("m_fMenu_Move"),	                    std::bind(&CExecuteMenuOperation::FileMenu_Move                 , this) },
		{ wxT("m_fMenu_Rename"),	                std::bind(&CExecuteMenuOperation::FileMenu_Rename               , this) },
		{ wxT("m_fMenu_MakeFolder"),	            std::bind(&CExecuteMenuOperation::FileMenu_CreateFolder         , this) },
		{ wxT("m_fMenu_FileEdit"),	                std::bind(&CExecuteMenuOperation::FileMenu_Edit                 , this) },
		{ wxT("m_fMenu_goTrash"),	                std::bind(&CExecuteMenuOperation::FileMenu_Trash                , this) },
		{ wxT("m_fMenu_goDel"),	                    std::bind(&CExecuteMenuOperation::FileMenu_CompletionDel        , this) },
		{ wxT("m_fMenu_CMD"),	                    std::bind(&CExecuteMenuOperation::FileMenu_ExecCMD              , this) },
		{ wxT("m_fMenu_Exit"),	                    std::bind(&CExecuteMenuOperation::FileMenu_Terminate            , this) },
		//편집 메뉴
		{ wxT("m_eMenu_Release"),	                std::bind(&CExecuteMenuOperation::EditMenu_AllRelease	        , this) },
		{ wxT("m_eMenu_SelAll"),	                std::bind(&CExecuteMenuOperation::EditMenu_AllSelect            , this) },
		{ wxT("m_eMenu_Copy"),	                    std::bind(&CExecuteMenuOperation::EditMenu_Copy_UseClipboard    , this) },
		{ wxT("m_eMenu_Cut"),	                    std::bind(&CExecuteMenuOperation::EditMenu_Move_UseClipboard    , this) },
		{ wxT("m_eMenu_Paste"),        	            std::bind(&CExecuteMenuOperation::EditMenu_Paste_UseClipboard   , this) },
		{ wxT("m_eMenu_Search"),	                std::bind(&CExecuteMenuOperation::EditMenu_FileFind             , this) },
		{ wxT("m_eMenu_ContextMenu"),	            std::bind(&CExecuteMenuOperation::EditMenu_ViewContextMenu      , this) },
		//경로메뉴
		{ wxT("m_pMenu_PathMng"),	                std::bind(&CExecuteMenuOperation::PathMenu_DirManager           , this) },
		{ wxT("m_pMenu_Refresh"),	                std::bind(&CExecuteMenuOperation::PathMenu_Refresh              , this) },
		{ wxT("m_pMenu_Back"),	                    std::bind(&CExecuteMenuOperation::PathMenu_Back                 , this) },
		{ wxT("m_pMenu_Root"),	                    std::bind(&CExecuteMenuOperation::PathMenu_GoRoot               , this) },
		{ wxT("m_pMenu_GotoDirDirectly"), 	        std::bind(&CExecuteMenuOperation::PathMenu_GotoDirDirectly      , this) },
		//압축
		{ wxT("m_menuComp_Progress"),	            std::bind(&CExecuteMenuOperation::CompressMenu_DoCompress       , this ) },
		{ wxT("m_menuComp_CurrentDirCompRelease"),	std::bind(&CExecuteMenuOperation::CompressMenu_DeCompressCurDir , this ) },
		{ wxT("m_menuComp_MakeDirCompRelease"),	    std::bind(&CExecuteMenuOperation::CompressMenu_DeCompressMkDir  , this ) },
		{ wxT("m_menuComp_SelDirCompRelease"),	    std::bind(&CExecuteMenuOperation::CompressMenu_DeCompressSelDir , this ) },

		//보기메뉴
		{ wxT("m_viewMenu_FullScr"),	            std::bind(&CExecuteMenuOperation::ViewMenu_FullScreen           , this ) },
		{ wxT("m_viewMenu_Window_0"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Window_Single        , this ) },
		{ wxT("m_viewMenu_Window_1"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Window_Vert          , this ) },
		{ wxT("m_viewMenu_Window_2"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Window_Horz          , this ) },
		{ wxT("m_viewMenu_Column_0"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_0             , this ) },
		{ wxT("m_viewMenu_Column_1"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_1             , this ) },
		{ wxT("m_viewMenu_Column_2"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_2             , this ) },
		{ wxT("m_viewMenu_Column_3"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_3             , this ) },
		{ wxT("m_viewMenu_Column_4"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_4             , this ) },
		{ wxT("m_viewMenu_Column_5"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_5             , this ) },
		{ wxT("m_viewMenu_Column_6"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_6             , this ) },
		{ wxT("m_viewMenu_Column_7"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_7             , this ) },
		{ wxT("m_viewMenu_Column_8"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_8             , this ) },
		{ wxT("m_viewMenu_Column_9"),	            std::bind(&CExecuteMenuOperation::ViewMenu_Column_9             , this ) },
		{ wxT("m_viewMenu_DefaultFile"),	        std::bind(&CExecuteMenuOperation::ViewMenu_FileView_Default     , this ) },
		{ wxT("m_viewMenu_HiddenFile"),	            std::bind(&CExecuteMenuOperation::ViewMenu_FileView_Hidden      , this ) },
		{ wxT("m_viewMenu_AllFile"),	            std::bind(&CExecuteMenuOperation::ViewMenu_FileView_All         , this ) },
		{ wxT("m_viewMenu_SortName"),	            std::bind(&CExecuteMenuOperation::ViewMenu_SortDefault          , this ) },
		{ wxT("m_viewMenu_SortTime"),	            std::bind(&CExecuteMenuOperation::ViewMenu_SortTime             , this ) },
		{ wxT("m_viewMenu_SortSize"),	            std::bind(&CExecuteMenuOperation::ViewMenu_SortSize             , this ) },
		{ wxT("m_viewMenu_SubDir"),	                std::bind(&CExecuteMenuOperation::ViewMenu_SubDir               , this ) },
		{ wxT("m_viewMenu_DirNumber"),	            std::bind(&CExecuteMenuOperation::ViewMenu_DirNum               , this ) },
		//즐겨찾기
		{ wxT("m_favoriteMenu_Add"),	            std::bind(&CExecuteMenuOperation::FavoriteMenu_Add              , this ) },
		{ wxT("m_favoriteMenu_Manager"),	        std::bind(&CExecuteMenuOperation::FavoriteMenu_Manager          , this ) },

		//System 메뉴
		{ wxT("m_SystemMenu_Resource"),	            std::bind(&CExecuteMenuOperation::SystemMenu_ResourceUsage      , this ) },
		{ wxT("m_toolSystem_DiskSize"),	            std::bind(&CExecuteMenuOperation::SystemMenu_DiskUsage          , this ) },
		//도구메뉴
		{ wxT("m_toolMenu_ENV"),	                std::bind(&CExecuteMenuOperation::ToolMenu_Setting              , this) },
		{ wxT("m_toolMenu_CurSave"),	            std::bind(&CExecuteMenuOperation::ToolMenu_SaveSetting          , this) },
		//도움말
		{ wxT("m_helpMenu_thisProgram"),	        std::bind(&CExecuteMenuOperation::HelpMenu_ThisProgram          , this) },
	};

	for(unsigned int i = 0; i < WXSIZEOF(stMenufnList); i++)
	{
		int iMenuID = XRCID(stMenufnList[i].strMenuID);
		m_mapMenuFunctionPtr.insert(std::make_pair(iMenuID, stMenufnList[i].funcPtr));
	}
}

void CExecuteMenuOperation::ExecuteMenuOperation(int iMenuID, _MENU_EVENT_TYPE _mType)
{
	std::unordered_map< int, std::function<void()> >::const_iterator fIter = m_mapMenuFunctionPtr.find(iMenuID);
	if(fIter == m_mapMenuFunctionPtr.end())
		return;

	auto functionPtr = fIter->second;
	(functionPtr)();
}


void CExecuteMenuOperation::ExecuteMenuAnother(int _menuID)
{
	switch(_menuID)
	{
		case TOOLBAR_ID_DISK_USAGE:
			ViewDiskSize();
			break;

		case TOOLBAR_ID_RESOURCE_USAGE:
			ResourceUsage();
			break;

		default:
			break;
	}
}

//실행
void CExecuteMenuOperation::FileMenu_Execution()
{

}
//새탭
void CExecuteMenuOperation::FileMenu_NewTab()
{
	_gActivatedTab->AddTab(false);
}

//복사
void CExecuteMenuOperation::FileMenu_Copy()
{
	DoFileOperation(FILE_OP_COPY);
}

//이동
void CExecuteMenuOperation::FileMenu_Move()
{
	DoFileOperation(FILE_OP_CUT);
}

void CExecuteMenuOperation::DoFileOperation(FILE_OPERATION fop, bool IsUseClipboard)
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();

	wxString strTargetPath(pFileListView->GetCurrentPath());

	if(!IsUseClipboard)
		strTargetPath = GetDirectorySelect();

	std::list<wxString> lstSrc;
	if(!IsUseClipboard)
		pFileListView->GetFileOperationSrc(lstSrc);
	else
	{
		fop = theClipboard->GetFileOperation() == _MENU_EDIT_COPY_CLIPBOARD ? FILE_OP_COPY : FILE_OP_CUT;
		theClipboard->GetData(lstSrc);
	}

	if(lstSrc.size() == 0)
		return;

#ifdef __WXMSW__
	bool IsCopy = fop == FILE_OP_COPY ? true : false;
	bool bResult;
	if(IsUseClipboard)
	{
		wxString strSrcPath = theClipboard->GetSrcPath();
		if((strSrcPath.CmpNoCase(strTargetPath) == 0) && IsCopy)
			bResult = CLocalFileSystem::RecursiveCopySamePathForWindows(lstSrc, strTargetPath, nullptr);
		else
			bResult = CLocalFileSystem::RecursiveCopyMove(lstSrc, strTargetPath, nullptr, IsCopy);
	}
	else
		bResult = CLocalFileSystem::RecursiveCopyMove(lstSrc, strTargetPath, nullptr, IsCopy);

	if(bResult)
	{
		//Display Message
	}
#else

#endif

	pFileListView->SelectedItemsClear();
}

#ifdef __WXMSW__
static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData)
{
	switch (uMsg)
	{
		case BFFM_INITIALIZED:
		{
			//캡션에서 Help 삭제
			DWORD dwStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
			::SetWindowLong(hWnd, GWL_EXSTYLE, dwStyle & ~WS_EX_CONTEXTHELP);

			HWND hStatic = GetDlgItem(hWnd, 0x00003742);//14146); // 0x00003742(Static Control)
			if(hStatic)
			{
				::EnableWindow(hStatic, FALSE);
				::ShowWindow(hStatic, SW_HIDE);
			}

			//지정된 폴더가 선택되도록 한다.
			::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, dwData);

			//대화상자가 중앙에 오도록 한다.
			RECT rc;
			GetClientRect(hWnd, &rc);
			::SetWindowPos(hWnd
				    	 , NULL
					     , (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2
					     , (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2
					     , 0
					     , 0
					     , SWP_NOZORDER | SWP_NOSIZE);

			//Status text에 3D 테두리 첨가
			HWND hwndLabel = ::GetDlgItem(hWnd, 0x3743);
			if (IsWindow(hwndLabel))
			{
				TCHAR szClass[MAX_PATH] = {0};
				::GetClassName(hwndLabel, szClass, MAX_PATH);
				if (lstrcmpi(szClass, __TEXT("static")))
					break;
			}
			else
				break;

			dwStyle = ::GetWindowLong(hwndLabel, GWL_EXSTYLE);
			::SetWindowLong(hwndLabel, GWL_EXSTYLE, dwStyle | WS_EX_STATICEDGE);
			::SetWindowPos(hwndLabel, NULL, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME);
		}
			break;

		case BFFM_SELCHANGED:
		{
			TCHAR szText[MAX_PATH] = {0};
			::SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), szText);
			::SendMessage(hWnd, BFFM_SETSTATUSTEXT, 0, reinterpret_cast<LPARAM>(szText));
		}
			break;

		case BFFM_VALIDATEFAILED:
			break;
	}

	return 0;
}
#endif

wxString CExecuteMenuOperation::GetDirectorySelect()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	wxString strTargetPath(wxT(""));

	if(theJsonConfig->GetSplitStyle() == WINDOW_SPLIT_NONE)
	{
		strTargetPath = pFileListView->GetCurrentPath();
		bool bWindowSelectDir = false;
	#ifdef __WXMSW__
		if(theJsonConfig->GetUseSHBrowseForFolder())
		{
			bWindowSelectDir = true;
			LPITEMIDLIST pidlBrowse;
			TCHAR szDir[MAX_PATH] = {0, };

			BROWSEINFO brInfo;
			wxZeroMemory(brInfo);

			brInfo.hwndOwner = _gMainFrame->GetHWND();
			brInfo.pidlRoot = NULL;
			brInfo.pszDisplayName = szDir;
			brInfo.lpszTitle = _T("Select copy or move target directory");
			brInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_VALIDATE;
			brInfo.lpfn = BrowseCallbackProc;
			brInfo.lParam = (LPARAM)strTargetPath.wc_str();

			pidlBrowse = ::SHBrowseForFolder(&brInfo);
			if(pidlBrowse != NULL)
			{
				::SHGetPathFromIDList(pidlBrowse, szDir);
				strTargetPath = wxString::Format(wxT("%s"), szDir);
			}
		}
	#endif

		if(!bWindowSelectDir)
		{
			wxGenericDirDialog dlgTargetPath(_gMainFrame, wxT("Select target directory"), strTargetPath);
			dlgTargetPath.SetSize(wxSize(800, 600));
			dlgTargetPath.CenterOnScreen();

			if(dlgTargetPath.ShowModal() == wxID_OK)
				strTargetPath = dlgTargetPath.GetPath();

			dlgTargetPath.Destroy();
		}
	}
	else
	{
		pFileListView = (CFileListView *)_gContextManager->GetNotActivateViewItem();
		strTargetPath = pFileListView->GetCurrentPath();
	}

	return strTargetPath;
}

//이름변경
void CExecuteMenuOperation::FileMenu_Rename()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();

	wxCommandEvent evt(wxEVT_ITEM_RENAME);
	wxPostEvent(pFileListView, evt);
}

//폴더생성
void CExecuteMenuOperation::FileMenu_CreateFolder()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	wxString strPath(pFileListView->GetCurrentPath());

	DlgAddDir dlgDir(_gMainFrame);
	dlgDir.SetCurrentPath(strPath);
	dlgDir.ShowModal();
	dlgDir.Destroy();
}

//편집
void CExecuteMenuOperation::FileMenu_Edit()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();

	wxCommandEvent evt(wxEVT_FILE_EDIT);
	wxPostEvent(pFileListView, evt);
}

void CExecuteMenuOperation::FileMenu_Trash()
{
	DeleteProcess(_MENU_FILE_DEL_TRASH);
}

void CExecuteMenuOperation::FileMenu_CompletionDel()
{
	DeleteProcess(_MENU_FILE_DEL_COMP);
}

void CExecuteMenuOperation::DeleteProcess(_MENU_EVENT_TYPE mType)
{
	bool bTrash = (mType == _MENU_FILE_DEL_TRASH) ? true : false;
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();

	if(pFileListView)
	{
		std::list<wxString> lstSrc;
		pFileListView->GetFileOperationSrc(lstSrc);

		if(lstSrc.size() == 0)
			return;

		//별도 에러 처리는 하지 않음
		int nRet = 0;
		CLocalFileSystem::RecursiveDelete(lstSrc, _gMainFrame, bTrash, nRet);
		pFileListView->SelectedItemsClear();
		pFileListView->SetFocus();
	}
}

void CExecuteMenuOperation::FileMenu_ExecCMD()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	wxString strCurrentPath = pFileListView->GetCurrentPath();

	wxString strCommand(wxT(""));
#ifdef __WXMSW__
	wxString strVolume = strCurrentPath.Left(2);
	wxString strChildPath(wxT(""));
	size_t nSlash = 3;
	if(nSlash < strCurrentPath.Len())
		strChildPath = strCurrentPath.Mid(nSlash, strCurrentPath.Len());

	wxString strArgs(wxT(" /s /k "));
	if(!strChildPath.IsEmpty())
	{
		strArgs += strVolume;
		strArgs += wxT(" && cd ");
		strArgs += wxT("\"") + strCurrentPath + wxT("\"");
	}
	else
	{
		strArgs += strVolume;
		strArgs += wxT(" && cd \\");
	}

	strCommand = wxT("C:\\Windows\\System32\\CMD.exe") + strArgs;
#else

#endif

	wxExecute(strCommand);
}

void CExecuteMenuOperation::FileMenu_Terminate()
{
	wxTheApp->GetTopWindow()->Close();
}

//////////////////////////////////////////////// 편집메뉴 ///////////////////////////////////////////////////
//전체해제
void CExecuteMenuOperation::EditMenu_AllRelease()
{
	DoItemSelectProcess(_MENU_EDIT_ALL_RELEASE);
}
//전체선택
void CExecuteMenuOperation::EditMenu_AllSelect()
{
	DoItemSelectProcess(_MENU_EDIT_ALL_SELECT);
}

void CExecuteMenuOperation::DoItemSelectProcess(_MENU_EVENT_TYPE mType)
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	pFileListView->AllItemSelectOrRelease(mType);
}

//클립보드 복사
void CExecuteMenuOperation::EditMenu_Copy_UseClipboard()
{
	SetFileOperationClipboardData(false);
}

//클립보드 이동
void CExecuteMenuOperation::EditMenu_Move_UseClipboard()
{
	SetFileOperationClipboardData(true);
}

//클립보드 붙여넣기
void CExecuteMenuOperation::EditMenu_Paste_UseClipboard()
{
	DoFileOperation(FILE_OP_NONE, true);
}

void CExecuteMenuOperation::SetFileOperationClipboardData(bool IsCut)
{
	std::list<wxString> lstSrc;
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	pFileListView->GetFileOperationSrc(lstSrc, IsCut);

	if(lstSrc.size() > 0)
	{
		theClipboard->Clear();
		theClipboard->SetFileOperation(IsCut ? _MENU_EDIT_MOVE_CLIPBOARD : _MENU_EDIT_COPY_CLIPBOARD);
		theClipboard->AddData(lstSrc);
		theClipboard->CopyToClipboard();

#ifdef __WXMSW__
		theClipboard->SetSrcPath(pFileListView->GetCurrentPath());
#endif // __WXMSW__
	}
}

void CExecuteMenuOperation::EditMenu_FileFind()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	wxString strCurrentPath = pFileListView->GetCurrentPath();

	DlgFind findDlg(_gMainFrame);
	findDlg.setSearchDir(strCurrentPath);
	findDlg.ShowModal();
	findDlg.Destroy();
}

//컨택스트 메뉴 보기
void CExecuteMenuOperation::EditMenu_ViewContextMenu()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();

	wxCommandEvent evt(wxEVT_VIEW_CONTEXTMENU);
	wxPostEvent(pFileListView, evt);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//경로메뉴
void CExecuteMenuOperation::PathMenu_DirManager()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	wxString strCurrentPath = pFileListView->GetCurrentPath();

	wxString strTreeDataPath = theUtility->GetWorkDir() + SLASH + wxT("treedata");
	if (!wxDirExists(strTreeDataPath))
		wxMkDir(strTreeDataPath);

	wxString strSHCDFilePath = strTreeDataPath + SLASH + strCurrentPath.Left(1) + wxT(".SCD");
	DirectoryManagerDlg dirMng(_gMainFrame);
	dirMng.SetSCDFileInfo(strSHCDFilePath, strCurrentPath);
	dirMng.ShowModal();
	dirMng.Destroy();
}

void CExecuteMenuOperation::PathMenu_Refresh()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	wxString strCurrentPath = pFileListView->GetCurrentPath();

	wxCommandEvent evt(wxEVT_CHANGE_DIRECTORY);
	evt.SetString(strCurrentPath);
	evt.SetInt(SEND_PATH_TO_FILELIST);

	wxWindow* pActivateViewPanel = _gContextManager->GetActivatePanel(_gActivatedTab);
	wxPostEvent(pActivateViewPanel, evt);
}

void CExecuteMenuOperation::PathMenu_Back()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();

	wxCommandEvent evt(wxEVT_EXEC_MENU_CLICK);
	evt.SetInt(_MENU_PATH_BACK);

	wxPostEvent(pFileListView, evt);

}

void CExecuteMenuOperation::PathMenu_GoRoot()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();

	wxCommandEvent evt(wxEVT_EXEC_MENU_CLICK);
	evt.SetInt(_MENU_PATH_GOROOT);

	wxPostEvent(pFileListView, evt);
}

void CExecuteMenuOperation::PathMenu_GotoDirDirectly()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	wxString strCurrentPath = pFileListView->GetCurrentPath();

	wxGenericDirDialog dlgTargetPath(_gMainFrame, wxT("Select target directory"), strCurrentPath);
	dlgTargetPath.SetSize(wxSize(600, 450));
	dlgTargetPath.CenterOnScreen();

	if(dlgTargetPath.ShowModal() == wxID_OK)
	{
		wxString strTargetDir(dlgTargetPath.GetPath());

		wxCommandEvent evt(wxEVT_CHANGE_DIRECTORY);
		evt.SetString(strTargetDir);
		evt.SetInt(SEND_PATH_TO_FILELIST);

		wxWindow* pActivateViewPanel = _gContextManager->GetActivatePanel(_gActivatedTab);
		wxPostEvent(pActivateViewPanel, evt);
	}

	dlgTargetPath.Destroy();
}

//압축
void CExecuteMenuOperation::CompressMenu_DoCompress()
{
	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();

	wxCommandEvent event(wxEVT_SHOW_COMPRESS_MENU);
	wxPostEvent(pFileListView, event);
}

void CExecuteMenuOperation::CompressMenu_ExecuteCompress(int nId, const wxString& strCompressedFile)
{
	int nIndex = nId - COMPRESS_START_ID;

	wxString strCompressedExt = theCompressRelease->GetCompressList().at(nIndex);

	std::vector<wxString> vecItems;

	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	pFileListView->GetSelectedCompressedItems(vecItems);

	pFileListView->SelectedItemsClear();

	DlgCompress dlg(_gMainFrame);
	dlg.SetCompressInfo(vecItems, strCompressedFile, strCompressedExt);
	dlg.ShowModal();
	dlg.Destroy();
}

void CExecuteMenuOperation::CompressMenu_DeCompressCurDir()
{
	DoDecompress(_MENU_DECOMPRESS_CURRENT_DIR);
}

void CExecuteMenuOperation::CompressMenu_DeCompressMkDir()
{
	DoDecompress(_MENU_DECOMPRESS_MK_FOLDER);
}

void CExecuteMenuOperation::CompressMenu_DeCompressSelDir()
{
	DoDecompress(_MENU_DECOMPRESS_SEL_DIR);
}

void CExecuteMenuOperation::DoDecompress(_MENU_EVENT_TYPE _menuType)
{
	std::vector<wxString> vecItems;

	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	wxString strCurrentPath = pFileListView->GetCurrentPath();
	pFileListView->GetSelectedCompressedItems(vecItems);

	int nItemCount = vecItems.size();

	wxString strSelItem = vecItems.at(nItemCount - 1);
	wxString strExt = theUtility->GetExt(strSelItem);
	if(!theCompressRelease->IsCompressedFile(strExt))
	{
		wxMessageBox(wxT("The selected file is not compressed file"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}

	if(_menuType == _MENU_DECOMPRESS_SEL_DIR)
	{
		wxGenericDirDialog dlgTargetPath(_gMainFrame, wxT("Select target directory"), strCurrentPath);
		dlgTargetPath.SetSize(wxSize(600, 450));
		dlgTargetPath.CenterOnScreen();

		if(dlgTargetPath.ShowModal() != wxID_OK)
			return;

		strCurrentPath = dlgTargetPath.GetPath();
		dlgTargetPath.Destroy();
	}

	DlgDeCompress dlg(_gMainFrame);
	dlg.SetDecompressInfo(strSelItem, strCurrentPath, strExt.MakeLower(), _menuType);
	dlg.ShowModal();
	dlg.Destroy();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//보기메뉴
void CExecuteMenuOperation::ViewMenu_FullScreen()
{
	_gMainFrame->Maximize();
}

void CExecuteMenuOperation::ViewMenu_Window_Single()
{
	ChangeViewStyle(WINDOW_SPLIT_NONE);
}

void CExecuteMenuOperation::ViewMenu_Window_Vert()
{
	ChangeViewStyle(WINDOW_SPLIT_VERT);
}

void CExecuteMenuOperation::ViewMenu_Window_Horz()
{
	ChangeViewStyle(WINDOW_SPLIT_HORZ);
}

void CExecuteMenuOperation::ChangeViewStyle(int nViewStyle)
{
	theJsonConfig->SetChangeGlobalVal(wxT("SplitWindowStyle"), nViewStyle);
	theSplitterManager->SplitWindow();

	CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
	pFileListView->SetFocus();
}

void CExecuteMenuOperation::ViewMenu_Column_0()
{
	ChangeViewColumn(VIEW_COLUMN_AUTO);
}

void CExecuteMenuOperation::ViewMenu_Column_1()
{
	ChangeViewColumn(VIEW_COLUMN_ONE);
}

void CExecuteMenuOperation::ViewMenu_Column_2()
{
	ChangeViewColumn(VIEW_COLUMN_TWO);
}

void CExecuteMenuOperation::ViewMenu_Column_3()
{
	ChangeViewColumn(VIEW_COLUMN_THREE);
}

void CExecuteMenuOperation::ViewMenu_Column_4()
{
	ChangeViewColumn(VIEW_COLUMN_FOUR);
}

void CExecuteMenuOperation::ViewMenu_Column_5()
{
	ChangeViewColumn(VIEW_COLUMN_FIVE);
}

void CExecuteMenuOperation::ViewMenu_Column_6()
{
	ChangeViewColumn(VIEW_COLUMN_SIX);
}

void CExecuteMenuOperation::ViewMenu_Column_7()
{
	ChangeViewColumn(VIEW_COLUMN_SEVEN);
}

void CExecuteMenuOperation::ViewMenu_Column_8()
{
	ChangeViewColumn(VIEW_COLUMN_EIGHT);
}

void CExecuteMenuOperation::ViewMenu_Column_9()
{
	ChangeViewColumn(VIEW_COLUMN_NINE);
}

void CExecuteMenuOperation::ChangeViewColumn(COLUMN_VIEW_OPTION columnOP)
{
	theJsonConfig->SetChangeGlobalVal(wxT("DispColumn"), columnOP);
	_gContextManager->ChangeColumn();
}

void CExecuteMenuOperation::ViewMenu_FileView_Default()
{
	_gAttrIndex = 0;
	ChangeFileList(FILE_VIEW_OP_DEFAULT);
}

void CExecuteMenuOperation::ViewMenu_FileView_Hidden()
{
	_gAttrIndex = 1;
	ChangeFileList(FILE_VIEW_OP_HIDDEN);
}

void CExecuteMenuOperation::ViewMenu_FileView_All()
{
	_gAttrIndex = 2;
	ChangeFileList(FILE_VIEW_OP_ALL);
}

void CExecuteMenuOperation::ViewMenu_SortDefault()
{
	ChangeViewSort(VIEW_SORT_DEFAULT);
}

void CExecuteMenuOperation::ViewMenu_SortTime()
{
	ChangeViewSort(VIEW_SORT_TIME);
}

void CExecuteMenuOperation::ViewMenu_SortSize()
{
	ChangeViewSort(VIEW_SORT_SIZE);
}

void CExecuteMenuOperation::ViewMenu_SubDir()
{

}

// 2023.05.26 Add Start *****************************************
void CExecuteMenuOperation::ViewMenu_DirNum()
{
	_gContextManager->ViewDirectoryNumbering();
}
// 2023.05.26 Add End   *****************************************

void CExecuteMenuOperation::ChangeFileList(FILE_VIEW_OPTION viewOP)
{
	theJsonConfig->SetViewFileKind(viewOP);
	_gContextManager->ChangeFileList();
}

void CExecuteMenuOperation::ChangeViewSort(VIEW_SORT sType)
{
	theJsonConfig->SetChangeGlobalVal(wxT("SortType"), sType);
	_gContextManager->ChangeFileListSort();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//즐겨찾기
void CExecuteMenuOperation::FavoriteMenu_Add()
{
	BookmarkOperation(_MENU_FAVORITE_ITEM_ADD);
}

void CExecuteMenuOperation::FavoriteMenu_Manager()
{
	BookmarkOperation(_MENU_FAVORITE_MANAGER);
}

void CExecuteMenuOperation::FavoriteMenu_ItemSelected(int iMenuID)
{

}

void CExecuteMenuOperation::BookmarkOperation(_MENU_EVENT_TYPE mType)
{
	DlgFavoriteManager dlg(_gMainFrame);
	if (mType == _MENU_FAVORITE_ITEM_ADD)
	{
		CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
		wxString strCurrentPath = pFileListView->GetCurrentPath();

		dlg.SetAddPath(strCurrentPath, true);
	}

	dlg.ShowModal();
	dlg.Destroy();

	m_pMenuEvent->LoadBookmarkToMenu(true);
}

//System Menu
void CExecuteMenuOperation::SystemMenu_ResourceUsage()
{
	ResourceUsage();
}

void CExecuteMenuOperation::SystemMenu_DiskUsage()
{
	ViewDiskSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//도구메뉴
void CExecuteMenuOperation::ToolMenu_Setting()
{
	DlgEnv dlg(_gMainFrame);
	dlg.ShowModal();
	dlg.Destroy();
}

void CExecuteMenuOperation::ToolMenu_SaveSetting()
{
	int xPos = 0;
	int yPos = 0;

	int nCurrDisp = wxDisplay::GetFromWindow(_gMainFrame);

	wxDisplay disp(nCurrDisp);
	wxRect rcMonitor = disp.GetGeometry();

	wxTopLevelWindow* pTopWindow = (wxTopLevelWindow *)wxTheApp->GetTopWindow();
	pTopWindow->GetScreenPosition(&xPos, &yPos);
	wxRect rcScreen = pTopWindow->GetScreenRect();

	int iMaximized = _gMainFrame->IsMaximized() ? 1 : 0;

	if (rcMonitor.GetLeft() != 0)
		xPos = rcMonitor.GetLeft();

	int iWidth = rcScreen.GetWidth();
	int iHeight = rcScreen.GetHeight();

	theJsonConfig->SetChangeGlobalVal(wxT("Maximized"), iMaximized);
	theJsonConfig->SetChangeGlobalVal(wxT("DispMonitor"), nCurrDisp);
	theJsonConfig->SetChangeGlobalVal(wxT("XPos"), xPos);
	theJsonConfig->SetChangeGlobalVal(wxT("YPos"), yPos);
	theJsonConfig->SetChangeGlobalVal(wxT("Width"), iWidth);
	theJsonConfig->SetChangeGlobalVal(wxT("Height"), iHeight);

	if (theJsonConfig->IsSaveLastSession())
	{
		CFileListView* pFileListView = (CFileListView *)_gContextManager->GetActivateViewItem();
		CFileListView* pNonActiveView = nullptr;

		if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
			pNonActiveView = (CFileListView *)_gContextManager->GetNotActivateViewItem();

		wxString strActiveKey(wxT(""));
		wxString strAnotherKey(wxT(""));

		bool bLeftTab = _gActivatedTab->IsLeftTab() ? true : false;
		if(bLeftTab)
		{
			strActiveKey = wxT("LeftPath");
			strAnotherKey = wxT("RightPath");
		}
		else
		{
			strActiveKey = wxT("RightPath");
			strAnotherKey = wxT("LeftPath");
		}

		wxString strCurrentPath = pFileListView->GetCurrentPath();
		theJsonConfig->SetChangeGlobalVal(strActiveKey, strCurrentPath);

		if(pNonActiveView)
		{
			strCurrentPath = pNonActiveView->GetCurrentPath();
			theJsonConfig->SetChangeGlobalVal(strAnotherKey, strCurrentPath);
		}
	}

	theJsonConfig->SaveConfig();
	wxMessageBox(wxT("The Current settings have been saved."), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
}

//도움말
void CExecuteMenuOperation::HelpMenu_ThisProgram()
{
	// Create About box
	wxMozillaLikeAboutBoxDialog * dlg = new wxMozillaLikeAboutBoxDialog(_gMainFrame);

	// Set application name
	dlg->SetAppName(PROGRAM_NAME);
	// Set application version
	dlg->SetVersion(PROGRAM_MAJOR_VER + wxT(".") + PROGRAM_MINOR_HI_VER + wxT(".") + PROGRAM_MINOR_LO_VER);
	// Set copyright message
	wxDateTime dt = wxDateTime::Now();
	int iYear = dt.GetYear();
	wxString strCopyRight = wxString::Format(theMsgManager->GetMessage(wxT("MSG_PROGRAMM_COPYRIGHT")), iYear);
	dlg->SetCopyright(strCopyRight);
	// Set header bitmap
	//	dlg->SetHeaderBitmap(wxGetBitmapFromMemory(header_png, sizeof(header_png)));

	wxString strBuildDisp(wxT(""));
	wxString strBuildInfo(wxMozillaLikeAboutBoxDialog::GetBuildInfo(wxMozillaLikeAboutBoxDialog::wxBUILDINFO_LONG).GetData());
	wxString strDeveloper(wxT("Developed by "));
	strDeveloper += theMsgManager->GetMessage(wxT("MSG_PROGRAMM_DEVELOPER"));

	strBuildInfo = wxString::Format(wxT("%s. \n%s"), strBuildInfo, strDeveloper);
	// Set build info message. This is optional step. If you don't specify build info message then
	// default one will be used
	dlg->SetCustomBuildInfo(wxT("Advanced File Manager ") + strBuildInfo);
	// Apply changes
	dlg->ApplyInfo();
	// Show dialog
	dlg->ShowModal();
	// Destroy dialog
	dlg->Destroy();
}

void CExecuteMenuOperation::ResourceUsage()
{
	int w = 0;
	int h = 0;

	_gMainFrame->GetSize(&w, &h);

	w -= 400;
	h -= 160;


	DlgResourceUsage dlg(_gMainFrame, wxSize(w, h));
	dlg.ShowModal();
	dlg.Destroy();
}

void CExecuteMenuOperation::ViewDiskSize()
{
	int w = 0;
	int h = 0;

	_gMainFrame->GetSize(&w, &h);

	w -= 650;
	h -= 160;

	DlgDiskSizeView dlg(_gMainFrame, wxSize(w, h));
	dlg.ShowModal();
	dlg.Destroy();
}
