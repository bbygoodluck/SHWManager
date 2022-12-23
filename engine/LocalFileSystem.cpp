#include "../ginc.h"
#include "LocalFileSystem.h"

static wxInt64 EPOCH_OFFSET_IN_MSEC = wxLL(11644473600000);

CLocalFileSystem::CLocalFileSystem()
	: m_bDirs_only(false)
#ifdef __WXMSW__
	, m_hFind(INVALID_HANDLE_VALUE)
	, m_bfound(false)
	, m_strFind_path(wxT(""))
#else
	, m_raw_path()
	, m_file_part()
	, m_buffer_length()
	, m_dir()
#endif
{
}

CLocalFileSystem::~CLocalFileSystem()
{
	EndFindFiles();
}

bool CLocalFileSystem::BeginFindFiles(wxString path, bool dirs_only)
{
	EndFindFiles();
	m_bDirs_only = dirs_only;

	m_strFind_path = path;
	m_IsLastSlash = false;
#ifdef __WXMSW__
/*
	if (path.Last() != '/' && path.Last() != '\\')
	{
		m_strFind_path = path + SLASH;
		path += SLASH + wxT("*");
	}
	else
	{
		m_strFind_path = path;
		path += '*';
	}
*/
	if( path.Right(1).CmpNoCase(SLASH) != 0)
	{
		m_strFind_path = path + SLASH;
		m_strFind_path += wxT("*");
	}
	else
	{
		m_IsLastSlash = true;
		m_strFind_path = path;
		m_strFind_path += wxT("*");
	}

//	m_hFind = FindFirstFileEx(m_strFind_path, FindExInfoStandard, &m_find_data, dirs_only ? FindExSearchLimitToDirectories : FindExSearchNameMatch, NULL, 0);
	m_hFind = FindFirstFile(m_strFind_path, &m_find_data);
	if (m_hFind == INVALID_HANDLE_VALUE)
	{
		m_bfound = false;
		return false;
	}

	m_bfound = true;
	return true;
#else
	if (path != _T("/") && path.Last() == '/')
		path.RemoveLast();

	const wxCharBuffer s = path.fn_str();

	m_dir = opendir(s);
	if (!m_dir)
		return false;

	const wxCharBuffer p = path.fn_str();
	const int len = strlen(p);
	m_raw_path = new char[len + 2048 + 2];
	m_buffer_length = len + 2048 + 2;
	strcpy(m_raw_path, p);
	if (len > 1)
	{
		m_raw_path[len] = '/';
		m_file_part = m_raw_path + len + 1;
	}
	else
		m_file_part = m_raw_path + len;

	return true;
#endif
}

