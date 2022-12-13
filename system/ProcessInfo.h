#ifndef PROCESSINFO_H_INCLUDED
#define PROCESSINFO_H_INCLUDED

#include "ResourceCommon.h"
#ifdef __WXMSW__
#include <wtsapi32.h>
#include <aclapi.h>
#include <psapi.h>
#else
#endif // __WXMSW__

class CGlobalHook;
class CProcessInfo : public CResourceCommon
{
public:
	CProcessInfo();
	~CProcessInfo();

public:
	virtual void Init() override;
	virtual void Clear() override;

	std::unordered_map<unsigned long, PROCESS_INFO *>::iterator BeginIter() { return m_mapProcessList.begin(); }
	std::unordered_map<unsigned long, PROCESS_INFO *>::iterator EndIter()  { return m_mapProcessList.end(); }

	PROCESS_INFO* UpdateProcessInfo(unsigned long _ulProcessId);

private:
	virtual void Update() override;
	//Process CPU
	void InitProcessCPUTime(PROCESS_INFO* _pPsInfo);
	void UpdateProcessCPUTime(PROCESS_INFO* _pPsInfo);
	//Process Memory
	void UpdateProcessMemory(PROCESS_INFO* _pPsInfo);

#ifdef __WXMSW__
	void AddProcessInfo(const WTS_PROCESS_INFO& wtsProcessInfo);
	void AddProcessInfo(unsigned long ulProcessID, const wxString& _strProcessName);
	bool SetPrivilige(LPCTSTR lpszPrivilege, bool bEnablePrivilege = true);
	int SetUserNameAndDomainNameFromSid(PROCESS_INFO* pInfo);
	void GetUserNameFromToken(PROCESS_INFO* pInfo);

#else
#endif // __WXMSW__
private:
	std::unordered_map<unsigned long, PROCESS_INFO *> m_mapProcessList;
	CGlobalHook* m_pGlobalHook = nullptr;
	wxString  m_strHookLibPath = wxT("");
private:
	virtual void OnTimer(wxTimerEvent& event) override;
	wxDECLARE_EVENT_TABLE();
};

#endif // PROCESSINFO_H_INCLUDED
