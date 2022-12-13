#include "../ginc.h"
#include "SystemResourceInfo.h"
#include "system.h"

std::unique_ptr<CSystemResourceInfo> CSystemResourceInfo::m_pInstance(nullptr);

CSystemResourceInfo::CSystemResourceInfo()
{

}

CSystemResourceInfo::~CSystemResourceInfo()
{
	if(m_cpuInfoImpl)
		wxDELETE(m_cpuInfoImpl);

	if(m_memoryInfoImpl)
		wxDELETE(m_memoryInfoImpl);

	if(m_networkInfoImpl)
		wxDELETE(m_networkInfoImpl);

	if(m_processInfoImpl)
		wxDELETE(m_processInfoImpl);
}

CSystemResourceInfo* CSystemResourceInfo::Get()
{
	if (m_pInstance == nullptr)
		m_pInstance.reset(new CSystemResourceInfo());

	return m_pInstance.get();
}

void CSystemResourceInfo::Init()
{
	m_cpuInfoImpl = new CCPUInfo();
	m_memoryInfoImpl = new CMemoryInfo();
	m_networkInfoImpl = new CNetworkInfo();
	m_processInfoImpl = new CProcessInfo();
}

CCPUInfo* CSystemResourceInfo::GetCPUInfo()
{
	if(m_cpuInfoImpl == nullptr)
		return nullptr;


	return m_cpuInfoImpl;
}

CMemoryInfo* CSystemResourceInfo::GetMemoryInfo()
{
	if(m_memoryInfoImpl == nullptr)
		return nullptr;

	return m_memoryInfoImpl;
}

CNetworkInfo* CSystemResourceInfo::GetNetworkInfo()
{
	if(m_networkInfoImpl == nullptr)
		return nullptr;

	return m_networkInfoImpl;
}

CProcessInfo* CSystemResourceInfo::GetProcessInfo()
{
	if(m_processInfoImpl == nullptr)
		return nullptr;

	return m_processInfoImpl;
}
