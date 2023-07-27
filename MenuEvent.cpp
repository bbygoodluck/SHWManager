#include "ginc.h"
#include "MainFrame.h"
#include "MenuEvent.h"
#include "ExecuteMenuOperation.h"

wxBEGIN_EVENT_TABLE(CMenuEvent, wxEvtHandler)
	EVT_MENU(wxID_ANY, CMenuEvent::OnMenuEvent)
	EVT_UPDATE_UI(wxID_ANY, CMenuEvent::OnMenuEventUpdate)
	EVT_MY_CUSTOM_COMMAND(wxEVT_COMPRESS_EXEC, wxID_ANY, CMenuEvent::OnCompressExecute)
wxEND_EVENT_TABLE()

CMenuEvent::CMenuEvent(CMainFrame* _pMainFrame)
	: m_pMainFrame(_pMainFrame)
{
	m_pMenuOperation = new CExecuteMenuOperation(this);
	Init();
}

CMenuEvent::~CMenuEvent()
{
	wxDELETE(m_pMenuOperation);
}

void CMenuEvent::Init()
{
	typedef struct stMenuList {
		wxString strMenuID;
		_MENU_EVENT_TYPE mType = _MENU_EVENT_NONE;
	} MENU_LIST;

	MENU_LIST mList[] = {
		{ wxT("m_fMenu_Execute"),	                _MENU_FILE_EXECUTION },
		{ wxT("m_fMenu_NewTab"),	                _MENU_FILE_NEW_TAB },
		{ wxT("m_fMenu_Copy"),	                    _MENU_FILE_COPY },
		{ wxT("m_fMenu_Move"),	                    _MENU_FILE_MOVE },
		{ wxT("m_fMenu_Rename"),	                _MENU_FILE_RENAME },
		{ wxT("m_fMenu_MakeFolder"),	            _MENU_FILE_ADD_DIR },
		{ wxT("m_fMenu_FileEdit"),	                _MENU_FILE_EDIT },
		{ wxT("m_fMenu_goTrash"),	                _MENU_FILE_DEL_TRASH },
		{ wxT("m_fMenu_goDel"),	                    _MENU_FILE_DEL_COMP },
		{ wxT("m_fMenu_CMD"),	                    _MENU_FILE_CMD_EXEC },
		{ wxT("m_fMenu_Exit"),	                    _MENU_FILE_PROGRAM_TERMINATE },
		{ wxT("m_eMenu_Release"),	                _MENU_EDIT_ALL_RELEASE },
		{ wxT("m_eMenu_SelAll"),	                _MENU_EDIT_ALL_SELECT },
		{ wxT("m_eMenu_Copy"),	                    _MENU_EDIT_COPY_CLIPBOARD },
		{ wxT("m_eMenu_Cut"),	                    _MENU_EDIT_MOVE_CLIPBOARD },
		{ wxT("m_eMenu_Paste"),        	            _MENU_EDIT_PASTE_CLIPBOARD },
		{ wxT("m_eMenu_Search"),	                _MENU_EDIT_FIND },
		{ wxT("m_eMenu_ContextMenu"),	            _MENU_EDIT_CONTEXT_MENU },
		{ wxT("m_pMenu_PathMng"),	                _MENU_PATH_DIR_MANAGER },
		{ wxT("m_pMenu_Refresh"),	                _MENU_PATH_REFRESH },
		{ wxT("m_pMenu_Back"),	                    _MENU_PATH_BACK },
		{ wxT("m_pMenu_Root"),	                    _MENU_PATH_GOROOT },
		{ wxT("m_pMenu_GotoDirDirectly"), 	        _MENU_PATH_GOTO_DIR_DIRECTLY },
		{ wxT("m_menuComp_View"),	                _MENU_COMPRESS_FILE_VIEW },
		{ wxT("m_menuComp_Progress"),	            _MENU_COMPRESS_DO_COMPRESS },
		{ wxT("m_menuComp_CurrentDirCompRelease"),	_MENU_DECOMPRESS_CURRENT_DIR },
		{ wxT("m_menuComp_MakeDirCompRelease"),	    _MENU_DECOMPRESS_MK_FOLDER },
		{ wxT("m_menuComp_SelDirCompRelease"),	    _MENU_DECOMPRESS_SEL_DIR },
		{ wxT("m_viewMenu_FullScr"),	            _MENU_VIEW_FULLSCREEN },
		{ wxT("m_viewMenu_Window_0"),	            _MENU_VIEW_WINDOW_SINGLE },
		{ wxT("m_viewMenu_Window_1"),	            _MENU_VIEW_WINDOW_VERT },
		{ wxT("m_viewMenu_Window_2"),	            _MENU_VIEW_WINDOW_HORZ },
		{ wxT("m_viewMenu_ToolBar"),	            _MENU_VIEW_TOOLBAR },
		{ wxT("m_viewMenu_ShortCut"),	            _MENU_VIEW_SHORTCUT },
		{ wxT("m_viewMenu_Column_0"),	            _MENU_VIEW_COLUMN_0 },
		{ wxT("m_viewMenu_Column_1"),	            _MENU_VIEW_COLUMN_1 },
		{ wxT("m_viewMenu_Column_2"),	            _MENU_VIEW_COLUMN_2 },
		{ wxT("m_viewMenu_Column_3"),	            _MENU_VIEW_COLUMN_3 },
		{ wxT("m_viewMenu_Column_4"),	            _MENU_VIEW_COLUMN_4 },
		{ wxT("m_viewMenu_Column_5"),	            _MENU_VIEW_COLUMN_5 },
		{ wxT("m_viewMenu_Column_6"),	            _MENU_VIEW_COLUMN_6 },
		{ wxT("m_viewMenu_Column_7"),	            _MENU_VIEW_COLUMN_7 },
		{ wxT("m_viewMenu_Column_8"),	            _MENU_VIEW_COLUMN_8 },
		{ wxT("m_viewMenu_Column_9"),	            _MENU_VIEW_COLUMN_9 },
		{ wxT("m_viewMenu_DefaultFile"),	        _MENU_VIEW_FILE_DEFAULT },
		{ wxT("m_viewMenu_HiddenFile"),	            _MENU_VIEW_FILE_HIDDEN },
		{ wxT("m_viewMenu_AllFile"),	            _MENU_VIEW_FILE_ALL },
		{ wxT("m_viewMenu_SortName"),	            _MENU_VIEW_SORT_DEFAULT },
		{ wxT("m_viewMenu_SortTime"),	            _MENU_VIEW_SORT_TIME },
		{ wxT("m_viewMenu_SortSize"),	            _MENU_VIEW_SORT_SIZE },
		{ wxT("m_viewMenu_SubDir"),	                _MENU_VIEW_SUBDIR },
		{ wxT("m_viewMenu_DirNumber"),	            _MENU_VIEW_DIR_NUMBER },
		{ wxT("m_favoriteMenu_Add"),	            _MENU_FAVORITE_ITEM_ADD },
		{ wxT("m_favoriteMenu_Manager"),	        _MENU_FAVORITE_MANAGER },
		{ wxT("m_SystemMenu_Resource"),	            _MENU_SYSTEM_RESOURCE_USAGE },
		{ wxT("m_toolSystem_DiskSize"),	            _MENU_SYSTEM_DISK_USAGE },
		{ wxT("m_toolMenu_ENV"),	                _MENU_TOOL_ENVIRONMENT },
		{ wxT("m_toolMenu_CurSave"),	            _MENU_TOOL_SAVECONFIG },
		{ wxT("m_helpMenu_thisProgram"),	        _MENU_HELP_THIS_PROGRAM }
	};

	for(unsigned int i = 0; i < WXSIZEOF(mList); i++)
	{
		int iMenuID = XRCID(mList[i].strMenuID);
		_MENU_EVENT_TYPE mType = mList[i].mType;

		m_mapMenu.insert(std::make_pair(iMenuID, mType));
	}
}

