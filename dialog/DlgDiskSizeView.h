#ifndef DLGDISKSIZEVIEW_H_INCLUDED
#define DLGDISKSIZEVIEW_H_INCLUDED

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/dialog.h>
#include <wx/treelist.h>

enum
{
//    Col_Name = 0,
    Col_Files = 1,
    Col_Dirs,
    Col_Size
};

class MyComparator : public wxTreeListItemComparator
{
public:
	virtual int Compare(wxTreeListCtrl* treelist, unsigned column, wxTreeListItem item1, wxTreeListItem item2) wxOVERRIDE
	{
		wxString text1 = treelist->GetItemText(item1, column);
		wxString text2 = treelist->GetItemText(item2, column);

		switch ( column )
		{
	//	case Col_Name:
	//		return text1.CmpNoCase(text2);
			case Col_Files:
				return GetNumFilesFromText(text1) - GetNumFilesFromText(text2);

			case Col_Dirs:
				return GetNumFilesFromText(text1) - GetNumFilesFromText(text2);

			case Col_Size:
				int ret = GetSizeFromText(text1) - GetSizeFromText(text2);
				if(ret != 0)
					return ret;

				if(text1.CmpNoCase(text2) > 0)
					return 1;

				return 0;
		}

		wxFAIL_MSG( "Sorting on unknown column?" );
		return 0;
	}

private:
	// Return the number of files handling special value "many". Notice that
	// the returned value is signed to allow using it in subtraction above.
	int GetNumFilesFromText(const wxString& text) const
	{
		unsigned long n;
		if ( !text.ToULong(&n) )
		{
			if ( text == "many" )
				n = 9999;
			else
				n = 0;
		}

		return n;
	}

	// Return the size in KiB from a string with either KiB or MiB suffix.
	int GetSizeFromText(const wxString& text) const
	{
		wxString size(wxT(""));
		unsigned factor = 2;
/*
		if(text.Right(1).CmpNoCase(wxT("B")) == 0)
		{
			size = text.Left(text.Len() - 2);
			wxString strType = text.Right(2);
			if(strType.CmpNoCase(wxT("GB")) == 0)
				factor = 1024;
			else if(strType.CmpNoCase(wxT("MB")) == 0)
				factor = 128;
			else if(strType.CmpNoCase(wxT("KB")) == 0)
				factor = 64;
		}
		else
		{
			if(text.IsEmpty() || (text.CmpNoCase(wxT("0")) == 0) )
				return 1;

			size = text;
		}
*/

		if(text.EndsWith(wxT("GB"), &size) )
			factor = 1024;
		else if ( text.EndsWith(wxT("MB"), &size) )
			factor = 128;
		else if ( !text.EndsWith(wxT("KB"), &size) )
			factor = 64;
		else if(!text.IsEmpty())
		{
			size = text;
			if(text.CmpNoCase(wxT("0")) != 0)
				factor = 32;
			else
				factor = 16;
		}

		unsigned long n = size.length() + 1;
		return n * factor;
	}
};

class CDirInfoThread;
///////////////////////////////////////////////////////////////////////////////
/// Class DlgDiskSizeView
///////////////////////////////////////////////////////////////////////////////

class CVolumeSizeView;
class DlgDiskSizeView : public wxDialog
{
public:
	DlgDiskSizeView( wxWindow* parent, const wxSize& sz);
	~DlgDiskSizeView();

	class CVolumeInfo : public wxTreeItemData
	{
	public:
		CVolumeInfo(const wxString& strVolume)
			: _strVolume(strVolume)
		{

		}
		~CVolumeInfo() {}

	public:
		wxString _strVolume;
	};

public:
	enum
    {
        Icon_File,
        Icon_FolderClosed,
        Icon_FolderOpened,
        Icon_Drive_Item
    };

    wxTreeListCtrl* GetTreeListCtrl() { return m_treeLstSize; }
	void SetCurrentReadDirectory(const wxString& strCurrentDir);

private:
	void Init();
	void InitImageList();
	void CreateTreeImages(int size);
	void ProcessClose();
	void ReadDriveInfo(CVolumeInfo* pVolumeInfo);
	void ChageFileSize(wxTreeListItem* pTreeListItem);

protected:
	wxTreeItemId m_rootItem;

	wxTreeCtrl* m_treeCtrlDriveList;
	wxPanel* m_panelSelDisk;
	wxTreeListCtrl* m_treeLstSize;
	wxButton* m_btnClose;

	CVolumeSizeView* m_pVolumeSizeView = nullptr;
	wxImageList* m_imageList;

	CDirInfoThread* m_pThread = nullptr;
	wxFont m_DiskSizeViewFont;
	wxTreeListItem _root;

	MyComparator m_comparator;
	// Virtual event handlers, override them in your derived class
	virtual void OnInitDialog( wxInitDialogEvent& event );
	virtual void OnTreeSelChanged( wxTreeEvent& event );
	virtual void OnClose( wxCloseEvent& event );
	virtual void OnBtnClose( wxCommandEvent& event );
	virtual void OnTreeListItemExpanded( wxTreeListEvent& event );

public:
	enum
	{
		TreeCtrlIcon_Root = 0,
		TreeCtrlIcon_Item,
	};
};

#endif // DLGDISKSIZEVIEW_H_INCLUDED
