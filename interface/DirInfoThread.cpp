#include "../ginc.h"
#include "../dialog/DlgSearchDir.h"
#include "../dialog/DlgDiskSizeView.h"
#include "../engine/LocalFileSystem.h"
#include "DirInfoThread.h"

#include <wx/busyinfo.h>
#include <filesystem>

CDirInfoThread::CDirInfoThread(DlgSearchDir* pDlg)
	: wxThread(wxTHREAD_JOINABLE)
	, m_pDlg(pDlg)

{

}

CDirInfoThread::~CDirInfoThread()
{

}

void CDirInfoThread::SetViewInfo(DlgDiskSizeView* pTreeListView, wxTreeListItem item, const wxString& strVolume)
{
	m_pTreeListDlg = pTreeListView;
	m_treeListItem = item;
	m_strVolume = strVolume;
}

wxThread::ExitCode CDirInfoThread::Entry()
{
	wxString strPath(m_strVolume);
#ifdef __WXMSW__
	strPath += wxT(":");
#endif // __WXMSW__

	m_dirCount = 0;
	m_pDlg->SetCurrentDir(strPath + SLASH);
	m_pDlg->SetReadDirectory(m_dirCount, strPath);
/*
	wxBusyInfo info
    (
        wxBusyInfoFlags()
            .Parent(m_pTreeListDlg)
            .Icon(wxArtProvider::GetIcon(wxART_EXECUTABLE_FILE,
                                         wxART_OTHER, wxSize(32, 32)))
            .Title("<b>Directory Reading...</b>")
            .Text("please wait...")
            .Foreground(*wxWHITE)
            .Background(wxColour(100, 100, 100))
            .Transparency(5 * (wxALPHA_OPAQUE / 5))
    );
*/
	DoGetDirInfo(&m_treeListItem, strPath);

	//Root 사이즈 변경
	double dblSize = 0.0f;

	wxString strDispFileSize(wxT(""));
	wxString strDispFileSizeType(wxT(""));
	wxColor col;

	dblSize = 0.0f;

	wxString strFileSize = GetTreeListCtrl()->GetItemText(m_treeListItem, 3);
	if(strFileSize.Right(1).CmpNoCase(wxT("B")) != 0)
	{
		strFileSize.ToDouble(&dblSize);

		if(dblSize != 0.0)
		{
			theUtility->GetSizeInfo(dblSize, strDispFileSize, strDispFileSizeType, col);
			strDispFileSize += strDispFileSizeType;

			GetTreeListCtrl()->SetItemText(m_treeListItem, 3, strDispFileSize);
		}
	}

	wxThreadEvent event(wxEVT_THREAD, SCD_DIRECTORY_SEARCH_THREAD_EVENT);
	event.SetInt(-1);
	wxQueueEvent(m_pDlg, event.Clone());

	GetTreeListCtrl()->Expand(m_treeListItem);

	 return (wxThread::ExitCode)0;
}

