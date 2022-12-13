#ifndef __MY_TOOLTIP_VIEW_H_INCLUDED
#define __MY_TOOLTIP_VIEW_H_INCLUDED

#include "../interface/MemoryDC.h"
class CSHWTooltipView : public wxWindow
{
public:
	CSHWTooltipView(wxWindow* pParent);
	~CSHWTooltipView();

public:
	void SetTooltipText(const wxString& strTooltip);

private:
	void Render(wxDC* pDC);

private:
	CMemoryDC m_memDC;
	//화면 영역 크기
	wxRect m_viewRect = wxRect(0, 0, 0, 0);
	wxFont m_toolTipFont = wxNullFont;

	wxString m_strTooltip = wxT("");
private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnSize(wxSizeEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif
