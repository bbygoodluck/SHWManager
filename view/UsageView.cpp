#include "../ginc.h"
#include "UsageView.h"
#include "../system/system.h"

wxBEGIN_EVENT_TABLE(CUsageView, wxWindow)
	EVT_PAINT(CUsageView::OnPaint)
	EVT_SIZE(CUsageView::OnSize)
	EVT_ERASE_BACKGROUND(CUsageView::OnErase)
	EVT_MY_CUSTOM_COMMAND(wxEVT_RESOURCE_MONITORING, wxID_ANY, CUsageView::UpdateListener)
wxEND_EVENT_TABLE()

CUsageView::CUsageView(wxWindow* parent, const int nID, const wxSize& sz, USAGE_TYPE usage_type, long lStyle)
	: wxWindow(parent, nID, wxDefaultPosition, sz, lStyle)
	, e_usage_type(usage_type)
{
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(CUsageView::OnTimer));
	m_timer.SetOwner(this, wxTIMER_ID++);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

CUsageView::~CUsageView()
{
	ShadowStop();
	ClearData();

	wxVector<SPECTRUM_DATA *>().swap(m_Datas);
	m_Datas.reserve(0);

	this->Disconnect(wxEVT_TIMER, wxTimerEventHandler(CUsageView::OnTimer));
}

void CUsageView::OnErase(wxEraseEvent& event)
{

}

void CUsageView::OnSize(wxSizeEvent& event)
{
	_SevenSegment.ResetCreatedSevenSegment();
	_bCreatedSpectrum = false;

	wxSize size = event.GetSize();
	m_memDC.ChangeViewSize(size);

    theUtility->RefreshWindow(this, m_viewRect);
}

void CUsageView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

    m_viewRect = GetClientRect();

    m_viewRectSpectrum = m_viewRect;
    if(bViewSegment_)
	{
		int nSegmentWidth = 15 * m_viewRect.GetWidth() / 100;
		m_viewRectSpectrum.SetRight(m_viewRect.GetRight() - nSegmentWidth);

		m_viewRectSegment.SetLeft(m_viewRectSpectrum.GetRight() + _gap);
		m_viewRectSegment.SetTop(m_viewRect.GetTop() + 1);
		m_viewRectSegment.SetRight(m_viewRect.GetRight() - _gap);
		m_viewRectSegment.SetBottom(m_viewRect.GetBottom() - 1);
	}

	wxMemoryDC* pMemDC = m_memDC.CreateMemoryDC(&dc, m_viewRect);

	if(!_bCreatedSpectrum)
		CreateSpectrumData(pMemDC);

	if(bViewSegment_)
	{
		if(!_SevenSegment.IsCreatedSevenSegment())
			_SevenSegment.CreateSevenSegment(pMemDC, m_viewRectSegment);
	}

	if(_nShadowIndex == 0 || _nShadowIndex < 0)
	{
		_nShadowIndex = 0;
		_bShadowForward = true;
		_nShadowCalcIndex = 0;
		_nShadowColIndex = 0;
	}
	else if(_nShadowIndex >= m_SpectrumDatas && _nShadowIndex != 0)
	{
		_nShadowCalcIndex = 0;
		_nShadowIndex = m_SpectrumDatas;
		_bShadowForward = false;
		_nShadowColIndex = 0;
	}

	Render(pMemDC);

	dc.Blit(0, 0, m_viewRect.GetWidth(), m_viewRect.GetHeight(), pMemDC, 0, 0);
}

void CUsageView::ClearData()
{
	for (auto item : m_Datas)
	{
		SPECTRUM_DATA* pData = item;
		wxDELETE(pData);
	}

	m_Datas.clear();
}

void CUsageView::Render(wxDC* pDC)
{
	if(m_Datas.size() == 0)
		return;

	pDC->SetFont(*_gViewFont);

	DrawSpectrum(pDC);
	DrawUsageSpectrum(pDC);
//	DrawShadow(pDC);

	if(bViewSegment_)
		_SevenSegment.DrawSevenSegment(pDC, m_UsagePercent);
}