void CMenuEvent::SetMainFrame(CMainFrame* pMainFrame)
{
	if(m_pMainFrame != nullptr)
		return;

	m_pMainFrame = pMainFrame;
}

void CMenuEvent::OnMenuEvent(wxCommandEvent& event)
{
	int _iMenuID = event.GetId();
	DoMenuOperation(_iMenuID);
}

void CMenuEvent::OnMenuEventUpdate(wxUpdateUIEvent& event)
{
	if(event.IsCheckable())
		event.Check(false);

	int _iupdateID = event.GetId();
	//윈도우창
	if ((_iupdateID == XRCID("m_viewMenu_Window_0")) ||	//단일창
		(_iupdateID == XRCID("m_viewMenu_Window_1")) ||	//수직창
		(_iupdateID == XRCID("m_viewMenu_Window_2")) ||	//수평창
		(_iupdateID == XRCID("m_viewMenu_Column_0")) ||	//컬럼자동
		(_iupdateID == XRCID("m_viewMenu_Column_1")) ||	//컬럼1
		(_iupdateID == XRCID("m_viewMenu_Column_2")) ||	//컬럼2
		(_iupdateID == XRCID("m_viewMenu_Column_3")) ||	//컬럼3
		(_iupdateID == XRCID("m_viewMenu_Column_4")) ||	//컬럼4
		(_iupdateID == XRCID("m_viewMenu_Column_5")) ||	//컬럼5
		(_iupdateID == XRCID("m_viewMenu_Column_6")) ||	//컬럼6
		(_iupdateID == XRCID("m_viewMenu_Column_7")) ||	//컬럼7
		(_iupdateID == XRCID("m_viewMenu_Column_8")) ||	//컬럼8
		(_iupdateID == XRCID("m_viewMenu_Column_9")))	//컬럼8
	{
		//윈도우 스타일
		int iViewStyle = theJsonConfig->GetSplitStyle();
		wxString strMenuID = "m_viewMenu_Window_" + wxString::Format("%d", iViewStyle);
		if (_iupdateID == XRCID(strMenuID))
			event.Check(true);

		//컬럼
		int iColumnCnt = theJsonConfig->GetColumnCount();
		strMenuID = "m_viewMenu_Column_" + wxString::Format("%d", iColumnCnt);

		if (_iupdateID == XRCID(strMenuID))
			event.Check(true);
	}
	else if ((_iupdateID == XRCID("m_viewMenu_DefaultFile")) ||
	    	 (_iupdateID == XRCID("m_viewMenu_HiddenFile")) ||
		     (_iupdateID == XRCID("m_viewMenu_SystemFile")) ||
		     (_iupdateID == XRCID("m_viewMenu_AllFile")))

	{
		if (theJsonConfig->IsViewAllFile())
		{
			if (_iupdateID == XRCID("m_viewMenu_AllFile"))
				event.Check(true);
			else
				event.Check(false);
		}
		else if(theJsonConfig->IsViewHiddenFile())
		{
			if (_iupdateID == XRCID("m_viewMenu_HiddenFile"))
				event.Check(true);
			else
				event.Check(false);
		}
		else
		{
			if(_iupdateID == XRCID("m_viewMenu_DefaultFile"))
				event.Check(true);
			else
				event.Check(false);
		}
	}
	else if ((_iupdateID == XRCID("m_viewMenu_SortName")) ||
	    	 (_iupdateID == XRCID("m_viewMenu_SortTime")) ||
		     (_iupdateID == XRCID("m_viewMenu_SortSize")))
	{
		typedef struct _stSORTTYPE {
			wxString strType;
		} SORTTYPE;

		SORTTYPE _stSortType[] = {
			wxT("Name"),
			wxT("Time"),
			wxT("Size"),
		};

		int iSortType = theJsonConfig->GetSortType();

		wxString strSortMenu = wxT("m_viewMenu_Sort") + _stSortType[iSortType].strType;
		if(_iupdateID == XRCID(strSortMenu))
			event.Check(true);
	}
	else if(_iupdateID == XRCID("m_viewMenu_DirNumber"))
	{
		if(_gContextManager->IsShowDirectoryNumbering())
			event.Check(true);
	}
}

