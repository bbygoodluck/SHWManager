#ifndef DLGRESOURCEUSAGE_H_INCLUDED
#define DLGRESOURCEUSAGE_H_INCLUDED

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/statline.h>
#include <wx/listctrl.h>

///////////////////////////////////////////////////////////////////////////////
/// Class DlgResourceUsage
///////////////////////////////////////////////////////////////////////////////

class CUsageView;
class CGraphView;
class CNetworkUsageGraphView;
class CProcessListView;

class DlgResourceUsage : public wxDialog
{
public:
	DlgResourceUsage( wxWindow* parent, const wxSize& sz);
	~DlgResourceUsage();

protected:
	wxStaticBitmap* m_bmpProcessor;
	wxStaticText* m_staticText2;
	wxStaticText* m_staticProcessor;
	wxStaticLine* m_staticline1;
	wxStaticLine* m_staticline2;
	wxStaticLine* m_staticline3;
	wxStaticText* m_staticNetInfo;
	wxStaticText* m_staticText51;
	wxStaticText* m_staticText6;
	wxButton* m_btnClose;

	wxStaticText* m_staticText7;

//	wxListCtrl* m_lstProcess;

	CUsageView* m_pUsageCPUView                 = nullptr;
	CUsageView* m_pUsageMemoryView              = nullptr;
	CUsageView* m_pUsagePageFileView            = nullptr;
	CNetworkUsageGraphView* m_pNetworkUsageView = nullptr;
	CProcessListView* m_pProcessListView        = nullptr;

private:
	void CreateCPUCoreList(wxSizer* pSizer);

private:
	virtual void OnInitDialog( wxInitDialogEvent& event );
	virtual void OnClose( wxCommandEvent& event );
};

#endif // DLGRESOURCEUSAGE_H_INCLUDED
