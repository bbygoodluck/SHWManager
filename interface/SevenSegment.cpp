#include "../ginc.h"
#include "SevenSegment.h"

CSevenSegment::CSevenSegment()
{

}
CSevenSegment::~CSevenSegment()
{

}

void CSevenSegment::CreateSevenSegment(wxDC* pDC, const wxRect& viewRect)
{
	_bCreatedSegment = true;

	wxPen pen(wxColor(0, 255, 0));
	wxBrush brush(wxColor(0, 0, 0));

	pDC->SetPen(pen);
	pDC->SetBrush(brush);

	int nRectWidth = (viewRect.GetWidth() - (_padding * 2)) / 3;

	wxRect rc100Area(viewRect.GetLeft(), viewRect.GetTop() + 1, nRectWidth, viewRect.GetHeight());
	wxRect rc10Area(rc100Area.GetRight() + _padding * 2, viewRect.GetTop() + 1, nRectWidth, viewRect.GetHeight());
	wxRect rc1Area(rc10Area.GetRight() + _padding * 2 , viewRect.GetTop() + 1, nRectWidth, viewRect.GetHeight());

	const int countWH = 5;
	int nSegmentW = rc100Area.GetWidth() / countWH;
	int nSegmentH = rc100Area.GetHeight() / countWH;

	int nSegmentCenter = viewRect.GetHeight() / 2;
	//100단위
	_rcA100.SetLeft(rc100Area.GetLeft() + nSegmentW);
	_rcA100.SetTop(rc100Area.GetTop());
	_rcA100.SetRight(rc100Area.GetRight() - nSegmentW);
	_rcA100.SetBottom(rc100Area.GetTop() + nSegmentH - _padding);
//	pDC->DrawRectangle(_rcA100);

	_rcB100.SetLeft(_rcA100.GetRight() + _padding);
	_rcB100.SetTop(_rcA100.GetBottom() + _padding);
	_rcB100.SetRight(rc100Area.GetRight());
	_rcB100.SetBottom(rc100Area.GetTop() + nSegmentCenter - _padding);
//	pDC->DrawRectangle(_rcB100);

	_rcC100.SetLeft(_rcB100.GetLeft());
	_rcC100.SetTop(_rcB100.GetBottom() + _padding);
	_rcC100.SetRight(rc100Area.GetRight());
	_rcC100.SetBottom(rc100Area.GetBottom() - nSegmentH - _padding);
//	pDC->DrawRectangle(_rcC100);

	_rcD100.SetLeft(_rcA100.GetLeft());
	_rcD100.SetTop(_rcC100.GetBottom() + _padding);
	_rcD100.SetRight(_rcA100.GetRight());
	_rcD100.SetBottom(rc100Area.GetBottom() - _padding);
//	pDC->DrawRectangle(_rcD100);

	_rcE100.SetLeft(rc100Area.GetLeft());
	_rcE100.SetTop(_rcC100.GetTop());
	_rcE100.SetRight(_rcA100.GetLeft() - _padding);
	_rcE100.SetBottom(_rcC100.GetBottom());
//	pDC->DrawRectangle(_rcE100);

	_rcF100.SetLeft(rc100Area.GetLeft());
	_rcF100.SetTop(_rcB100.GetTop());
	_rcF100.SetRight(_rcA100.GetLeft() - _padding);
	_rcF100.SetBottom(_rcB100.GetBottom());
//	pDC->DrawRectangle(_rcF100);

	_rcG100.SetLeft(_rcA100.GetLeft() + _padding);
	_rcG100.SetTop(_rcF100.GetBottom() + 1);
	_rcG100.SetRight(_rcA100.GetRight() - _padding);
	_rcG100.SetBottom(_rcG100.GetTop() + nSegmentH - _padding);
//	pDC->DrawRectangle(_rcG100);

	//10단위
	_rcA10.SetLeft(rc10Area.GetLeft() + nSegmentW);
	_rcA10.SetTop(rc10Area.GetTop());
	_rcA10.SetRight(rc10Area.GetRight() - nSegmentW);
	_rcA10.SetBottom(rc10Area.GetTop() + nSegmentH - _padding);
//	pDC->DrawRectangle(_rcA10);

	_rcB10.SetLeft(_rcA10.GetRight() + _padding);
	_rcB10.SetTop(_rcA10.GetBottom() + _padding);
	_rcB10.SetRight(rc10Area.GetRight());
	_rcB10.SetBottom(rc10Area.GetTop() + nSegmentCenter - _padding);
//	pDC->DrawRectangle(_rcB10);

	_rcC10.SetLeft(_rcB10.GetLeft());
	_rcC10.SetTop(_rcB10.GetBottom() + _padding);
	_rcC10.SetRight(rc10Area.GetRight());
	_rcC10.SetBottom(rc10Area.GetBottom() - nSegmentH - _padding);
//	pDC->DrawRectangle(_rcC10);

	_rcD10.SetLeft(_rcA10.GetLeft());
	_rcD10.SetTop(_rcC10.GetBottom() + _padding);
	_rcD10.SetRight(_rcA10.GetRight());
	_rcD10.SetBottom(rc10Area.GetBottom() - _padding);
//	pDC->DrawRectangle(_rcD10);

	_rcE10.SetLeft(rc10Area.GetLeft());
	_rcE10.SetTop(_rcC10.GetTop());
	_rcE10.SetRight(_rcA10.GetLeft() - _padding);
	_rcE10.SetBottom(_rcC10.GetBottom());
//	pDC->DrawRectangle(_rcE10);

	_rcF10.SetLeft(rc10Area.GetLeft());
	_rcF10.SetTop(_rcB10.GetTop());
	_rcF10.SetRight(_rcA10.GetLeft() - _padding);
	_rcF10.SetBottom(_rcB10.GetBottom());
//	pDC->DrawRectangle(_rcF10);

	_rcG10.SetLeft(_rcA10.GetLeft() + _padding);
	_rcG10.SetTop(_rcF10.GetBottom() + 1);
	_rcG10.SetRight(_rcA10.GetRight() - _padding);
	_rcG10.SetBottom(_rcG10.GetTop() + nSegmentH - _padding);
//	pDC->DrawRectangle(_rcG10);

	//1단위
	_rcA1.SetLeft(rc1Area.GetLeft() + nSegmentW);
	_rcA1.SetTop(rc1Area.GetTop());
	_rcA1.SetRight(rc1Area.GetRight() - nSegmentW);
	_rcA1.SetBottom(rc1Area.GetTop() + nSegmentH - _padding);
//	pDC->DrawRectangle(_rcA1);

	_rcB1.SetLeft(_rcA1.GetRight() + _padding);
	_rcB1.SetTop(_rcA1.GetBottom() + _padding);
	_rcB1.SetRight(rc1Area.GetRight());
	_rcB1.SetBottom(rc1Area.GetTop() + nSegmentCenter - _padding);
//	pDC->DrawRectangle(_rcB1);

	_rcC1.SetLeft(_rcB1.GetLeft());
	_rcC1.SetTop(_rcB1.GetBottom() + _padding);
	_rcC1.SetRight(rc1Area.GetRight());
	_rcC1.SetBottom(rc1Area.GetBottom() - nSegmentH - _padding);
//	pDC->DrawRectangle(_rcC1);

	_rcD1.SetLeft(_rcA1.GetLeft());
	_rcD1.SetTop(_rcC1.GetBottom() + _padding);
	_rcD1.SetRight(_rcA1.GetRight());
	_rcD1.SetBottom(rc1Area.GetBottom() - _padding);
//	pDC->DrawRectangle(_rcD1);

	_rcE1.SetLeft(rc1Area.GetLeft());
	_rcE1.SetTop(_rcC1.GetTop());
	_rcE1.SetRight(_rcA1.GetLeft() - _padding);
	_rcE1.SetBottom(_rcC1.GetBottom());
//	pDC->DrawRectangle(_rcE1);

	_rcF1.SetLeft(rc1Area.GetLeft());
	_rcF1.SetTop(_rcB1.GetTop());
	_rcF1.SetRight(_rcA1.GetLeft() - _padding);
	_rcF1.SetBottom(_rcB1.GetBottom());
//	pDC->DrawRectangle(_rcF1);

	_rcG1.SetLeft(_rcA1.GetLeft() + _padding);
	_rcG1.SetTop(_rcF1.GetBottom() + 1);
	_rcG1.SetRight(_rcA1.GetRight() - _padding);
	_rcG1.SetBottom(_rcG1.GetTop() + nSegmentH - _padding);
}

