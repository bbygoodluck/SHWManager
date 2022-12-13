#include "../ginc.h"
#include <wx/busyinfo.h>

#include "DlgResourceUsage.h"
#include "../system/system.h"
#include "../view/UsageView.h"
#include "../view/GraphView.h"
#include "../view/NetworkUsageGraphView.h"
#include "../view/ProcessListView.h"

DlgResourceUsage::DlgResourceUsage( wxWindow* parent, const wxSize& sz)
	: wxDialog( parent, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_RESOURCE_USAGE_VIEW")), wxDefaultPosition, sz, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	SetIcon(wxIcon("wxwin"));
	CenterOnScreen();

	wxBusyInfo info
    (
        wxBusyInfoFlags()
            .Parent(nullptr)
            .Icon(wxArtProvider::GetIcon(wxART_INFORMATION,
                                         wxART_OTHER, wxSize(64, 64)))
            .Title("<b>Initialize System resource...</b>")
            .Text("please wait...")
            .Foreground(*wxWHITE)
            .Background(wxColour(100, 100, 100))
            .Transparency(5 * (wxALPHA_OPAQUE / 5))
    );

	//CPU사용량 초기화
	theSystem->GetCPUInfo()->Init();
	//메모리사용량 초기화
	theSystem->GetMemoryInfo()->Init();
	//네트워크정보 초기화
	theSystem->GetNetworkInfo()->Init();
	//프로세스 정보 초기화
	theSystem->GetProcessInfo()->Init();

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );

	m_bmpProcessor = new wxStaticBitmap( this, wxID_ANY, wxArtProvider::GetBitmap( wxART_HARDDISK, wxART_FRAME_ICON ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_bmpProcessor, 0, wxALL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Processor : "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont(*_gViewFont);
	bSizer8->Add( m_staticText2, 0, wxBOTTOM|wxTOP, 5 );

	m_staticProcessor = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticProcessor->Wrap( -1 );
	m_staticProcessor->SetFont(*_gViewFont);

	bSizer8->Add( m_staticProcessor, 1, wxBOTTOM|wxEXPAND|wxTOP, 5 );

	bSizer7->Add( bSizer8, 0, wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline1, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_pUsageCPUView = new CUsageView(this, wxID_ANY, wxSize(-1, 25), USAGE_TYPE::USAGE_CPU);
	bSizer9->Add( m_pUsageCPUView, 1, wxEXPAND | wxALL, 5 );

	m_pUsageMemoryView = new CUsageView(this, wxID_ANY, wxSize(-1, 25), USAGE_TYPE::USAGE_MEMORY);
	bSizer9->Add( m_pUsageMemoryView, 1, wxEXPAND | wxALL, 5 );

	m_pUsagePageFileView = new CUsageView(this, wxID_ANY, wxSize(-1, 25), USAGE_TYPE::USAGE_PAGEFILE);
	bSizer9->Add( m_pUsagePageFileView, 1, wxEXPAND | wxALL, 5 );

	bSizer7->Add( bSizer9, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	CreateCPUCoreList(bSizer14);

	bSizer7->Add( bSizer14, 0, wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Process"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	m_staticText7->SetFont(*_gViewFont);
	bSizer7->Add( m_staticText7, 0, wxEXPAND|wxLEFT, 5 );

	m_pProcessListView = new CProcessListView(this);//new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer7->Add( m_pProcessListView, 1, wxALL|wxEXPAND, 5 );

	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );

	m_staticNetInfo = new wxStaticText( this, wxID_ANY, wxT("Network"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticNetInfo->Wrap( -1 );
	m_staticNetInfo->SetFont(*_gViewFont);
	bSizer7->Add( m_staticNetInfo, 0, wxEXPAND|wxLEFT, 5 );

	m_pNetworkUsageView = new CNetworkUsageGraphView(this, wxID_ANY, wxSize(-1, 70));
	bSizer7->Add( m_pNetworkUsageView, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	m_btnClose = new wxButton( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_OPTION_BTN_CLOSE")), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnClose->SetBitmap( wxArtProvider::GetBitmap( wxART_QUIT, wxART_BUTTON ) );

	bSizer13->Add( m_btnClose, 0, wxALIGN_RIGHT|wxBOTTOM|wxRIGHT, 5 );

	bSizer7->Add( bSizer13, 0, wxEXPAND, 5 );

	this->SetSizer( bSizer7 );
	this->Layout();

	this->Centre( wxBOTH );

	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgResourceUsage::OnInitDialog ) );
	m_btnClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgResourceUsage::OnClose ), NULL, this );
}

DlgResourceUsage::~DlgResourceUsage()
{
	theSystem->GetCPUInfo()->Stop();
	theSystem->GetCPUInfo()->Clear();

	theSystem->GetMemoryInfo()->Stop();
	theSystem->GetMemoryInfo()->Clear();

	theSystem->GetNetworkInfo()->Stop();
	theSystem->GetNetworkInfo()->Clear();

	theSystem->GetProcessInfo()->Stop();
	theSystem->GetProcessInfo()->Clear();

	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgResourceUsage::OnInitDialog ) );
	m_btnClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgResourceUsage::OnClose ), NULL, this );
}

