#ifndef LOCALFILESYSTEMWATCHER_H_INCLUDED
#define LOCALFILESYSTEMWATCHER_H_INCLUDED

#include "../FileSystemWatcherBase.h"

class CLocalFileSystemWatcher : public CFileSystemWatcherBase
{
	class CLocalWatcherDir : public CWatcherDir
	{
		public:
			CLocalWatcherDir() : hFile(NULL), m_hIOCP(INVALID_HANDLE_VALUE) {
				wxZeroMemory(PollingOverlap);
			}
			virtual ~CLocalWatcherDir() {}

		public:
			OVERLAPPED	PollingOverlap;
			HANDLE		hFile;
			HANDLE		m_hIOCP;
			ULONG_PTR   CompletionKey;
	};

public:
	CLocalFileSystemWatcher(wxWindow* pOwner);
	virtual ~CLocalFileSystemWatcher();

	void Clear() override;
	int AddPath(const wxString& strPath,
	 		unsigned long ulNotifyFilter = FILE_NOTIFY_CHANGE_FILE_NAME |
											FILE_NOTIFY_CHANGE_DIR_NAME |
											FILE_NOTIFY_CHANGE_ATTRIBUTES |
											FILE_NOTIFY_CHANGE_SIZE |
											FILE_NOTIFY_CHANGE_LAST_WRITE |
											FILE_NOTIFY_CHANGE_LAST_ACCESS |
											FILE_NOTIFY_CHANGE_CREATION |
											FILE_NOTIFY_CHANGE_SECURITY,
						bool bSubTree = false) override;


protected:
	wxThread::ExitCode Entry() override;

private:
	CLocalWatcherDir  m_watchDir;
	unsigned long	  m_ulNotifyFilters;
	int	m_nLastError = 0;
	bool m_bRemove = false;
};

#endif // LOCALFILESYSTEMWATCHER_H_INCLUDED
