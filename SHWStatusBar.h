#ifndef __CUSTOM_STATUS_BAR_H__
#define __CUSTOM_STATUS_BAR_H__

#include "./interface/MemoryDC.h"

class CSHWStatusBar : public wxStatusBar
{
	enum _DEFAULT
	{
		Field_F2 = 0,
		Field_F3,
		Field_F4,
		Field_F5,
	//	Field_F6,
		Field_F7,
		Field_F8,
		Field_F10,
		Field_Ctrl,
		Field_Shift,
		Field_Alt,
		Field_NumLockIndicator,
		Field_CapsLockIndicator,
		Field_Clock,
		Field_Max
	};

public:
	CSHWStatusBar(wxWindow *parent, long style = wxSTB_DEFAULT_STYLE);
	virtual ~CSHWStatusBar();

private:
	void Setting();
	void UpdateClock();
	void Render(wxDC* dc);
	void ExecuteStatusBarClick(const std::unordered_map<int, wxString>& _statusMap, int nIndex);

private:
	wxTimer m_timer;
	bool m_bSet;

	//화면 사이즈
    wxRect m_viewRect = wxRect(0, 0, 0, 0);
	CMemoryDC m_memDC;

	std::unordered_map<int, wxString> m_defaultMap;
	std::unordered_map<int, wxString> m_ctrlMap;
	std::unordered_map<int, wxString> m_altMap;

private:
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnTimer(wxTimerEvent& WXUNUSED(event));
	void OnMouseLBottonDown(wxMouseEvent& event);
	wxDECLARE_EVENT_TABLE();
};
#endif
