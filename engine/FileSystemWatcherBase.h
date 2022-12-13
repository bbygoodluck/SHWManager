#ifndef FILESYSTEMWATCHERBASE_H_INCLUDED
#define FILESYSTEMWATCHERBASE_H_INCLUDED

#include "../interface/WatcherQueue.h"
class CWatcherDir
{
public:
	CWatcherDir()
		: bSubTree(false), m_strDir(wxT(""))
	{
		wxZeroMemory(m_Buffer);
	}

	~CWatcherDir() {}

	enum
	{
		BUFFER_SIZE = 4096
	};

public:
	void* GetBuffer() { return m_Buffer; }

public:
	bool		bSubTree;
	wxString	m_strDir;
	char m_Buffer[BUFFER_SIZE];
};

class wxCustomFileSystemWatcherEvent;
wxDECLARE_EVENT(wxEVT_CUSTOM_FILESYSTEM_WATCHER, wxCustomFileSystemWatcherEvent);

class wxCustomFileSystemWatcherEvent : public wxEvent
{
public:
	wxCustomFileSystemWatcherEvent(int watchType, int watchid = wxID_ANY);
	wxCustomFileSystemWatcherEvent(const wxString& strOldName, const wxString& strNewName, int watchType, int watchid = wxID_ANY);

	virtual wxEvent* Clone() const wxOVERRIDE
	{
		wxCustomFileSystemWatcherEvent* evt = new wxCustomFileSystemWatcherEvent(*this);
		evt->m_watchType = m_watchType;
		evt->m_strOldName = m_strOldName.Clone();
		evt->m_strNewName = m_strNewName.Clone();

		return evt;
	}

	const wxString& GetOldName() const { return m_strOldName; }
	const wxString& GetNewName() const { return m_strNewName; }
	const int GetWatchType() const { return m_watchType; }

protected:
	int m_watchType;
	wxString m_strOldName;
	wxString m_strNewName;
};

typedef void (wxEvtHandler::*evtFileSystemWatcherFunction)(wxCustomFileSystemWatcherEvent&);
#define FileSystemWatcherEventHandler(func) wxEVENT_HANDLER_CAST(evtFileSystemWatcherFunction, func)
#define EVT_FSWATCH(winid, func) wx__DECLARE_EVT1(wxEVT_CUSTOM_FILESYSTEM_WATCHER, winid, FileSystemWatcherEventHandler(func))

class CFileSystemWatcherBase : public wxThreadHelper
{
protected:
	enum
	{
		BUFFER_SIZE = 4096
	};

public:
	CFileSystemWatcherBase(wxWindow* pOwner) : m_pOwner(pOwner)
	{
		if(!m_pWatcherQueue)
			m_pWatcherQueue = new CWatcherQueue();

		m_pWatcherQueue->SetOwner(pOwner);
	}
	virtual ~CFileSystemWatcherBase()
	{
		wxDELETE(m_pWatcherQueue);
	}

	virtual int	AddPath(const wxString&, unsigned long, bool) = 0;
	virtual void Clear() = 0;

	void SetOwner(wxWindow* owner)
	{
		m_pOwner = owner;
		m_pWatcherQueue->SetOwner(owner);
	}

	void SendEvent(wxCustomFileSystemWatcherEvent& evt);
	int Native2WatcherFlags(int flags);
	void ProcessWatchAction(const wxString& strOldName, const wxString& strFileName, int iAction);

	void SetWatcherQueue(CWatcherQueue* pWatcherQueue);

protected:
	virtual wxThread::ExitCode Entry() = 0;
	void DoWatchDirectory();
#if !defined(NDEBUG)
	void DoDisplayWatchLog(int iAction, const wxString& strOldName, const wxString& strNewName);
#endif

protected:
	wxWindow* m_pOwner;
	bool m_bRemove = false;

	CWatcherQueue* m_pWatcherQueue = nullptr;
};

#endif // FILESYSTEMWATCHERBASE_H_INCLUDED
