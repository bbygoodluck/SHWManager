#ifndef SEVENSEGMENT_H_INCLUDED
#define SEVENSEGMENT_H_INCLUDED

class CSevenSegment
{
public:
	CSevenSegment();
	~CSevenSegment();

public:
	void CreateSevenSegment(wxDC* pDC, const wxRect& viewRect);
	bool IsCreatedSevenSegment() { return _bCreatedSegment; }
	void ResetCreatedSevenSegment() { _bCreatedSegment = false; }
	void DrawSevenSegment(wxDC* pDC, unsigned int _value);

private:
	wxColour GetColor(int iVal, int ishift);

private:
	//Segment
	wxRect _rcA100;
	wxRect _rcB100;
	wxRect _rcC100;
	wxRect _rcD100;
	wxRect _rcE100;
	wxRect _rcF100;
	wxRect _rcG100;

	wxRect _rcA10;
	wxRect _rcB10;
	wxRect _rcC10;
	wxRect _rcD10;
	wxRect _rcE10;
	wxRect _rcF10;
	wxRect _rcG10;

	wxRect _rcA1;
	wxRect _rcB1;
	wxRect _rcC1;
	wxRect _rcD1;
	wxRect _rcE1;
	wxRect _rcF1;
	wxRect _rcG1;

	const int _padding = 2;

	int m_iSegmentNums[10] = {0x7E, //0
							  0x30, //1
							  0x6D, //2
							  0x79, //3
							  0x33, //4
							  0x5B, //5
							  0x5F, //6
							  0x70, //7
							  0x7F, //8
							  0x7B  //9
	};

	bool _bCreatedSegment = false;
};

#endif // SEVENSEGMENT_H_INCLUDED
