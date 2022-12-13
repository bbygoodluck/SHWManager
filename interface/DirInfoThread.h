#ifndef DIRINFOTHREAD_H_INCLUDED
#define DIRINFOTHREAD_H_INCLUDED

class DlgSearchDir;
class DlgDiskSizeView;

#include <wx/treelist.h>
class CDirInfoThread : public wxThread
{
public:
	CDirInfoThread(DlgSearchDir* pDlg);
	~CDirInfoThread();

public:
	void SetViewInfo(DlgDiskSizeView* pTreeListView, wxTreeListItem item, const wxString& strVolume);

protected:
	virtual wxThread::ExitCode Entry() wxOVERRIDE;

private:
	void DoGetDirInfo(wxTreeListItem* pTreeListItem, const wxString& strPath);
	void GetSubDirAndFileCount(const wxString& strPath, int* dircnt, int* filecnt);

	wxTreeListCtrl* GetTreeListCtrl();

private:
	wxTreeListItem m_treeListItem;
	wxString m_strVolume = wxT("");
	DlgSearchDir* m_pDlg = nullptr;
	DlgDiskSizeView* m_pTreeListDlg = nullptr;

	int m_dirCount = 0;
};

#endif // DIRINFOTHREAD_H_INCLUDED
