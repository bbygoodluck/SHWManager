#ifndef CPUINFO_H_INCLUDED
#define CPUINFO_H_INCLUDED

#include "ResourceCommon.h"
class CCPUInfo : public CResourceCommon
{
public:
	CCPUInfo();
	~CCPUInfo();

public:
	virtual void Init() override;
	virtual void Clear() override;

	inline wxString GetProcessorInfo() { return _strProcessorInfo; }
	unsigned long GetCPUUsage() { return m_ulCPUTotalUage; }
	unsigned long GetCPUCoreUsage(unsigned int index);
	unsigned int GetCoreCount() { return (unsigned int)m_CPUCoreCount; }

private:
	virtual void Update() override;

private:
	wxString _strProcessorInfo;
	unsigned long  m_ulCPUTotalUage = 0;

	size_t    m_CPUCoreCount = 0;

#ifdef __WXMSW__
	unsigned long* m_pArrayCore = nullptr;
	HQUERY m_hCpuQuery;

	HCOUNTER  m_hCounterCPUTotal;
	HCOUNTER* m_phCounterCPUCore = nullptr;

	bool m_bPhdOK = false;
#else
	FILETIME m_ftPrevSysIdle;
	FILETIME m_ftPrevSysKernel;
	FILETIME m_ftPrevSysUser;
#endif // __WXMSW__

private:
	virtual void OnTimer(wxTimerEvent& event) override;
	wxDECLARE_EVENT_TABLE();
};

#endif // CPUINFO_H_INCLUDED