void CDirInfoThread::DoGetDirInfo(wxTreeListItem* pTreeListItem, const wxString& strPath)
{
	if (m_pDlg->IsCancel())
		return;

	CLocalFileSystem localFileSys;
	if (!localFileSys.BeginFindFiles(strPath, false))
		return;

	wxString strName(wxT(""));
	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	m_pDlg->SetReadDirectory(++m_dirCount, strPath);

	int nDirCount = 0;
	int nFileCount = 0;
	double dblFileSize = 0.0f;

	std::vector<wxString> vecDirs;

	while (localFileSys.GetNextFile(strName, &isDir, &lattr, &llSize, &dt))
	{
		if (m_pDlg->IsCancel())
			break;

		if (isDir && (strName.CmpNoCase(wxT("..")) == 0) )
			continue;

		if(isDir)
			vecDirs.push_back(strName);
		else
		{
			wxTreeListItem childItem = GetTreeListCtrl()->AppendItem(*pTreeListItem, strName, m_pTreeListDlg->Icon_File, m_pTreeListDlg->Icon_File);

			GetTreeListCtrl()->SetItemText(childItem, 1, wxT("1"));
			GetTreeListCtrl()->SetItemText(childItem, 2, wxT("0"));

			dblFileSize = llSize.ToDouble();
			GetTreeListCtrl()->SetItemText(childItem, 3, wxString::Format(wxT("%5.0f"), dblFileSize));

		}
	}

	if(vecDirs.size() > 0)
	{
		nDirCount = vecDirs.size();
		GetTreeListCtrl()->SetItemText(*pTreeListItem, 1, wxT("0"));
		GetTreeListCtrl()->SetItemText(*pTreeListItem, 2, wxString::Format(wxT("%d"), nDirCount));

		for(const auto& dirItem : vecDirs)
		{
			if (m_pDlg->IsCancel())
				break;

			wxTreeListItem childDir = GetTreeListCtrl()->AppendItem(*pTreeListItem, dirItem, m_pTreeListDlg->Icon_FolderClosed, m_pTreeListDlg->Icon_FolderOpened);

			wxString strNewPath(strPath);
			strNewPath += SLASH;
			strNewPath += dirItem;

			DoGetDirInfo(&childDir, strNewPath);
		}
	}

	if (m_pDlg->IsCancel())
		return;

	nFileCount = 0;
	dblFileSize = 0.0f;

	wxTreeListItem child = GetTreeListCtrl()->GetFirstChild(*pTreeListItem);
	wxTreeListItem sibiling = child;

	while(true)
	{
		if(!child.IsOk())
			break;

		int fCount = 0;
		int dCount = 0;
		double dblSize = 0.0f;

		wxString strFileCount = GetTreeListCtrl()->GetItemText(child, 1);
		wxString stDirCount = GetTreeListCtrl()->GetItemText(child, 2);
		wxString strFileSize = GetTreeListCtrl()->GetItemText(child, 3);

		strFileCount.ToInt(&fCount);
		stDirCount.ToInt(&dCount);
		strFileSize.ToDouble(&dblSize);

		nFileCount += fCount;
		nDirCount += dCount;
		dblFileSize += dblSize;

		sibiling = GetTreeListCtrl()->GetNextSibling(child);
		child = sibiling;
	}

	wxString strSelfFileCount = GetTreeListCtrl()->GetItemText(*pTreeListItem, 1);
	wxString strSelfFileSize = GetTreeListCtrl()->GetItemText(*pTreeListItem, 3);

	if(!strSelfFileCount.IsEmpty())
	{
		int nSelfFileCount = 0;
		double dblSelfFileSize = 0.0f;

		strSelfFileCount.ToInt(&nSelfFileCount);
		strSelfFileSize.ToDouble(&dblSelfFileSize);

		nFileCount += nSelfFileCount;
		dblFileSize += dblSelfFileSize;
	}

	GetTreeListCtrl()->SetItemText(*pTreeListItem, 1, wxString::Format(wxT("%d"), nFileCount));
	GetTreeListCtrl()->SetItemText(*pTreeListItem, 2, wxString::Format(wxT("%d"), nDirCount));
	GetTreeListCtrl()->SetItemText(*pTreeListItem, 3, wxString::Format(wxT("%5.0f"), dblFileSize));
}

wxTreeListCtrl* CDirInfoThread::GetTreeListCtrl()
{
	return m_pTreeListDlg->GetTreeListCtrl();
}

void CDirInfoThread::GetSubDirAndFileCount(const wxString& strPath, int* dircnt, int* filecnt)
{
#ifdef _UNICODE
	std::filesystem::path p(strPath.ToStdWstring());
#else
	std::filesystem::path p(strPath.ToStdString());
#endif
	using fp = bool (*)(const std::filesystem::path &);

	*dircnt = std::count_if(std::filesystem::directory_iterator{p}, std::filesystem::directory_iterator{}, (fp)std::filesystem::is_directory);
	*filecnt = std::count_if(std::filesystem::directory_iterator{p}, std::filesystem::directory_iterator{}, (fp)std::filesystem::is_regular_file);
}
