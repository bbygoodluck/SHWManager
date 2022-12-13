#ifndef EXECUTEMENUOPERATION_H_INCLUDED
#define EXECUTEMENUOPERATION_H_INCLUDED

class CMenuEvent;
class CExecuteMenuOperation final
{
public:
	CExecuteMenuOperation(CMenuEvent* pMenuEvent);
	~CExecuteMenuOperation();

public:
	void ExecuteMenuOperation(int iMenuID, _MENU_EVENT_TYPE _mType);
	void ExecuteMenuAnother(int _menuID);
	//압축 실행
	void CompressMenu_ExecuteCompress(int nId, const wxString& strCompressedFile);

private:
	void ResourceUsage();
	void ViewDiskSize();

	void Init();
	//파일메뉴
	void FileMenu_Execution();
	void FileMenu_NewTab();
	void FileMenu_Copy();
	void FileMenu_Move();
	void FileMenu_Rename();
	void FileMenu_CreateFolder();
	void FileMenu_Edit();
	void FileMenu_Trash();
	void FileMenu_CompletionDel();
	void FileMenu_ExecCMD();
	void FileMenu_Terminate();
	//편집메뉴
	void EditMenu_AllRelease();
	void EditMenu_AllSelect();
	void EditMenu_Copy_UseClipboard();
	void EditMenu_Move_UseClipboard();
	void EditMenu_Paste_UseClipboard();
	void EditMenu_FileFind();
	void EditMenu_ViewContextMenu();

	//경로메뉴
	void PathMenu_DirManager();
	void PathMenu_Refresh();
	void PathMenu_Back();
	void PathMenu_GoRoot();
	void PathMenu_GotoDirDirectly();

	//압축
	void CompressMenu_DoCompress();

	void CompressMenu_DeCompressCurDir();
	void CompressMenu_DeCompressMkDir();
	void CompressMenu_DeCompressSelDir();

	//보기메뉴
	void ViewMenu_FullScreen();
	void ViewMenu_Window_Single();
	void ViewMenu_Window_Vert();
	void ViewMenu_Window_Horz();
	void ViewMenu_Column_0();
	void ViewMenu_Column_1();
	void ViewMenu_Column_2();
	void ViewMenu_Column_3();
	void ViewMenu_Column_4();
	void ViewMenu_Column_5();
	void ViewMenu_Column_6();
	void ViewMenu_Column_7();
	void ViewMenu_Column_8();
	void ViewMenu_Column_9();
	void ViewMenu_FileView_Default();
	void ViewMenu_FileView_Hidden();
	void ViewMenu_FileView_All();
	void ViewMenu_SortDefault();
	void ViewMenu_SortTime();
	void ViewMenu_SortSize();
	void ViewMenu_SubDir();
	//즐겨찾기
	void FavoriteMenu_Add();
	void FavoriteMenu_Manager();
	void FavoriteMenu_ItemSelected(int iMenuID);
	//System메뉴
	void SystemMenu_ResourceUsage();
	void SystemMenu_DiskUsage();
	//도구메뉴
	void ToolMenu_Setting();
	void ToolMenu_SaveSetting();
	//이프로그램은..
	void HelpMenu_ThisProgram();

	void DoFileOperation(FILE_OPERATION fop, bool IsUseClipboard = false);
	wxString GetDirectorySelect();

	void SetFileOperationClipboardData(bool IsCut);
	void DeleteProcess(_MENU_EVENT_TYPE mType);

	void DoItemSelectProcess(_MENU_EVENT_TYPE mType);

private:
	void ChangeViewStyle(int nViewStyle);
	void ChangeViewColumn(COLUMN_VIEW_OPTION columnOP);
	void ChangeFileList(FILE_VIEW_OPTION viewOP);
	void ChangeViewSort(VIEW_SORT sType);
	void BookmarkOperation(_MENU_EVENT_TYPE mType);
	void DoDecompress(_MENU_EVENT_TYPE _menuType);

private:
	std::unordered_map< int, std::function<void()> > m_mapMenuFunctionPtr;
	CMenuEvent* m_pMenuEvent = nullptr;
};

#endif // EXECUTEMENUOPERATION_H_INCLUDED
