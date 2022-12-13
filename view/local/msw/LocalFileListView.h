#ifndef LOCALLISTVIEW_H_INCLUDED
#define LOCALLISTVIEW_H_INCLUDED

#include "../../FileListView.h"
#include "../../ViewEventListener.h"
#include "../../../engine/msw/SHWLocalFileSystemWatcher.h"

class CLocalFileListView : public CFileListView, public IViewEventListener
{
public:
	CLocalFileListView(wxWindow* parent, const int nID, const wxSize& sz);
	virtual ~CLocalFileListView();

public:
	void LoadDirectory(const wxString& strPath);

private:
	virtual void Render(wxDC* pDC) override;
    virtual void ApplyChangedViewSize() override;
	//IViewEventListener
	virtual void DoCreate(CWatcherItem* pItem) override;
	virtual void DoModify(CWatcherItem* pItem) override;
	virtual void DoDelete(CWatcherItem* pItem) override;
	virtual void DoRename(CWatcherItem* pItem) override;

	void AddDrive();
	void RemoveDrive();
	void SetDriveSpace(const wxString& strVolume) override;

private:
	std::unique_ptr<CSHWLocalFileSystemWatcher> m_pFileSystemWatcher;

private:
	void OnFileSystemWatcher(wxCommandEvent& event) override;
	void OnDriveReload(wxCommandEvent& event);
	void OnDriveSpaceUpdate(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

#endif // LOCALLISTVIEW_H_INCLUDED
