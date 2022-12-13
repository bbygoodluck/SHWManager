#include "../../ginc.h"
#include "LocalFileSystemWatcher.h"

CLocalFileSystemWatcher::CLocalFileSystemWatcher(wxWindow* pOwner)
	: CFileSystemWatcherBase(pOwner)
{
}

CLocalFileSystemWatcher::~CLocalFileSystemWatcher()
{
	Clear();
}

void CLocalFileSystemWatcher::Clear()
{
	m_bRemove = true;
	m_pWatcherQueue->Reset();

	if (theJsonConfig->IsDirectoryWatcherAsync())
	{
		if (m_watchDir.m_hIOCP != INVALID_HANDLE_VALUE)
		{
			PostQueuedCompletionStatus(m_watchDir.m_hIOCP, 0, (ULONG_PTR)NULL, NULL);
			if (GetThread() && GetThread()->IsRunning())
				GetThread()->Wait();

			CloseHandle(m_watchDir.m_hIOCP);
			m_watchDir.m_hIOCP = NULL;
		}
	}
	else
	{
		if (GetThread() && GetThread()->IsRunning())
		{
			SetEvent(m_watchDir.PollingOverlap.hEvent);
			GetThread()->Wait();
		}

		CloseHandle(m_watchDir.PollingOverlap.hEvent);
	}

	CancelIo(m_watchDir.hFile);
	CloseHandle(m_watchDir.hFile);

	m_bRemove = false;
}

int CLocalFileSystemWatcher::AddPath(const wxString& strPath, unsigned long ulNotifyFilter, bool bSubTree)
{
	Clear();

	if (theJsonConfig->IsDirectoryWatcherAsync())
	{
		if (!(m_watchDir.m_hIOCP = CreateIoCompletionPort((HANDLE)INVALID_HANDLE_VALUE, NULL, 0, 0)))
		{
			m_nLastError = GetLastError();
			return E_FILESYSMON_ERRORADDTOIOCP;
		}
	}

	m_ulNotifyFilters = ulNotifyFilter;
	m_watchDir.m_strDir = strPath;
	m_watchDir.bSubTree = bSubTree;

	m_watchDir.hFile = CreateFile(m_watchDir.m_strDir
		, FILE_LIST_DIRECTORY //| GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY
		, FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE
		, NULL
		, OPEN_EXISTING
		, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED
		, NULL);

	if (m_watchDir.hFile == INVALID_HANDLE_VALUE)
		return E_FILESYSMON_ERROROPENFILE;

	m_watchDir.PollingOverlap.OffsetHigh = 0;

	if (theJsonConfig->IsDirectoryWatcherAsync())
	{
		m_watchDir.CompletionKey = (ULONG_PTR)m_watchDir.hFile;
		if (CreateIoCompletionPort(m_watchDir.hFile, m_watchDir.m_hIOCP, m_watchDir.CompletionKey, 0) == NULL)
		{
			CloseHandle(m_watchDir.hFile);
			return E_FILESYSMON_ERRORADDTOIOCP;
		}
	}
	else
		m_watchDir.PollingOverlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DoWatchDirectory();

	return E_FILESYSMON_SUCCESS;
}

wxThread::ExitCode CLocalFileSystemWatcher::Entry()
{
	DWORD dwBytesReturned;
	BOOL bResult = TRUE;

	const int bufferSize = MAX_PATH * 4;
	TCHAR buf[bufferSize] = { 0, };

	BOOL bResultQ = FALSE;
//	BOOL bResultR = FALSE;
	DWORD dwBytesXFered = 0;

	while (bResult)
	{
		ReadDirectoryChangesW(m_watchDir.hFile,
			m_watchDir.m_Buffer,
			CWatcherDir::BUFFER_SIZE,
			m_watchDir.bSubTree,
			m_ulNotifyFilters,
			&dwBytesReturned,
			&m_watchDir.PollingOverlap,
			NULL);

		if (theJsonConfig->IsDirectoryWatcherAsync())
		{
			bResultQ = GetQueuedCompletionStatus(m_watchDir.m_hIOCP, &dwBytesXFered, &m_watchDir.CompletionKey, (LPOVERLAPPED* )&(m_watchDir.PollingOverlap), INFINITE);
			if (!bResultQ && dwBytesXFered == 0)
			{
				if ((m_nLastError = GetLastError()) == WAIT_TIMEOUT)
					break;

				break;
			}
		}
		else
		{
			WaitForSingleObject(m_watchDir.PollingOverlap.hEvent, INFINITE);
			::GetOverlappedResult(m_watchDir.hFile, &m_watchDir.PollingOverlap, &dwBytesReturned, TRUE);
			if(dwBytesReturned != 0)
				bResultQ = TRUE;
		}

		if (m_bRemove)
			break;

		PFILE_NOTIFY_INFORMATION pNotify = (PFILE_NOTIFY_INFORMATION)m_watchDir.m_Buffer;
		bool bCanRead = true;
		if ((ULONG_PTR)pNotify - (ULONG_PTR)m_watchDir.m_Buffer > bufferSize)
			bCanRead = false;

		if (!bCanRead)
			continue;

		DWORD dwOffset = 0;

		wxString strOldName(wxT(""));
		wxString strNewName(wxT(""));

		do
		{
			dwOffset = pNotify->NextEntryOffset;
			wxZeroMemory(buf);
			errno_t err = wcsncat_s(buf, bufferSize, pNotify->FileName, std::min(bufferSize, int(pNotify->FileNameLength / sizeof(TCHAR))));
			if (err == STRUNCATE)
			{
				pNotify = (PFILE_NOTIFY_INFORMATION)((LPBYTE)pNotify + dwOffset);
				continue;
			}

			buf[std::min((decltype((pNotify->FileNameLength / sizeof(WCHAR))))bufferSize - 1, (pNotify->FileNameLength / sizeof(WCHAR)))] = L'\0';
			wxString strFileName(buf);
			wxString strFullPath = m_watchDir.m_strDir[m_watchDir.m_strDir.Len() - 1] == SLASH[0] ? m_watchDir.m_strDir + strFileName : m_watchDir.m_strDir + SLASH + strFileName;

			int iAction = Native2WatcherFlags(pNotify->Action);
			if (pNotify->Action == FILE_ACTION_RENAMED_OLD_NAME)
				strOldName = strFileName;

			if ((iAction != -1) && (pNotify->Action != FILE_ACTION_RENAMED_OLD_NAME))
				m_pWatcherQueue->AddQueue(strOldName, strFileName, iAction);

			//	ProcessWatchAction(strOldName, strFileName, iAction);

			pNotify = (PFILE_NOTIFY_INFORMATION)((LPBYTE)pNotify + dwOffset);
			if ((ULONG_PTR)pNotify - (ULONG_PTR)m_watchDir.m_Buffer > bufferSize)
				break;

		} while (dwOffset);

		m_pWatcherQueue->Run();

		wxZeroMemory(m_watchDir.m_Buffer);

		if (theJsonConfig->IsDirectoryWatcherAsync())
			SecureZeroMemory(&m_watchDir.PollingOverlap, sizeof(OVERLAPPED));
		else
			ResetEvent(m_watchDir.PollingOverlap.hEvent);
	}

	return (wxThread::ExitCode)0;
}
