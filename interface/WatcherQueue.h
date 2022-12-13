#ifndef WATCHERQUEUE_H_INCLUDED
#define WATCHERQUEUE_H_INCLUDED

class CWatcherItem;
class CWatcherQueue : public wxThreadHelper
{
public:
	CWatcherQueue();
	~CWatcherQueue();

public:
	void AddQueue(const wxString& strOld, const wxString& strNew, int WatchAction);
	void GetQueue();

	void SetOwner(wxWindow* pWindow);
	void Run();
	void Reset();
	void Lock();
	void UnLock();

private:
	void Init();
	void Clear();

private:
	wxWindow* m_pOwner = nullptr;
	std::queue<CWatcherItem> m_Queue;

	wxMutex m_mutex;
	std::unique_ptr<wxCondition> m_pCondition = nullptr;

#ifdef __WXMSW__
	HANDLE hEvent;
#endif

	bool m_IsRunning   = false;
	bool m_IsReset     = false;
	bool m_IsTerminate = false;

protected:
	wxThread::ExitCode Entry() override;
};

#endif // WATCHERQUEUE_H_INCLUDED
