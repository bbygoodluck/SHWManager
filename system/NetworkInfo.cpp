#include "../ginc.h"
#include "NetworkInfo.h"

wxBEGIN_EVENT_TABLE(CNetworkInfo, CResourceCommon)
wxEND_EVENT_TABLE()

CNetworkInfo::CNetworkInfo()
{
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(CNetworkInfo::OnTimer));
	m_timer.SetOwner(this, wxTIMER_ID++);
}

CNetworkInfo::~CNetworkInfo()
{
	Stop();
	Clear();
}

void CNetworkInfo::Clear()
{
	CResourceCommon::Clear();
}

void CNetworkInfo::Init()
{
	Update();
}

void CNetworkInfo::OnTimer(wxTimerEvent& event)
{
	Update();
	SendEvent();
}


void CNetworkInfo::Update()
{
#ifdef __WXMSW__
	IP_ADAPTER_INFO  *pAdapterInfo;
	ULONG            ulOutBufLen;
	DWORD            dwRetVal;

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
	{
		free (pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc ( ulOutBufLen );
	}

	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) != ERROR_SUCCESS)
		return;


	if(!bSetNetInfo_)
	{
		bSetNetInfo_ = true;
		strNetAdapterName_ = pAdapterInfo->AdapterName;
		strNetDescription_ = pAdapterInfo->Description;
		strNetIP_ = pAdapterInfo->IpAddressList.IpAddress.String;
	}

	MIB_IFROW ifrow;
	wxZeroMemory(ifrow);

	ifrow.dwIndex = pAdapterInfo->Index;
	if(GetIfEntry(&ifrow) != NO_ERROR)
		return;

	dwcurrent_sent_ = ifrow.dwOutOctets - dwlast_sent_;
	dwcurrent_recv_ = ifrow.dwInOctets - dwlast_recv_;

	dwlast_sent_ = ifrow.dwOutOctets;
	dwlast_recv_ = ifrow.dwInOctets;

	float diftime = (GetTickCount() - dw_refreshtime_);

	dblrecvspeed_ = dwcurrent_recv_ / diftime;
	dblsentspeed_ = dwcurrent_sent_ / diftime;

	dw_refreshtime_ = GetTickCount();

	if (pAdapterInfo)
        free(pAdapterInfo);
#else

#endif

}
