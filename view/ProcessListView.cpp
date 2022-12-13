#include "../ginc.h"
#include "../system/system.h"
#include "ProcessListView.h"

wxBEGIN_EVENT_TABLE(CProcessListView, wxWindow)
	EVT_MY_CUSTOM_COMMAND(wxEVT_RESOURCE_MONITORING, wxID_ANY, CProcessListView::UpdateListener)
wxEND_EVENT_TABLE()

CProcessListView::CProcessListView(wxWindow* _parent)
	: wxListCtrl(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxBORDER_NONE)
{
#ifdef __WXMSW__
	ListView_SetImageList(GetHWND(), _gImageList, LVSIL_SMALL);
#else
	this->SetImageList(_gImageList);
#endif

	this->Connect(wxEVT_TIMER, wxTimerEventHandler(CProcessListView::OnTimer));
	m_timer.SetOwner(this, wxTIMER_ID++);

	this->SetForegroundColour(wxColour(60, 60, 60));
	this->SetFont(*_gViewFont);

	this->m_bAscending = true;
	this->m_nSortingColumn = 0;

	InitColumn();
	InitList();
}

CProcessListView::~CProcessListView()
{

}

void CProcessListView::InitColumn()
{
	sListColumnItem lstColumn[] = {
		{wxT("ProcessName")     , 250, wxLIST_FORMAT_LEFT},
		{wxT("PID")             , 70 , wxLIST_FORMAT_RIGHT},
		{wxT("UserName")        , 230, wxLIST_FORMAT_RIGHT},
		{wxT("CPU")             , 70 , wxLIST_FORMAT_RIGHT},
		{wxT("Private KBytes")  , 140, wxLIST_FORMAT_RIGHT},
		{wxT("WorkingSet")      , 140, wxLIST_FORMAT_RIGHT},
		{wxT("File Description"), 270, wxLIST_FORMAT_LEFT},
		{wxT("Company Name")    , 180, wxLIST_FORMAT_LEFT},
		{wxT("FullPath")        , 440, wxLIST_FORMAT_LEFT},
	};

	for (unsigned int i = 0; i < WXSIZEOF(lstColumn); i++)
	{
		wxListItem lstItemInfo;
		lstItemInfo.SetText(lstColumn[i]._strColumnName);
		lstItemInfo.SetAlign(lstColumn[i]._align);
		lstItemInfo.SetWidth(lstColumn[i]._width);

		this->InsertColumn(i, lstItemInfo);
	}
}

void CProcessListView::InitList()
{
	m_ulCurrentProcessId = GetCurrentProcessId();
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iterS = theSystem->GetProcessInfo()->BeginIter();
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iterE = theSystem->GetProcessInfo()->EndIter();

	wxString strSize(wxT(""));
	int iIndex = 0;
	while(iterS != iterE)
	{
		PROCESS_INFO* pProcessInfo = iterS->second;

		InsertItem(iIndex, pProcessInfo->_strProcessName);
		SetItem(iIndex, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		SetItem(iIndex, 2, pProcessInfo->_strDomainAndUser);
		SetItem(iIndex, 3, wxT("0.00%"));
		SetItem(iIndex, 4, wxT("0 KB"));
		SetItem(iIndex, 5, wxT("0 KB"));
		SetItem(iIndex, 6, pProcessInfo->_strFileDescription);
		SetItem(iIndex, 7, pProcessInfo->_strCompanyName);
		SetItem(iIndex, 8, pProcessInfo->_strProcessFullPath);

		this->SetItemImage(iIndex, pProcessInfo->iIconIndex);
		this->SetItemData(iIndex, iIndex);

		if(m_ulCurrentProcessId == pProcessInfo->_ulProcessID)
			this->SetItemBackgroundColour(iIndex, wxColour(255, 255, 0));

		iIndex++;
		++iterS;
	}

	this->SortItems(CProcessListView::ListCompareFunction, (LPARAM)this);
}

void CProcessListView::OnTimer(wxTimerEvent& event)
{
	Update();
}

void CProcessListView::UpdateListener(wxCommandEvent& event)
{
	Update();
}

void CProcessListView::Update()
{
	wxString strCPUUsage(wxT(""));
	wxString strPrivateSize(wxT(""));
	wxString strWorkingSet(wxT(""));

	float fCPUUsage = 0.0f;
	int nProcessCounts = this->GetItemCount();
	for (int index = 0; index < nProcessCounts; index++)
	{
		wxString strProcessID = this->GetItemText(index, 1);
		unsigned long ulProcessID = 0;

		strProcessID.ToCULong(&ulProcessID);

		PROCESS_INFO* pProcessInfo = theSystem->GetProcessInfo()->UpdateProcessInfo(ulProcessID);
		if(pProcessInfo == nullptr)
			continue;

		fCPUUsage = 0.0f;
		fCPUUsage = pProcessInfo->fCPUUsage;
		strCPUUsage = wxString::Format(wxT("%.2f%"), fCPUUsage);

		strPrivateSize = theUtility->SetComma(wxString::Format(wxT("%.0f"), (float)pProcessInfo->_PrivateSize / (1024.0f)));
		strWorkingSet = theUtility->SetComma(wxString::Format(wxT("%.0f"), (float)pProcessInfo->_WorkingSetSize / (1024.0f)));

		this->SetItem(index, 3, strCPUUsage);
		this->SetItem(index, 4, strPrivateSize + wxT(" KB"));
		this->SetItem(index, 5, strWorkingSet + wxT(" KB"));

		if(m_ulCurrentProcessId != pProcessInfo->_ulProcessID)
			UpdateBackgroundColour(index, fCPUUsage);
	}
}

void CProcessListView::UpdateBackgroundColour(int _index, float _fCPUUsage)
{
	unsigned int nCPUUsage = (unsigned int)_fCPUUsage;
	unsigned int nColVal = (255 * nCPUUsage) / 100;

	this->SetItemBackgroundColour(_index, wxColour(255, 255 - nColVal, 255 - nColVal));
//	this->SetItemTextColour(_index, wxColour(nColVal, nColVal, nColVal));
}

void CProcessListView::ShadowStart()
{
	m_timer.Start(1000);
}

void CProcessListView::ShadowStop()
{
	m_timer.Stop();
}

int wxCALLBACK CProcessListView::ListCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
	CProcessListView* pListCtrl = (CProcessListView *)sortData;
	if(pListCtrl == nullptr)
	{
		if (item1 < item2)
			return 1;

		if (item1 > item2)
			return -1;

		return 0;
	}

	int iSortingColumn = pListCtrl->GetSortingColumn();
	bool bAscending = pListCtrl->IsAscending();

	wxString str1 = pListCtrl->GetItemText(item1, iSortingColumn);
	wxString str2 = pListCtrl->GetItemText(item2, iSortingColumn);

	int iRet = 0;
	if(str1.CmpNoCase(str2) > 0)
	{
		if(bAscending)
			iRet = 1;
		else
			iRet = -1;
	}

	if(str1.CmpNoCase(str2) < 0)
	{
		if(bAscending)
			iRet = -1;
		else
			iRet = 1;
	}

	return iRet;
}