bool CLocalFileSystem::GetNextFile(wxString& strName, bool* isDir, unsigned long* lattr, wxLongLong* llSize, wxDateTime* dt)
{
#ifdef __WXMSW__
	if (!m_bfound)
		return false;
	do
	{
		if (!m_find_data.cFileName[0])
		{
			m_bfound = FindNextFile(m_hFind, &m_find_data) != 0;
			return true;
		}

		if (m_bDirs_only && !(m_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;

		wxString strTmpName(m_find_data.cFileName);
		if(strTmpName.CmpNoCase(wxT(".")) == 0)
			continue;

		/*
		 * 공유드라이브(또는 공유폴더)의 최상위 폴더일경우 .. 표시가 되어
		 * 공유드라이브(또는 공유폴더)일경우 ..표시를 화면에 나타내지 않음
		 */
		if(m_IsLastSlash)
		{
			if(strTmpName.CmpNoCase(wxT("..")) == 0)
				continue;
		}
	//	if ((m_find_data.cFileName[0] == '.') && !m_find_data.cFileName[1])
	//		continue;

		strName = strTmpName;//m_find_data.cFileName;
	//	if(lattr && lattr != nullptr)
		if(lattr != nullptr)
			*lattr = m_find_data.dwFileAttributes;

		if(isDir != nullptr)
			*isDir = (m_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

	//	if (llSize && llSize != nullptr)
		if (llSize != nullptr)
		{
			if (*isDir)
				*llSize = -1;
			else
				*llSize = wxLongLong(m_find_data.nFileSizeHigh, m_find_data.nFileSizeLow);
		}

	//	if (dt && dt != nullptr)
		if (dt != nullptr)
		{
			wxLongLong t(m_find_data.ftLastWriteTime.dwHighDateTime, m_find_data.ftLastWriteTime.dwLowDateTime);
			t /= 10000; // Convert hundreds of nanoseconds to milliseconds.
			t -= EPOCH_OFFSET_IN_MSEC;

			*dt = wxDateTime(t);
		}

		m_bfound = FindNextFile(m_hFind, &m_find_data) != 0;
		return true;
	} while ((m_bfound = FindNextFile(m_hFind, &m_find_data) != 0));

	return false;
#else
	return false;
#endif
}

void CLocalFileSystem::EndFindFiles()
{
#ifdef __WXMSW__
	m_bfound = false;
	if (m_hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(m_hFind);
		m_hFind = INVALID_HANDLE_VALUE;
	}
#else
	if (m_dir)
	{
		closedir(m_dir);
		m_dir = 0;
	}

	delete[] m_raw_path;
	m_raw_path = 0;
	m_file_part = 0;
#endif
}

bool CLocalFileSystem::IsDirReadable(const wxString& strPath)
{
	if (!wxFileName::IsDirReadable(strPath))
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_DIREDTORY_READ_FAIL")), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return false;
	}

	return true;
}

bool CLocalFileSystem::IsWritable(const wxString& strPath, DWORD dwShareMode, bool IsWatcher)
{
#ifdef __WXMSW__
	DWORD dwAttr = GetFileAttributes(strPath);
	DWORD dwDesiredAccess = GENERIC_READ;
	DWORD dwFlagsAndAttr = 0;
	if(dwAttr == FILE_ATTRIBUTE_DIRECTORY)
	{
		dwDesiredAccess = FILE_GENERIC_READ;
		dwFlagsAndAttr = FILE_FLAG_BACKUP_SEMANTICS;
//		return wxDirExists(strPath);
	}
//	bool IsFileExist = wxFileExists(strPath);
//	if (!IsFileExist)
//		return true;

	if(dwAttr == (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE))
		return true;

	HANDLE hFile = INVALID_HANDLE_VALUE;

    hFile = CreateFile(strPath,
					   dwDesiredAccess,
                       dwShareMode,
                       NULL,
                       OPEN_EXISTING,
                       dwFlagsAndAttr,
                       NULL);

	DWORD dwErr = GetLastError();
    if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		if(IsWatcher)
		{
			dwErr = GetLastError();
			if(dwErr == 0x0020) //다른 프로세스에서 이 파일을 사용하고 있기 때문에 이 파일을 액세스할 수 없습니다.(잘라내기가 아닌경우 체크)
				return true;
		}

		return false;
	}

    CloseHandle(hFile);
    return true;
#else
	if (mode == ACCESS_FILE_READ)
		return false;

	tstring _strFileName = CONVSTR(strPath);

	std::ifstream ifs(_strFileName);
	if (ifs.is_open())
		return true;

	return false;
#endif
}

bool CLocalFileSystem::GetAttribute(const wxString& strFilePathName, bool* isDir, unsigned long* lattr, wxLongLong* llSize, wxDateTime* dt)
{
#ifdef __WXMSW__
	WIN32_FILE_ATTRIBUTE_DATA fileAttr = { 0, };
	BOOL result = GetFileAttributesEx(strFilePathName, GetFileExInfoStandard, &fileAttr);

	if(!result)
		return false;

	*isDir = (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

	*lattr = fileAttr.dwFileAttributes;

	*llSize = 0;
	if(!*isDir)
		*llSize = wxLongLong(fileAttr.nFileSizeHigh, fileAttr.nFileSizeLow);

	wxLongLong t(fileAttr.ftLastWriteTime.dwHighDateTime, fileAttr.ftLastWriteTime.dwLowDateTime);
	t /= 10000; // Convert hundreds of nanoseconds to milliseconds.
	t -= EPOCH_OFFSET_IN_MSEC;

	*dt = wxDateTime(t);

#else
#endif // __WXMSW__

	return true;
}

bool CLocalFileSystem::CheckFileUseAnotherProcess(const std::list<wxString>& lstSrc, std::list<wxString>& lstOPTgt)
{
	size_t itemCount = lstSrc.size();
	if(itemCount == 1)
	{
#ifdef __WXMSW__
		std::list<wxString>::const_iterator iter = lstSrc.begin();

		wxString strItem(*iter);
		DWORD dwRet = GetFileAttributes(strItem);
		if(dwRet == FILE_ATTRIBUTE_NORMAL || dwRet == FILE_ATTRIBUTE_ARCHIVE)
		{
			bool bWritable = IsWritable(strItem, 0);
			wxString strMsg(strItem);

			if (!bWritable)
			{
				wxMessageBox(strMsg + wxT(" is opened!. you can't file operation!"), wxT("FileOperation"), wxICON_ERROR, _gMainFrame);
				return false;
			}
		}

		lstOPTgt.push_back(strItem);
#else
#endif // __WXMSW__
	}
	else
	{
		int nRet = 0;
		for (auto const& item : lstSrc)
		{
#ifdef __WXMSW__
			DWORD dwRet = GetFileAttributes(item);
			if(dwRet == FILE_ATTRIBUTE_NORMAL)
			{
				bool bWritable = IsWritable(item, 0);

				if (!bWritable)
				{
					nRet = wxMessageBox(item + wxT(" is opend another program!. this file to be skipped! \n Continue?"), wxT("File Operation"), wxYES_NO | wxICON_ERROR, _gMainFrame);
					if (nRet == wxYES)
						continue;

					lstOPTgt.clear();
					return false;
				}
			}

			lstOPTgt.push_back(item);
#else
#endif // __WXMSW__
		}
	}


	return lstOPTgt.size() > 0 ? true : false;
}

bool CLocalFileSystem::RecursiveCopyMove(const std::list<wxString>& dirsToVisit, const wxString& strTargetPath, wxWindow* parent, bool IsCopy)
{
	std::list<wxString> lstOPTgt;
	if(!IsCopy)
	{
		if(!CheckFileUseAnotherProcess(dirsToVisit, lstOPTgt))
			return false;
	}
	else
		lstOPTgt.assign(dirsToVisit.begin(), dirsToVisit.end());

#ifdef __WXMSW__
	//Reference filezilla
	int len = 1;

	for (auto const& dirItem : lstOPTgt)
		len += dirItem.size() + 1;


	wxChar* from = new wxChar[len];
	wxChar* p = from;

	for (auto dir : lstOPTgt)
	{
		_tcscpy(p, dir);
		p += dir.size() + 1;
	}

	*p = 0; // End of list

	wxChar* to = new wxChar[strTargetPath.Len() + 2];
	wxStrcpy(to, strTargetPath);
	to[strTargetPath.Len() + 1] = 0; // End of list

	SHFILEOPSTRUCT op;
	wxZeroMemory(op);

	op.hwnd = parent ? (HWND)parent->GetHandle() : 0;
	op.pFrom = from;
	op.pTo = to;
	op.wFunc = IsCopy ? FO_COPY : FO_MOVE;

	int iRet = SHFileOperation(&op);

	delete [] to;
	delete [] from;

	if(iRet != 0)
		return false;

	return true;

#else

#endif
}

#ifdef __WXMSW__
bool CLocalFileSystem::RecursiveCopySamePathForWindows(std::list<wxString>& dirsToVisit, const wxString& strDest, wxWindow* parent)
{
	wxString strName(wxT(""));
	wxString strFullPath(wxT(""));
	bool bReturn = true;

	IFileOperation* pfo;
	HRESULT hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
		return false;

	hr = ::CoCreateInstance(__uuidof(FileOperation), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfo));
	if (FAILED(hr))
		return false;

	IShellItem* psiTo = nullptr;
	hr = SHCreateItemFromParsingName(CONVSTR(strDest), NULL, IID_PPV_ARGS(&psiTo));

	if(FAILED(hr))
		return false;

	for (auto strItem : dirsToVisit)
	{
		IShellItem* psiFrom = nullptr;
		hr = SHCreateItemFromParsingName(CONVSTR(strItem), NULL, IID_PPV_ARGS(&psiFrom));

		if(SUCCEEDED(hr))
		{
			strName = theUtility->GetPathName(strItem);
			if(theUtility->Compare(strDest.Right(1), SLASH) == 0)
				strFullPath = strDest + strName;
			else
				strFullPath = strDest + SLASH + strName;

			if (strFullPath.CmpNoCase(strItem) == 0)
				strName  = theUtility->ChangeName(strFullPath);

			hr = pfo->CopyItem(psiFrom, psiTo, strName, nullptr);
			if(SUCCEEDED(hr))
				hr = pfo->PerformOperations();

			psiFrom->Release();
		}
		else
		{
			bReturn = false;
			break;
		}
	}

	psiTo->Release();
	CoUninitialize();
	return bReturn;
}
#endif

bool CLocalFileSystem::RecursiveDelete(const wxString& path, wxWindow* parent, bool bGoTrash, int& iRet)
{
	std::list<wxString> paths;
	paths.push_back(path);
	return RecursiveDelete(paths, parent, bGoTrash, iRet);
}

bool CLocalFileSystem::RecursiveDelete(const std::list<wxString>& dirsToVisit, wxWindow* parent, bool bGoTrash, int& iRet)
{
	std::list<wxString> lstOPTgt;
	if(!CheckFileUseAnotherProcess(dirsToVisit, lstOPTgt))
		return false;

	if(bGoTrash && lstOPTgt.size() > 0)
	{
		wxString strMsg(wxT(""));
		int nOperationItemCounts = lstOPTgt.size();
		for(auto const& tgtItem: lstOPTgt)
		{
			strMsg += tgtItem;
			strMsg += wxT("\n");
		}

		strMsg += wxString::Format(wxT("%d "), nOperationItemCounts);
		strMsg += theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_SELITEM"));
		strMsg += wxT("\n");

		int nRet = wxMessageBox(strMsg, wxT("File Operation"), wxYES_NO | wxICON_EXCLAMATION, _gMainFrame);
		if(nRet != wxYES)
		{
			lstOPTgt.clear();
			return false;
		}
	}

	bool bRet = true;

#ifdef __WXMSW__

	int i_use_windowsShell = theJsonConfig->GetUseWindowShell();
	bRet = i_use_windowsShell == 0 ? RecursiveDeleteSHFileOperation(lstOPTgt, parent, bGoTrash, iRet) :
		                             RecursiveDeleteIFileOperation(lstOPTgt, parent, bGoTrash, iRet);
#else
#endif

	return bRet;
}

#ifdef __WXMSW__
bool CLocalFileSystem::RecursiveDeleteSHFileOperation(const std::list<wxString>& dirsToVisit, wxWindow* parent, bool bGoTrash, int& iRet)
{
	bool bRet = true;
	// String list terminated by empty string
	size_t len = 1;

	for (auto const& dirItem : dirsToVisit)
		len += dirItem.size() + 1;

	// Allocate memory
	wxChar* pBuffer = new wxChar[len];
	wxChar* p = pBuffer;

	for (auto dir : dirsToVisit)
	{
		if (dir.Last() == wxFileName::GetPathSeparator())
			dir.RemoveLast();

		if (GetFileType(dir) == FTYPE_UNKNOWN)
			continue;

		_tcscpy(p, dir);
		p += dir.size() + 1;
	}

	if (p != pBuffer)
	{
		*p = 0;

		SHFILEOPSTRUCT fileop;
		wxZeroMemory(fileop);

		fileop.hwnd = parent ? (HWND)parent->GetHandle() : 0;
		fileop.wFunc = FO_DELETE;
		fileop.pFrom = pBuffer;

		if (parent)
			fileop.fFlags = bGoTrash ? FOF_ALLOWUNDO : 0;
		else
			fileop.fFlags = FOF_NOCONFIRMATION;

		int ret = SHFileOperation(&fileop);
		if (ret != 0)
		{
			wxLogApiError(wxT("SHFileOperation"), ret);
			bRet = false;
		}
	}

	delete[] pBuffer;
	return bRet;
}

bool CLocalFileSystem::RecursiveDeleteIFileOperation(const std::list<wxString>& dirsToVisit, wxWindow* parent, bool bGoTrash, int& iRet)
{
	bool bRet = true;

	HRESULT hr;
	IFileOperation* pfo = 0;

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
		return false;

	hr = CoCreateInstance(__uuidof(FileOperation), 0, CLSCTX_ALL, IID_PPV_ARGS(&pfo));

	if (FAILED(hr) || !pfo)
	{
		CoUninitialize();
		return false;
	}

	std::vector<ITEMIDLIST *> idLists;
	for (auto& path : dirsToVisit)
	{
		__unaligned ITEMIDLIST* idl = ILCreateFromPath(path);
		if(!idl)
		{
			for(auto& pid: idLists)
				ILFree(pid);

			return false;
		}

		idLists.push_back(idl);
	}

	IShellItemArray* iArray = 0;
	hr = SHCreateShellItemArrayFromIDLists((UINT)idLists.size(), (LPCITEMIDLIST *)idLists.data(), &iArray);

	for(auto& pid: idLists)
		ILFree(pid);

	idLists.clear();

	if(!SUCCEEDED(hr) || !iArray)
	{
		CoUninitialize();
		iArray->Release();
		return false;
	}

	hr = pfo->SetOperationFlags(bGoTrash ? FOF_ALLOWUNDO : FOF_WANTNUKEWARNING);
	hr = pfo->SetOwnerWindow(parent->GetHWND());

	if(FAILED(hr))
	{
		CoUninitialize();
		iArray->Release();
		return false;
	}

	hr = pfo->DeleteItems(iArray);
	if(FAILED(hr))
	{
		CoUninitialize();
		iArray->Release();
		return false;
	}

	hr = pfo->PerformOperations();
	if(FAILED(hr))
		bRet = false;

	pfo->Release();
	iArray->Release();

	CoUninitialize();
	return bRet;
}
#endif
#include <io.h>

enum FILE_TYPE CLocalFileSystem::GetFileType(const wxString& path)
{
#ifdef __WXMSW__
	DWORD result = GetFileAttributes(path);
	if (result == INVALID_FILE_ATTRIBUTES)
		return FTYPE_UNKNOWN;

	const unsigned long attrubute_mapping[][2] = {
		{ FILE_ATTRIBUTE_DIRECTORY,			FTYPE_DIR     },
		{ FILE_ATTRIBUTE_REPARSE_POINT,		FTYPE_LINK    },
		{ FILE_ATTRIBUTE_READONLY,          FTYPE_FILE    },
		{ FILE_ATTRIBUTE_HIDDEN,            FTYPE_FILE    },
		{ FILE_ATTRIBUTE_SYSTEM,            FTYPE_FILE    }
	};

	for (unsigned long i = 0; i < WXSIZEOF(attrubute_mapping); ++i)
	{
		if (result & attrubute_mapping[i][0])
			return attrubute_mapping[i][1];
	}
	/*
	if (result & FILE_ATTRIBUTE_DIRECTORY)
		return FTYPE_DIR;



	if (result & FILE_ATTRIBUTE_REPARSE_POINT)
		return FTYPE_LINK;
	*/
/*
	const HANDLE h = ::CreateFile( path.t_str() ,
                                   0,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   NULL,
                                   OPEN_EXISTING,
                                   result & FILE_ATTRIBUTE_DIRECTORY ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL,
                                   NULL);

    if ( h != INVALID_HANDLE_VALUE )
	{
        CloseHandle(h);
        return FTYPE_FILE;
	}
*/
	int nExist;
	int nAccess;

#ifdef _UNICODE
	nExist = _waccess(path.wchar_str(), 0);
	nAccess = _waccess(path.wchar_str(), 2);

	if(nExist == -1)
		return FTYPE_UNKNOWN;

	if(nExist == 0)
	{
		if(nAccess == -1)
			return FTYPE_UNKNOWN;

		return FTYPE_FILE;
	}
#else
	nExist = _assess(path.c_str(), 0);
	nAccess = _assess(path.c_str(), 2);

	if(nExist != -1)
		return FTYPE_UNKNOWN;

	if(nExist)
	{
		if(nAccess == -1)
			return FTYPE_UNKNOWN;

		return FTYPE_FILE;
	}
#endif

	return FTYPE_UNKNOWN;
#else
	if (path.Last() == '/' && path != _T("/"))
	{
		wxString tmp = path;
		tmp.RemoveLast();
		return GetFileType(tmp);
	}

	wxStructStat buf;
	int result = wxLstat(path, &buf);
	if (result)
		return FTYPE_UNKNOWN;

#ifdef S_ISLNK
	if (S_ISLNK(buf.st_mode))
		return FTYPE_LINK;
#endif

	if (S_ISDIR(buf.st_mode))
		return FTYPE_DIR;

	return FTYPE_FILE;
#endif
}
