#ifndef SHWLOCALFILESYSTEMWATCHER_H_INCLUDED
#define SHWLOCALFILESYSTEMWATCHER_H_INCLUDED

#include "../SHWFileSystemWatcherBase.h"

class CSHWLocalFileSystemWatcher : public CSHWFileSystemWatcherBase
{
public:
	CSHWLocalFileSystemWatcher();
	CSHWLocalFileSystemWatcher(wxWindow* pOwner);
	~CSHWLocalFileSystemWatcher();

	virtual void Clear() override;
	virtual bool StartWatch(const wxString& strPath) override;

	virtual wxThread::ExitCode Entry() override;

private:
	HANDLE m_hFile = NULL;
	OVERLAPPED	m_PollingOverlap;
	HANDLE		m_hIOCP = NULL;
	ULONG_PTR   m_CompletionKey = 0;

	unsigned long m_ulNotifyFilter;

	char m_buffer[4096] = {0x00, };
};

#endif // SHWLOCALFILESYSTEMWATCHER_H_INCLUDED