void CUsageView::CreateSpectrumData(wxDC* pDC)
{
	ClearData();

	int iXStartPos = 0;
	int iXEndPos = _gap;
	int iWidth = 3;
	int iHeight = m_viewRectSpectrum.GetHeight() - _gap;
	int iRed = 0;
	int iGreen = 0;

	m_SpectrumDatas = 0;

	int nRightPos = m_viewRectSpectrum.GetRight() - _gap;
	while (iXStartPos < nRightPos)
	{
		iXStartPos = iXEndPos;
		wxRect rc(iXStartPos, 2, iWidth, iHeight);

		if(rc.GetRight() > nRightPos)
			break;

		SPECTRUM_DATA* pSpectrumInfo = new SPECTRUM_DATA();
		pSpectrumInfo->_rect = rc;

		m_Datas.push_back(pSpectrumInfo);
		iXEndPos = rc.GetRight() + _gap;
	}

	m_SpectrumDatas = m_Datas.size();
	int nIndex = 1;

	wxVector<SPECTRUM_DATA *>::const_iterator iter = nullptr;
	for(iter = m_Datas.begin(); iter != m_Datas.end(); ++iter)
	{
		SPECTRUM_DATA* pData = *iter;

		iRed = 1 + ((255 * nIndex) / m_SpectrumDatas);
		iRed = iRed > 255 ? 255 : iRed;

		iGreen = 255 - ((255 * nIndex) / m_SpectrumDatas);
		iGreen = iGreen < 0 ? 0 : iGreen;

		wxColour col(iRed, iGreen, 0);
		pData->_col = col;

		nIndex++;
	}

	_bCreatedSpectrum = true;
}

void CUsageView::DrawSpectrum(wxDC* pDC)
{
	wxVector<SPECTRUM_DATA *>::const_iterator iter = nullptr;
	for(iter = m_Datas.begin(); iter != m_Datas.end(); ++iter)
	{
		SPECTRUM_DATA* pData = *iter;
		wxPen pen(wxColour(_redBack, _greenBack, _blueBack));
		wxBrush brush(wxColour(_redBack, _greenBack, _blueBack));

		pDC->SetPen(pen);
		pDC->SetBrush(brush);

		pDC->DrawRectangle(pData->_rect);
	}
}
void CUsageView::DrawUsageSpectrum(wxDC* pDC)
{
	if(m_SpectrumDatas == 0)
		return;

	for(unsigned int index = 0; index < m_UsageCount; index++)
	{
		SPECTRUM_DATA* pData = m_Datas.at(index);
		wxPen pen(pData->_col);
		wxBrush brush(pData->_col);

		pDC->SetPen(pen);
		pDC->SetBrush(brush);

		pDC->DrawRectangle(pData->_rect);
	}

	wxRect viewRectShadow = m_viewRectSpectrum;
	viewRectShadow.SetLeft(m_viewRectSpectrum.GetLeft() + 1);
	viewRectShadow.SetTop(m_viewRectSpectrum.GetTop() + 1);

	pDC->SetTextForeground(wxColour(128, 128, 128));
	pDC->DrawLabel(_strUsage, viewRectShadow, wxALIGN_CENTER);

	pDC->SetTextForeground(wxColour(250, 250, 250));
	pDC->DrawLabel(_strUsage, m_viewRectSpectrum, wxALIGN_CENTER);
}

