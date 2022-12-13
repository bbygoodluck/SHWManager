#ifndef SYSTEMIMPL_H_INCLUDED
#define SYSTEMIMPL_H_INCLUDED

class CCPUInfo;
class CMemoryInfo;
class CNetworkInfo;
class CProcessInfo;

class CSystemResourceInfo
{
private:
	CSystemResourceInfo();

public:
	~CSystemResourceInfo();
	static CSystemResourceInfo* Get();

	void Init();

	CCPUInfo* GetCPUInfo();
	CMemoryInfo* GetMemoryInfo();
	CNetworkInfo* GetNetworkInfo();
	CProcessInfo* GetProcessInfo();

private:

private:
	static std::unique_ptr<CSystemResourceInfo> m_pInstance;

	CCPUInfo* m_cpuInfoImpl         = nullptr;
	CMemoryInfo* m_memoryInfoImpl   = nullptr;
	CNetworkInfo* m_networkInfoImpl = nullptr;
	CProcessInfo* m_processInfoImpl = nullptr;
};

#endif // SYSTEMIMPL_H_INCLUDED
