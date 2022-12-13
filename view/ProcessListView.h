#ifndef PROCESSVIEW_H_INCLUDED
#define PROCESSVIEW_H_INCLUDED

#include <wx/listctrl.h>
#include "../system/UpdateListener.h"

class CProcessListView : public wxListCtrl, public IUpdateListener
{
	struct sListColumnItem {
		wxString _strColumnName = wxT("");
		unsigned int _width = 0;
		wxListColumnFormat _align = wxLIST_FORMAT_LEFT;
	};

public:
	explicit CProcessListView(wxWindow* _parent);
	virtual ~CProcessListView();

public:
	void ShadowStart();
	void ShadowStop();

	bool IsAscending() { return m_bAscending; }
	int GetSortingColumn() { return m_nSortingColumn; }

private:
	void InitColumn();
	void InitList();
	void Update();
	void UpdateBackgroundColour(int _index, float _fCPUUsage);

	static int wxCALLBACK ListCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);

private:
	//Timer
	wxTimer m_timer;
	bool m_bAscending = true;
	int m_nSortingColumn = -1;
	unsigned long m_ulCurrentProcessId = 0;

private:
	void OnTimer(wxTimerEvent& event);
	virtual void UpdateListener(wxCommandEvent& event) override;
	wxDECLARE_EVENT_TABLE();
};

#endif // PROCESSVIEW_H_INCLUDED
