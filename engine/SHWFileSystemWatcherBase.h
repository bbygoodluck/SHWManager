#ifndef SHWFILEWATCHERBASE_H_INCLUDED
#define SHWFILEWATCHERBASE_H_INCLUDED

#include "SHWFileSystemWatcherListener.h"
class CSHWFileSystemWatcherBase : public wxThreadHelper
{
protected:
	const int BUFFER_SIZE = 4096;

public:
	CSHWFileSystemWatcherBase();
	CSHWFileSystemWatcherBase(wxWindow* pOwner);
	~CSHWFileSystemWatcherBase();

	wxString GetWatchPath() { return m_strPath; }
	void SetOwner(wxWindow* pOwner);

	virtual bool StartWatch(const wxString& strPath) = 0;
	virtual void Clear() = 0;
	virtual wxThread::ExitCode Entry() = 0;

protected:
	void RunWatchThread();
	int Native2WatcherFlags(int flags);

protected:
	wxString m_strPath = wxT("");
	bool m_bRemove = false;
	wxWindow* m_pOwner = nullptr;
	std::unique_ptr<CSHWFileSystemWatcherListener> m_pListener = nullptr;
};

#endif // SHWFILEWATCHERBASE_H_INCLUDED