void CSevenSegment::DrawSevenSegment(wxDC* pDC, unsigned int _value)
{
	if(!_bCreatedSegment)
		return;

	int iVal100 = 0;
	int iVal10  = 0;
	int iVal1   = 0;

	if(_value == 100)
		iVal100 = 1;
	else
	{
		iVal10 = _value / 10;
		iVal1  = _value % 10;
	}

	//백단위
	pDC->SetPen(GetColor(m_iSegmentNums[iVal100], 6));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal100], 6));
	pDC->DrawRectangle(_rcA100);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal100], 5));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal100], 5));
	pDC->DrawRectangle(_rcB100);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal100], 4));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal100], 4));
	pDC->DrawRectangle(_rcC100);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal100], 3));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal100], 3));
	pDC->DrawRectangle(_rcD100);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal100], 2));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal100], 2));
	pDC->DrawRectangle(_rcE100);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal100], 1));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal100], 1));
	pDC->DrawRectangle(_rcF100);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal100], 0));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal100], 0));
	pDC->DrawRectangle(_rcG100);

	//십단위
	pDC->SetPen(GetColor(m_iSegmentNums[iVal10], 6));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal10], 6));
	pDC->DrawRectangle(_rcA10);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal10], 5));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal10], 5));
	pDC->DrawRectangle(_rcB10);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal10], 4));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal10], 4));
	pDC->DrawRectangle(_rcC10);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal10], 3));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal10], 3));
	pDC->DrawRectangle(_rcD10);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal10], 2));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal10], 2));
	pDC->DrawRectangle(_rcE10);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal10], 1));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal10], 1));
	pDC->DrawRectangle(_rcF10);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal10], 0));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal10], 0));
	pDC->DrawRectangle(_rcG10);

	//일단위
	pDC->SetPen(GetColor(m_iSegmentNums[iVal1], 6));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal1], 6));
	pDC->DrawRectangle(_rcA1);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal1], 5));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal1], 5));
	pDC->DrawRectangle(_rcB1);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal1], 4));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal1], 4));
	pDC->DrawRectangle(_rcC1);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal1], 3));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal1], 3));
	pDC->DrawRectangle(_rcD1);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal1], 2));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal1], 2));
	pDC->DrawRectangle(_rcE1);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal1], 1));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal1], 1));
	pDC->DrawRectangle(_rcF1);

	pDC->SetPen(GetColor(m_iSegmentNums[iVal1], 0));
	pDC->SetBrush(GetColor(m_iSegmentNums[iVal1], 0));
	pDC->DrawRectangle(_rcG1);
}

wxColour CSevenSegment::GetColor(int iVal, int ishift)
{
	int r = 255 * ((iVal >> ishift) & 1);
	int g = 0;
	int b = 0;

	return wxColour(r, g, b);
}