void DlgResourceUsage::OnInitDialog( wxInitDialogEvent& event )
{
	//CPU 프로세서 정보
	wxString strProcessorInfo = theSystem->GetCPUInfo()->GetProcessorInfo();
	wxString strCore = wxString::Format(wxT(" - Core Count : %d"), theSystem->GetCPUInfo()->GetCoreCount());
	strProcessorInfo += strCore;

	m_staticProcessor->SetLabelText(strProcessorInfo);

	//네트워크 정보
	wxString strNetDescription = theSystem->GetNetworkInfo()->GetDescription();
	wxString strIP = theSystem->GetNetworkInfo()->GetIP();

	wxString strNetInfo = wxT("Network - ");
	strNetInfo += strNetDescription;
	strNetInfo += wxT(" / IP : ");
	strNetInfo += strIP;

	m_staticNetInfo->SetLabelText(strNetInfo);

	//CPU 사용량 이벤트 리스너 등록
	theSystem->GetCPUInfo()->AddEventListener(m_pUsageCPUView);
	//메모리 사용량 이벤트 리스너 등록
	theSystem->GetMemoryInfo()->AddEventListener(m_pUsageMemoryView);
	//페이지 파일 이벤트 리스너 등록
	theSystem->GetMemoryInfo()->AddEventListener(m_pUsagePageFileView);
	//네트워크 이벤트 리스너 등록
	theSystem->GetNetworkInfo()->AddEventListener(m_pNetworkUsageView);
	//프로세스 이벤트 리스너 등록
//	theSystem->GetProcessInfo()->AddEventListener(m_pProcessListView);
	//CPU 사용량 모니터링 시작
	theSystem->GetCPUInfo()->Start();
	//Memory 사용량 모니터링 시작
	theSystem->GetMemoryInfo()->Start();
	//네트워크 모니터링 시작
	theSystem->GetNetworkInfo()->Start();
	//프로세스 모니터링 시작
	//프로세스의 모니터링은 ListCtrl 에서 하는게 더 좋을듯 함..
	//theSystem->GetProcessInfo()()->Start();
	m_pUsageCPUView->ShadowStart();
	m_pUsageMemoryView->ShadowStart();
	m_pUsagePageFileView->ShadowStart();
	m_pNetworkUsageView->ShadowStart();
	m_pProcessListView->ShadowStart();
}

void DlgResourceUsage::CreateCPUCoreList(wxSizer* pSizer)
{
	unsigned int nCoreCount = theSystem->GetCPUInfo()->GetCoreCount();
	unsigned int nMaxDispCore = 8;

	unsigned int nDisp = nCoreCount / nMaxDispCore;
	unsigned int nCPUCoreRow = nDisp == 0 ? 1 : nDisp;

	unsigned int corenum = 0;
	//한번에 표시할 수 있는 Core수는 8개
	if(nCoreCount > nMaxDispCore)
		nCoreCount = nMaxDispCore;

	for(unsigned int index = 0; index < nCPUCoreRow; index++)
	{
		wxBoxSizer* coreSizer = new wxBoxSizer( wxHORIZONTAL );
		for(unsigned int coreindex = 0; coreindex < nCoreCount; coreindex++ )
		{
			CGraphView* pGraphView = new CGraphView(this, wxID_ANY, wxSize(-1, 50));
			pGraphView->SetCoreIndex(corenum++);
			coreSizer->Add( pGraphView, 1, wxEXPAND | wxALL, 5 );

			theSystem->GetCPUInfo()->AddEventListener(pGraphView);
		}

		pSizer->Add( coreSizer, 1, wxEXPAND, 5 );
	}
}

void DlgResourceUsage::OnClose( wxCommandEvent& event )
{
	EndDialog(wxID_CLOSE);
}
