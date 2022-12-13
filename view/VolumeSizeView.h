#ifndef VOLUMESIZEVIEW_H_INCLUDED
#define VOLUMESIZEVIEW_H_INCLUDED

#include "../interface/MemoryDC.h"

class CVolumeSizeView : public wxWindow
{
public:
	CVolumeSizeView(wxWindow* parent, const wxSize& size);
	~CVolumeSizeView();

public:
	void SetVolumeInfo(const wxString& strVolume);
	void RefreshVolumeInfo();

private:
	void Render(wxDC* pDC);

private:
	//화면 사이즈
    wxRect m_viewRect = wxRect(0, 0, 0, 0);
	CMemoryDC m_memDC;
	wxFont m_viewFont;

	wxString m_strVolume = wxT("");
	wxString m_strSpaceInfo = wxT("");
	double m_dblTotalSpace = 0.0f;
	double m_dblUsedSpace  = 0.0f;
	double m_dblFreeSpace  = 0.0f;

private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnSize(wxSizeEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif // VOLUMESIZEVIEW_H_INCLUDED
