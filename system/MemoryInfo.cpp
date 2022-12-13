#include "../ginc.h"
#include "MemoryInfo.h"

wxBEGIN_EVENT_TABLE(CMemoryInfo, CResourceCommon)
wxEND_EVENT_TABLE()

CMemoryInfo::CMemoryInfo()
{
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(CMemoryInfo::OnTimer));
	m_timer.SetOwner(this, wxTIMER_ID++);

}

CMemoryInfo::~CMemoryInfo()
{
	Stop();
	Clear();
}

void CMemoryInfo::Init()
{
	Update();
}

void CMemoryInfo::OnTimer(wxTimerEvent& event)
{
	Update();
	//이벤트 보내기..
	SendEvent();
	event.Skip();
}

void CMemoryInfo::Update()
{
	wxZeroMemory(_memroyStatus);
	_memroyStatus.dwLength = sizeof(_memroyStatus);
	::GlobalMemoryStatusEx(&_memroyStatus);

	//물리메모리
	_ullTotalPhys  = _memroyStatus.ullTotalPhys / MEGABYTE;
	_ullAvailPhys  = _memroyStatus.ullAvailPhys / MEGABYTE;
	_ullUsedPhys   = (_memroyStatus.ullTotalPhys - _memroyStatus.ullAvailPhys) / MEGABYTE;

	//가상메모리
	_ullTotalVirtual = _memroyStatus.ullTotalVirtual / MEGABYTE;
	_ullAvailVirtual = _memroyStatus.ullAvailVirtual / MEGABYTE;
	_ullUsedVirtual  = (_memroyStatus.ullTotalVirtual - _memroyStatus.ullAvailVirtual) / MEGABYTE;

	//페이지파일
	_ullTotalPageFile = _memroyStatus.ullTotalPageFile / MEGABYTE;
	_ullAvailPageFile = _memroyStatus.ullAvailPageFile / MEGABYTE;
	_ullUsedPageFile  = (_memroyStatus.ullTotalPageFile - _memroyStatus.ullAvailPageFile) / MEGABYTE;
}


