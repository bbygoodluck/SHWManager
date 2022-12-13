#include "../ginc.h"
#include "SHWFileSystemWatcherListener.h"
#include "../interface/WatcherItem.h"

CSHWFileSystemWatcherListener::CSHWFileSystemWatcherListener()
{

}

CSHWFileSystemWatcherListener::CSHWFileSystemWatcherListener(wxWindow* pOwner)
	: m_pOwner(pOwner)
{

}

CSHWFileSystemWatcherListener::~CSHWFileSystemWatcherListener()
{

}

void CSHWFileSystemWatcherListener::OnFileChange(const wxString& strName)
{
	CWatcherItem item;

	item.m_strOld = wxT("");
	item.m_strNew = strName;
	item.m_Action = FS_WATCHER_MODIFY;

	wxCommandEvent evt(wxEVT_FS_WATCHER);
	evt.SetClientData(item.Clone());
	wxQueueEvent(m_pOwner, evt.Clone());
}

void CSHWFileSystemWatcherListener::OnFileAdded(const wxString& strName)
{
	CWatcherItem item;

	item.m_strOld = wxT("");
	item.m_strNew = strName;
	item.m_Action = FS_WATCHER_CREATE;

	wxCommandEvent evt(wxEVT_FS_WATCHER);
	evt.SetClientData(item.Clone());
	wxQueueEvent(m_pOwner, evt.Clone());
}

void CSHWFileSystemWatcherListener::OnFileRemoved(const wxString& strName)
{
	CWatcherItem item;

	item.m_strOld = wxT("");
	item.m_strNew = strName;
	item.m_Action = FS_WATCHER_DELETE;

	wxCommandEvent evt(wxEVT_FS_WATCHER);
	evt.SetClientData(item.Clone());
	wxQueueEvent(m_pOwner, evt.Clone());
}

void CSHWFileSystemWatcherListener::OnFileRenamed(const wxString& strOldName, const wxString& strNewName)
{
	CWatcherItem item;

	item.m_strOld = strOldName;
	item.m_strNew = strNewName;
	item.m_Action = FS_WATCHER_RENAME;

	wxCommandEvent evt(wxEVT_FS_WATCHER);
	evt.SetClientData(item.Clone());
	wxQueueEvent(m_pOwner, evt.Clone());
}
