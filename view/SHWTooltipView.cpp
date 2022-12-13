#include "../ginc.h"
#include "SHWToolTipView.h"

wxBEGIN_EVENT_TABLE(CSHWTooltipView, wxWindow)
	EVT_PAINT(CSHWTooltipView::OnPaint)
	EVT_ERASE_BACKGROUND(CSHWTooltipView::OnErase)
	EVT_SIZE(CSHWTooltipView::OnSize)
wxEND_EVENT_TABLE()

CSHWTooltipView::CSHWTooltipView(wxWindow* pParent)
	: wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, CHILD_WINDOW_STYLE)
{
#ifdef __WXMSW__
	m_toolTipFont.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Segoe UI"));
#else
	m_toolTipFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
#endif
}

CSHWTooltipView::~CSHWTooltipView()
{

}

void CSHWTooltipView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

	m_viewRect = GetClientRect();
	wxMemoryDC* pMemDC = m_memDC.CreateMemoryDC(&dc, m_viewRect, wxColour(0, 0, 0), wxColour(252, 242, 156));
	pMemDC->SetFont(m_toolTipFont);

	Render(pMemDC);
	dc.Blit(0, 0, m_viewRect.GetWidth(), m_viewRect.GetHeight(), pMemDC, 0, 0);
}

void CSHWTooltipView::Render(wxDC* pDC)
{
	pDC->SetTextBackground(wxColour(0, 0, 0));
	pDC->DrawLabel(m_strTooltip, m_viewRect, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
}

void CSHWTooltipView::OnErase(wxEraseEvent& event)
{

}

void CSHWTooltipView::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();
	m_memDC.ChangeViewSize(size);

	theUtility->RefreshWindow(this, m_viewRect);
}

void CSHWTooltipView::SetTooltipText(const wxString& strTooltip)
{
	m_strTooltip = strTooltip;
}
