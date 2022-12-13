#include "../ginc.h"
#include "VolumeSizeView.h"

wxBEGIN_EVENT_TABLE(CVolumeSizeView, wxWindow)
	EVT_PAINT(CVolumeSizeView::OnPaint)
	EVT_SIZE(CVolumeSizeView::OnSize)
wxEND_EVENT_TABLE()

CVolumeSizeView::CVolumeSizeView(wxWindow* parent, const wxSize& size)
	: wxWindow(parent, wxID_ANY, wxDefaultPosition, size, CHILD_WINDOW_STYLE)
{
	m_viewFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FIXED_FONT);
}

CVolumeSizeView::~CVolumeSizeView()
{

}

void CVolumeSizeView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

	m_viewRect = GetClientRect();

	wxMemoryDC* pMemDC = m_memDC.CreateMemoryDC(&dc, m_viewRect);

	pMemDC->SetFont(m_viewFont);
	pMemDC->DrawRectangle(m_viewRect);

	Render(pMemDC);

	dc.Blit(0, 0, m_viewRect.GetWidth(), m_viewRect.GetHeight(), pMemDC, 0, 0);
}

void CVolumeSizeView::OnErase(wxEraseEvent& event)
{

}

void CVolumeSizeView::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();
	m_memDC.ChangeViewSize(size);

    theUtility->RefreshWindow(this, m_viewRect);
}

void CVolumeSizeView::Render(wxDC* pDC)
{
	double dblTotalWidth = m_viewRect.GetWidth() * 1.0;
	double dblUsed = m_dblTotalSpace - m_dblFreeSpace;

	int iUsedWidth = (int)((dblUsed * dblTotalWidth) / m_dblTotalSpace);
	wxRect rcDiskSpaceUsed(m_viewRect.GetLeft() + 1 , m_viewRect.GetTop() + 1, iUsedWidth, m_viewRect.GetHeight() - 1);

	int iRed = 44 + (211 * iUsedWidth / dblTotalWidth);
	int iGreen = 150 - (105 * iUsedWidth / dblTotalWidth);
	int iBlue = 150 -  (105 * iUsedWidth / dblTotalWidth);

	iRed = iRed > 255 ? 255 : iRed;
	iGreen = iGreen < 0 ? 0 : iGreen;
	iBlue = iBlue < 0 ? 0 : iBlue;

	pDC->GradientFillLinear(rcDiskSpaceUsed, wxColour(44, 184, 65), wxColour(iRed , iGreen , iBlue));

	if(!m_strVolume.IsEmpty())
	{
		wxString strUsed = wxString::Format(wxT("%5.1f GB"), (dblUsed / GIGABYTE)) + theMsgManager->GetMessage(wxT("MSG_DISK_USED"));

		pDC->SetTextForeground(wxColour(255, 255, 255));
		wxString strDisp = theMsgManager->GetMessage(wxT("MSG_DISK_SELECTED"));
		strDisp += m_strVolume.Left(1).CmpNoCase(SLASH) == 0 ?
		           m_strVolume + wxT(" (") + strUsed + wxT(") - ") :
		           m_strVolume + wxT(":") + SLASH + wxT(" (") + strUsed + wxT(") - ");

		strDisp += m_strSpaceInfo;

		pDC->DrawLabel(strDisp, m_viewRect, wxALIGN_CENTER);
	}

	wxPen penLine(wxColour(240, 240, 240));
	pDC->SetPen(penLine);

	pDC->DrawLine(m_viewRect.GetLeft(), m_viewRect.GetBottom(), m_viewRect.GetRight() + 1, m_viewRect.GetBottom());
	pDC->DrawLine(m_viewRect.GetRight(), m_viewRect.GetTop(), m_viewRect.GetRight(), m_viewRect.GetBottom());
}

void CVolumeSizeView::SetVolumeInfo(const wxString& strVolume)
{
	m_strVolume = strVolume;
}

void CVolumeSizeView::RefreshVolumeInfo()
{
	if(m_strVolume.IsEmpty())
		return;

	m_dblTotalSpace = 0.0f;
	m_dblFreeSpace  = 0.0f;

	CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(m_strVolume);
	pDriveItem->GetDriveSpace(m_dblTotalSpace, m_dblFreeSpace);

	m_strSpaceInfo = pDriveItem->GetDriveSpace();

	theUtility->RefreshWindow(this, m_viewRect);
}
