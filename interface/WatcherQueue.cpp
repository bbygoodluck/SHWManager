#include "../ginc.h"
#include "WatcherItem.h"
#include "WatcherQueue.h"

CWatcherQueue::CWatcherQueue()
{
	Init();
}

CWatcherQueue::~CWatcherQueue()
{
	m_IsTerminate = true;
	m_IsReset = true;

	if (GetThread() && GetThread()->IsRunning())
	{
#ifdef __WXMSW__
	SetEvent(hEvent);
#endif // __WXMSW__

		GetThread()->Wait();
	}

	Clear();
}

void CWatcherQueue::Init()
{
	m_pCondition = std::make_unique<wxCondition>(m_mutex);

#ifdef __WXMSW__
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif

	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxMessageBox(wxT("Could not create the worker thread!"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	GetThread()->Run();
}

void CWatcherQueue::Clear()
{
	std::queue<CWatcherItem>().swap(m_Queue);
	m_IsReset = false;
}

void CWatcherQueue::Run()
{
#ifdef __WXMSW__
	SetEvent(hEvent);
#else
#endif
}

void CWatcherQueue::Lock()
{
	m_pCondition->Wait();
}

void CWatcherQueue::UnLock()
{
	m_pCondition->Broadcast();
}

void CWatcherQueue::AddQueue(const wxString& strOld, const wxString& strNew, int WatchAction)
{
	if(m_pOwner == nullptr)
		return;

	CWatcherItem item;

	item.m_strOld = strOld;
	item.m_strNew = strNew;
	item.m_Action = WatchAction;

	m_Queue.push(item);
}

void CWatcherQueue::GetQueue()
{

}

void CWatcherQueue::Reset()
{
	m_IsReset = true;

	Clear();
//	UnLock();
}

void CWatcherQueue::SetOwner(wxWindow* pWindow)
{
	m_pOwner = pWindow;
}

wxThread::ExitCode CWatcherQueue::Entry()
{
	while(1)
	{
#ifdef __WXMSW__
		WaitForSingleObject(hEvent, INFINITE);
#else

#endif
		if(m_IsTerminate)
			break;

		while(!m_Queue.empty())
		{
			if(m_IsReset)
				break;

			CWatcherItem item = m_Queue.front();

			wxCommandEvent evt(wxEVT_FS_WATCHER);
			evt.SetClientData(item.Clone());
			wxQueueEvent(m_pOwner, evt.Clone());

		//	Lock();
			m_Queue.pop();
		}

		m_IsRunning = false;
		m_IsReset = false;

#ifdef __WXMSW__
		ResetEvent(hEvent);
#else
#endif // __WXMSW__
	}

	return (wxThread::ExitCode)0;
}
