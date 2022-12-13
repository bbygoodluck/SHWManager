#include "../ginc.h"
#include "SHWFileSystemWatcherBase.h"
#include "SHWFileSystemWatcherListener.h"

CSHWFileSystemWatcherBase::CSHWFileSystemWatcherBase()
{
	if(m_pListener == nullptr)
		m_pListener = std::make_unique<CSHWFileSystemWatcherListener>();
}

CSHWFileSystemWatcherBase::CSHWFileSystemWatcherBase(wxWindow* pOwner)
{
	if(m_pListener == nullptr)
		m_pListener = std::make_unique<CSHWFileSystemWatcherListener>(pOwner);
}

CSHWFileSystemWatcherBase::~CSHWFileSystemWatcherBase()
{
}

void CSHWFileSystemWatcherBase::SetOwner(wxWindow* pOwner)
{
	if(m_pListener == nullptr)
		m_pListener = std::make_unique<CSHWFileSystemWatcherListener>();

	m_pListener->SetOwner(pOwner);
}

void CSHWFileSystemWatcherBase::RunWatchThread()
{
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxMessageBox(wxT("Could not create the worker thread!"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	GetThread()->Run();
}

int CSHWFileSystemWatcherBase::Native2WatcherFlags(int flags)
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