void CUsageView::DrawShadow(wxDC* pDC)
{
	wxVector<SPECTRUM_DATA *>::const_iterator citer = nullptr;

	if(_bShadowForward)
	{
		citer = m_Datas.begin() + _nShadowIndex;
		_nShadowIndex++;
	}
	else
	{
		_nShadowIndex--;
		citer = m_Datas.begin() + _nShadowIndex;
	}

	SPECTRUM_DATA* pData = *citer;
	if(pData == nullptr)
		return;

	unsigned int nCenter = m_SpectrumDatas / 2;
	if(_nShadowCalcIndex < nCenter)
		_nShadowColIndex++;
	else
	{
		_nShadowColIndex--;
		if(_nShadowColIndex < 0)
			_nShadowColIndex = 0;
	}

	const int maxcolor = 192;
	int nRed   = _redBack + (maxcolor * _nShadowColIndex) / nCenter;
	nRed = nRed >= maxcolor ? maxcolor : nRed;

	int nGreen = _greenBack + (maxcolor * _nShadowColIndex) / nCenter;
	nGreen = nGreen >= maxcolor ? maxcolor : nGreen;

	int nBlue  = _blueBack + (maxcolor * _nShadowColIndex) / nCenter;
	nBlue = nBlue >= maxcolor ? maxcolor : nBlue;

	wxColour col(nRed, nGreen, 0);

	wxPen pen(col);
	wxBrush brush(col);

	pDC->SetPen(pen);
	pDC->SetBrush(brush);
	pDC->DrawRectangle(pData->_rect);

	_nShadowCalcIndex++;
}

void CUsageView::UpdateListener(wxCommandEvent& event)
{
	if(e_usage_type == USAGE_TYPE::USAGE_NONE)
		return;

	_bSendedEvent = true;
	switch(e_usage_type)
	{
		case USAGE_TYPE::USAGE_CPU:
			CPUUsage();
			break;

		case USAGE_TYPE::USAGE_MEMORY:
			MemoryUsage();
			break;

		case USAGE_TYPE::USAGE_PAGEFILE:
			PageFileUsage();
			break;

		default:
			break;
	}

	theUtility->RefreshWindow(this, m_viewRect);
}

void CUsageView::OnTimer(wxTimerEvent& event)
{
	theUtility->RefreshWindow(this, m_viewRect);
}

void CUsageView::CPUUsage()
{
	unsigned long ulUsageData = theSystem->GetCPUInfo()->GetCPUUsage();
	m_UsagePercent = ulUsageData;

	_strUsage = wxString::Format(wxT("CPU : %u%"), m_UsagePercent);
	CalcUsage();
}

void CUsageView::MemoryUsage()
{
	unsigned long ulTotalMem = theSystem->GetMemoryInfo()->GetTotalMemory();
	unsigned long ulUsedMem = theSystem->GetMemoryInfo()->GetUsedMemory();

	wxString strFormat = wxT("Memory(%u%) : %.2f GB / %.2f GB");
	MemoryCalc(strFormat, ulTotalMem, ulUsedMem);
}

void CUsageView::PageFileUsage()
{
	unsigned long ulTotalPageFile = theSystem->GetMemoryInfo()->GetTotalPageFile();
	unsigned long ulUsedPageFile = theSystem->GetMemoryInfo()->GetUsedPageFile();

	wxString strFormat = wxT("Page File(%u%) : %.2f GB / %.2f GB");
	MemoryCalc(strFormat, ulTotalPageFile, ulUsedPageFile);
}

void CUsageView::MemoryCalc(const wxString& strFormat, unsigned long _ulTotal, unsigned long _ulUsed)
{
	m_UsagePercent = wx_static_cast(unsigned int, (_ulUsed  * 100) / _ulTotal);

	float fTotalGB = (float)_ulTotal / 1024.0f;
	float fUsedeGB = (float)_ulUsed / 1024.0f;

	_strUsage = wxString::Format(strFormat, m_UsagePercent, fUsedeGB, fTotalGB);
	CalcUsage();
}

void CUsageView::CalcUsage()
{
	m_UsageCount = (unsigned int)(m_UsagePercent * m_SpectrumDatas) / 100;
}

void CUsageView::ShadowStart()
{
//	m_timer.Start(10);
}

void CUsageView::ShadowStop()
{
	m_timer.Stop();
}
