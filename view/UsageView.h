#ifndef USAGEVIEW_H_INCLUDED
#define USAGEVIEW_H_INCLUDED

#include "../interface/MemoryDC.h"
#include "../interface/SevenSegment.h"
#include "../system/UpdateListener.h"

class CUsageView : public wxWindow, public IUpdateListener
{
	typedef struct _SPECTRUM_DATA {
		wxRect _rect;
		wxColor _col;
	} SPECTRUM_DATA;

public:
	explicit CUsageView(wxWindow* parent, const int nID, const wxSize& sz, USAGE_TYPE usage_type, long lStyle = CHILD_WINDOW_STYLE);
	~CUsageView();

public:
	void ShadowStart();
	void ShadowStop();

private:
	void Render(wxDC* pDC);
	void CreateSpectrumData(wxDC* pDC);
	void DrawSpectrum(wxDC* pDC);
	void DrawUsageSpectrum(wxDC* pDC);
	void DrawShadow(wxDC* pDC);

	void ClearData();

	void CPUUsage();
	void MemoryUsage();
	void PageFileUsage();
	void MemoryCalc(const wxString& strFormat, unsigned long _ulTotal, unsigned long _ulUsed);
	void CalcUsage();

private:
	//화면 사이즈
    wxRect m_viewRect = wxRect(0, 0, 0, 0);
    wxRect m_viewRectSpectrum;
    wxRect m_viewRectSegment;
    //MemoryDC
	CMemoryDC m_memDC;
	//Timer
	wxTimer m_timer;
	//Spectrum create flag
	bool _bCreatedSpectrum = false;
	wxVector<SPECTRUM_DATA *> m_Datas;

	const int _gap = 3;
	unsigned int m_SpectrumDatas = 0;
	unsigned int m_UsageCount = 0;
	unsigned long m_UsagePercent = 0;
	wxString _strUsage = wxT("");

	const unsigned int _redBack   = 80;
	const unsigned int _greenBack = 80;
	const unsigned int _blueBack  = 80;

	enum USAGE_TYPE e_usage_type = USAGE_NONE;

	CSevenSegment _SevenSegment;
	bool bViewSegment_ = false;
	unsigned int _nShadowIndex = 0;
	unsigned int _nShadowCalcIndex = 0;
	unsigned int _nShadowColIndex = 1;

	bool _bShadowForward = true;
	bool _bSendedEvent = false;

private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnTimer(wxTimerEvent& event);

	virtual void UpdateListener(wxCommandEvent& event) override;
	wxDECLARE_EVENT_TABLE();
};

#endif // USAGEVIEW_H_INCLUDED