void CMenuEvent::DoMenuOperation(int nMenuID)
{
	std::unordered_map<int, _MENU_EVENT_TYPE>::const_iterator fIter = m_mapMenu.find(nMenuID);
	if(fIter == m_mapMenu.end())
		return;

	m_pMenuOperation->ExecuteMenuOperation(nMenuID, fIter->second);
}

void CMenuEvent::InitToolbarItems(wxAuiToolBar* pAuiToolbar)
{
	wxString strRefreshBMP = theUtility->GetWorkDir() + SLASH + wxT("img") + SLASH + wxT("refresh.bmp");
	wxString strMakeFolderBMP = theUtility->GetWorkDir() + SLASH + wxT("img") + SLASH + wxT("MakeFolder.bmp");
	wxString strEnvBMP = theUtility->GetWorkDir() + SLASH + wxT("img") + SLASH + wxT("environment.bmp");

	m_pToolBar = pAuiToolbar;

	m_pToolBar->AddTool(TOOLBAR_ID_GO_ROOT, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_GO_ROOT")), wxArtProvider::GetBitmap( wxART_GOTO_FIRST, wxART_TOOLBAR ), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_GO_ROOT")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_GO_BACK, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_GO_BACK")), wxArtProvider::GetBitmap( wxART_GO_BACK, wxART_TOOLBAR ), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_GO_BACK")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_REFRESH, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_REFRESH_ALL")), wxBitmap(strRefreshBMP, wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_REFRESH_ALL")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(TOOLBAR_ID_FILE_EDIT, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_EDIT")), wxArtProvider::GetBitmap(wxART_EDIT, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_EDIT")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_FIND, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FIND")), wxArtProvider::GetBitmap(wxART_FIND, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FIND")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(TOOLBAR_ID_DIR_MANAGER, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_MCD")), wxBitmap(strMakeFolderBMP, wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_MCD")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_BOOKMARK, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FAVORITE")), wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK, wxART_BUTTON));
	m_pToolBar->SetToolDropDown(TOOLBAR_ID_BOOKMARK, true);
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(TOOLBAR_ID_ENV_SETTING, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENVIRONMENT")), wxBitmap(strEnvBMP, wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENVIRONMENT")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_ENV_SAVE, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENV_SAVE")), wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENV_SAVE")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();

	wxString strResourceBMP = theUtility->GetWorkDir() + SLASH + wxT("img") + SLASH + wxT("window_one.bmp");
	wxString strDriveBMP = theUtility->GetWorkDir() + SLASH + wxT("img") + SLASH + wxT("drvlist.bmp");
	//자원사용량
	m_pToolBar->AddTool(TOOLBAR_ID_RESOURCE_USAGE, theMsgManager->GetMessage(wxT("MSG_RESOURCE_USAGE_VIEW")), wxBitmap(strResourceBMP, wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_RESOURCE_USAGE_VIEW")), wxEmptyString, NULL);
	//디스크사용
	m_pToolBar->AddTool(TOOLBAR_ID_DISK_USAGE, theMsgManager->GetMessage(wxT("MSG_DISK_SIZE_VIEW")), wxBitmap(strDriveBMP, wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_DISK_SIZE_VIEW")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();

	m_pToolBar->AddTool(TOOLBAR_ID_THIS_PROGRAM, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_THISPG")), wxArtProvider::GetBitmap(wxART_HELP_PAGE, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_THISPG")), wxEmptyString, NULL);
	m_pToolBar->Realize();

	typedef struct stToolbarMenuList {
		int _ToolbarMenuID;
		wxString _strMenuID;
	} TOOLBAR_MENU_LIST;

	TOOLBAR_MENU_LIST toolBarList[] = {
		{ TOOLBAR_ID_GO_ROOT,         wxT("m_pMenu_Root") },
		{ TOOLBAR_ID_GO_BACK,         wxT("m_pMenu_Back") },
		{ TOOLBAR_ID_REFRESH,         wxT("m_pMenu_Refresh") },
		{ TOOLBAR_ID_FILE_EDIT,       wxT("m_fMenu_FileEdit") },
		{ TOOLBAR_ID_FIND,            wxT("m_eMenu_Search") },
		{ TOOLBAR_ID_DIR_MANAGER,     wxT("m_pMenu_PathMng") },
		{ TOOLBAR_ID_BOOKMARK,        wxT("") },

		{ TOOLBAR_ID_RESOURCE_USAGE,  wxT("") },
		{ TOOLBAR_ID_DISK_USAGE,      wxT("") },

		{ TOOLBAR_ID_ENV_SETTING,     wxT("m_toolMenu_ENV") },
		{ TOOLBAR_ID_ENV_SAVE,        wxT("m_toolMenu_CurSave") },
		{ TOOLBAR_ID_THIS_PROGRAM,    wxT("m_helpMenu_thisProgram") },
	};

	for(unsigned int i = 0; i < WXSIZEOF(toolBarList); i++)
	{
		m_mapToolBarMenu.insert(std::make_pair(toolBarList[i]._ToolbarMenuID, toolBarList[i]._strMenuID));
		if(toolBarList[i]._ToolbarMenuID == TOOLBAR_ID_BOOKMARK)
			this->Connect(toolBarList[i]._ToolbarMenuID, wxEVT_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(CMenuEvent::OnDropdownBookmark));
		else
			this->Connect(toolBarList[i]._ToolbarMenuID, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CMenuEvent::OnToolbarEvent));
	}
}

