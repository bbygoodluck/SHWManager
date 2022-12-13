#ifndef MEMORYINFO_H_INCLUDED
#define MEMORYINFO_H_INCLUDED

#include "ResourceCommon.h"

class CMemoryInfo : public CResourceCommon
{
public:
	CMemoryInfo();
	~CMemoryInfo();

public:
	virtual void Init() override;

	unsigned long GetTotalMemory() { return _ullTotalPhys; }
	unsigned long GetAvaliableMemory() { return _ullAvailPhys; }
	unsigned long GetUsedMemory() { return _ullUsedPhys; }

	unsigned long GetTotalVirtualMemory() { return _ullTotalVirtual; }
	unsigned long GetAvaliableVirtualMemory() { return _ullAvailVirtual; }
	unsigned long GetUsedVirtualMemory() { return _ullUsedVirtual; }

	unsigned long GetTotalPageFile() { return _ullTotalPageFile; }
	unsigned long GetAvaliablePageFile() { return _ullAvailPageFile; }
	unsigned long GetUsedPageFile() { return _ullUsedPageFile; }

private:
	virtual void Update() override;

private:
#ifdef __WXMSW__
	MEMORYSTATUSEX _memroyStatus = {0x00, };
#else
#endif // __WXMSW__
	//�����޸�
	unsigned long _ullTotalPhys  = 0;
	unsigned long _ullUsedPhys   = 0;
	unsigned long _ullAvailPhys  = 0;
	//����޸�
	unsigned long _ullTotalVirtual = 0;
	unsigned long _ullUsedVirtual  = 0;
	unsigned long _ullAvailVirtual = 0;
	//����������
	unsigned long _ullTotalPageFile = 0;
	unsigned long _ullUsedPageFile  = 0;
	unsigned long _ullAvailPageFile = 0;

private:
	virtual void OnTimer(wxTimerEvent& event) override;
	wxDECLARE_EVENT_TABLE();
};
#endif // MEMORYINFO_H_INCLUDED
