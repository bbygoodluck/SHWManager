#ifndef LOCALFILESYSTEM_H_INCLUDED
#define LOCALFILESYSTEM_H_INCLUDED

class CLocalFileSystem final
{
public:
	explicit CLocalFileSystem();
	~CLocalFileSystem();
/*
	enum local_filetype
	{
		unknown = -1,
		file,
		dir,
		link,
		link_file = link,
		link_dir
	};
*/
public:
	void EndFindFiles();
	bool BeginFindFiles(wxString path, bool dirs_only = false);
	bool GetNextFile(wxString& strName, bool* isDir = nullptr, unsigned long* latt = nullptr, wxLongLong* llSize = nullptr, wxDateTime* dt = nullptr);

	static bool GetAttribute(const wxString& strFilePathName, bool* isDir, unsigned long* lattr, wxLongLong* llSize, wxDateTime* dt);
	static bool IsDirReadable(const wxString& strPath);
	static bool IsWritable(const wxString& strPath, DWORD dwShareMode = 0, bool IsWatcher = false);
	static bool RecursiveCopyMove(const std::list<wxString>& dirsToVisit, const wxString& strTargetPath, wxWindow* parent, bool IsCopy);
#ifdef __WXMSW__
	static bool RecursiveCopySamePathForWindows(std::list<wxString>& dirsToVisit, const wxString& strDest, wxWindow* parent);
#endif
	static bool RecursiveDelete(const wxString& path, wxWindow* parent, bool bGoTrash, int& iRet);
	static bool RecursiveDelete(const std::list<wxString>& dirsToVisit, wxWindow* parent, bool bGoTrash, int& iRet);

	static enum FILE_TYPE GetFileType(const wxString& path);
	enum FILE_TYPE GetFileAttribute(const wxString& strFullPathFileName);
private:
#ifdef __WXMSW__
	static bool RecursiveDeleteSHFileOperation(const std::list<wxString>& dirsToVisit, wxWindow* parent, bool bGoTrash, int& iRet);
	static bool RecursiveDeleteIFileOperation(const std::list<wxString>& dirsToVisit, wxWindow* parent, bool bGoTrash, int& iRet);
	static bool CheckFileUseAnotherProcess(const std::list<wxString>& lstSrc, std::list<wxString>& lstOPTgt);
#endif // __WXMSW__

private:
	bool m_bDirs_only;
#ifdef __WXMSW__
	#ifdef _UNICODE
		typedef WIN32_FIND_DATAW WIN32_DATA;
	#else
		typedef WIN32_FIND_DATA WIN32_DATA;
	#endif

	WIN32_DATA m_find_data;
	HANDLE m_hFind;
	bool m_bfound;
	wxString m_strFind_path;

	bool m_IsLastSlash = false;
#else
	char* m_raw_path;
	char* m_file_part; // Points into m_raw_path past the trailing slash of the path part
	int m_buffer_length;
	DIR* m_dir;
#endif
};

#endif // LOCALFILESYSTEM_H_INCLUDED