void CMenuEvent::SetBookmarkMenuEvent(int nMenuID)
{
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, &CMenuEvent::OnSelectedBookmark, this, nMenuID);
}

void CMenuEvent::OnSelectedBookmark(wxCommandEvent& event)
{
	int nMenuID = event.GetId();

	wxString strBookmarkPath = theBookmark->GetBookmarkPath(nMenuID);
	_gContextManager->GotoBookmarkPath(strBookmarkPath);
}

void CMenuEvent::OnToolbarEvent(wxCommandEvent& event)
{
	int nMenuID = event.GetId();
	if(nMenuID == TOOLBAR_ID_BOOKMARK)
		return;

	// Disk Usage or Resource Usage
	if( (nMenuID == TOOLBAR_ID_RESOURCE_USAGE) ||
		(nMenuID == TOOLBAR_ID_DISK_USAGE) )
	{
		m_pMenuOperation->ExecuteMenuAnother(nMenuID);
		return;
	}

	std::unordered_map<int, wxString>::const_iterator fIter = m_mapToolBarMenu.find(nMenuID);
	if(fIter == m_mapToolBarMenu.end())
		return;

	nMenuID = XRCID(fIter->second);
	DoMenuOperation(nMenuID);
}

void CMenuEvent::OnDropdownBookmark(wxAuiToolBarEvent& event)
{
	if (theBookmark->GetBookmarkSize() > 0)
	{
		wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(event.GetEventObject());

		wxMenu* pFavoriteMenu = new wxMenu();
		theBookmark->CreateBookmarkMenu(pFavoriteMenu);

		tb->SetToolSticky(event.GetId(), true);

		wxRect rect = tb->GetToolRect(event.GetId());
		wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());

        pt = m_pMainFrame->ScreenToClient(pt);
        m_pMainFrame->PopupMenu(pFavoriteMenu, pt);

        tb->SetToolSticky(event.GetId(), false);
	}
}

void CMenuEvent::LoadBookmarkToMenu(bool IsFromFavoriteMangaer)
{
	wxMenuBar* menuBar = m_pMainFrame->GetMenuBar();
	wxMenu *pFavoriteMenu = menuBar->GetMenu(menuBar->FindMenu(theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FAVORITE"))));

	if (IsFromFavoriteMangaer)
	{
		//기존 추가되었던 메뉴 삭제
		int iMenuCount = pFavoriteMenu->GetMenuItemCount();
		for (int i = 2; i < iMenuCount; i++)
		{
			wxMenuItem *pMenuItem = pFavoriteMenu->FindItemByPosition(2);
			pFavoriteMenu->Destroy(pMenuItem);
		}
	}

	if (theBookmark->GetBookmarkSize() > 0)
	{
		pFavoriteMenu->AppendSeparator();
		theBookmark->CreateBookmarkMenu(pFavoriteMenu);
	}
}


void CMenuEvent::OnCompressExecute(wxCommandEvent& event)
{
	int nId = event.GetId();
	wxString strCompressedFile = event.GetString();
	m_pMenuOperation->CompressMenu_ExecuteCompress(nId, strCompressedFile);
}
