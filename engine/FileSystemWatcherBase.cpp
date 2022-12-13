#include "../ginc.h"
#include "FileSystemWatcherBase.h"

wxDEFINE_EVENT(wxEVT_CUSTOM_FILESYSTEM_WATCHER, wxCustomFileSystemWatcherEvent);

wxCustomFileSystemWatcherEvent::wxCustomFileSystemWatcherEvent(int watchType, int watchid)
	: wxEvent(watchid, wxEVT_CUSTOM_FILESYSTEM_WATCHER)
	, m_watchType(watchType)
{
}

wxCustomFileSystemWatcherEvent::wxCustomFileSystemWatcherEvent(const wxString& strOldName, const wxString& strNewName, int watchType, int watchid)
	: wxEvent(watchid, wxEVT_CUSTOM_FILESYSTEM_WATCHER)
	, m_watchType(watchType)
	, m_strOldName(strOldName)
	, m_strNewName(strNewName)
{
}

void CFileSystemWatcherBase::ProcessWatchAction(const wxString& strOldName, const wxString& strFileName, int iAction)
{
	wxCustomFileSystemWatcherEvent evt(strOldName, strFileName, iAction);
	SendEvent(evt);
}

void CFileSystemWatcherBase::SendEvent(wxCustomFileSystemWatcherEvent& evt)
{
	wxQueueEvent(m_pOwner, evt.Clone());
}

int CFileSystemWatcherBase::Native2WatcherFlags(int flags)
{
	const int flag_mapping[][2] = {
		{ FILE_ACTION_ADDED,			FS_WATCHER_CREATE },
		{ FILE_ACTION_REMOVED,			FS_WATCHER_DELETE },
		{ FILE_ACTION_MODIFIED,			FS_WATCHER_MODIFY },
		{ FILE_ACTION_RENAMED_OLD_NAME, FS_WATCHER_RENAME_OLD },
		{ FILE_ACTION_RENAMED_NEW_NAME, FS_WATCHER_RENAME },
	};

	for (unsigned int i = 0; i < WXSIZEOF(flag_mapping); ++i)
	{
		if (flags == flag_mapping[i][0])
			return flag_mapping[i][1];
	}

	return -1;
}

#if !defined(NDEBUG)
void CFileSystemWatcherBase::DoDisplayWatchLog(int iAction, const wxString& strOldName, const wxString& strNewName)
{
	switch(iAction)
	{
		case FILE_ACTION_ADDED:
			theUtility->LogDisplay(wxT("FILE_ACTION_ADDED : ") + strNewName);
			break;

		case FILE_ACTION_REMOVED:
			theUtility->LogDisplay(wxT("FILE_ACTION_REMOVED : ") + strNewName);
			break;

		case FILE_ACTION_MODIFIED:
			theUtility->LogDisplay(wxT("FILE_ACTION_MODIFIED : ") + strNewName);
			break;

		case FILE_ACTION_RENAMED_NEW_NAME:
			theUtility->LogDisplay(wxT("FILE_ACTION_RENAMED_NEW_NAME : ") + strOldName + wxT(" -> ") + strNewName);
			break;

		default:
			break;
	}
}
#endif

void CFileSystemWatcherBase::DoWatchDirectory()
{
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxMessageBox(wxT("Could not create the worker thread!"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	GetThread()->Run();
}

void CFileSystemWatcherBase::SetWatcherQueue(CWatcherQueue* pWatcherQueue)
{
	if(m_pWatcherQueue != nullptr)
		return;

	m_pWatcherQueue = pWatcherQueue;
}
