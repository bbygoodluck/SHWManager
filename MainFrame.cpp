#include "ginc.h"
#include "MainFrame.h"
#include "SHWStatusBar.h"
#include <wx/bitmap.h>

#ifdef __WXMSW__
	static const GUID usb_hid = { 0xA5DCBF10L, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED} };
#endif

wxBEGIN_EVENT_TABLE(CMainFrame, wxFrame)
	EVT_SIZE(CMainFrame::OnSize)
	EVT_CLOSE(CMainFrame::OnClose)
	EVT_MAXIMIZE(CMainFrame::OnMaximized)
	EVT_ICONIZE(CMainFrame::OnIconized)
	EVT_LEFT_DOWN(CMainFrame::OnMouseLBottonDown)
wxEND_EVENT_TABLE()

CMainFrame::CMainFrame(const wxString& strTitle)
	: wxFrame(NULL, wxID_ANY, strTitle, wxPoint(theJsonConfig->GetXPos(), theJsonConfig->GetYPos()),
		                                wxSize(theJsonConfig->GetWidth(), theJsonConfig->GetHeight()))
{
    SetIcon(wxIcon("wxwin"));
    wxInitAllImageHandlers();

	wxMenuBar* pMenuBar = wxXmlResource::Get()->LoadMenuBar(this, wxT("SHCMenu"));
	if (pMenuBar == nullptr)
	{
		wxMessageBox(wxT("Failed to load MenuBar resource"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	SetMenuBar(pMenuBar);
	RegisterEventHandler();

	Initialize();
	CreateToolBar();

	theSplitterManager->SetSashGravity(0.4);

	m_mgr.SetManagedWindow(this);
	m_mgr.AddPane(theSplitterManager->GetSplitter(), wxAuiPaneInfo().CenterPane().PaneBorder(false));
	m_mgr.Update();

	if (theJsonConfig->IsMaximized())
		this->Maximize();

	if(theJsonConfig->IsCenterOnScreen())
		CenterOnScreen();

	m_bInitialized = true;

#ifdef __WXMSW__
	//RegisterDeviceNotification()함수를 이용해서 usb 포트를 감시할 경우 주의해야 할 사항
	//    - DEV_BROADCAST_DEVICEINTERFACE 구조체의 dbcc_devicetype값을 DBT_DEVTYP_DEVICEINTERFACE로 설정할 경우
	//      usb포트에 장치가 연결될때마다 호출이 되므로 usb카드삽입시 새로운 드라이브가 생성되었을때만 이벤트를 발생시킬 경우는
	//      DBT_DEVTYP_VOLUME 값을 셋팅해야 정상적으로 usb카드삽입후 실제로 usb 드라이브가 생성된 이후에 이벤트가 발생한다.
	//      DBT_DEVTYP_DEVICEINTERFACE로 셋팅하게 되면 usb포트에 장치가 삽입되자마자 이벤트가 발생하게 됨
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_VOLUME;// DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = usb_hid;

	m_hNotifyDev = ::RegisterDeviceNotification(this->GetHWND(), &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
#endif

	CreateCustomStatusBar();
	_gContextManager->SetMainFrame(this);
	_gContextManager->ActivateView();
}

CMainFrame::~CMainFrame()
{

}

void CMainFrame::Initialize()
{
	int iMonitors = wxDisplay::GetCount();

	int nCurrDisp = wxDisplay::GetFromWindow(this);
	if (nCurrDisp < 0)
		nCurrDisp = 0;

	wxDisplay disp(nCurrDisp);

	if (iMonitors == 1)
	{	//모니터가 하나일경우 기존 모니터 인덱스와 다를 경우는 Default로 셋팅
		int iMonitorPos = theJsonConfig->GetMonitorPos();
		if (nCurrDisp != iMonitorPos)
		{
			wxRect rcDefault = theJsonConfig->GetDefaultRect();

			theJsonConfig->SetChangeGlobalVal(wxT("DispMonitor"), 0);
			theJsonConfig->SetChangeGlobalVal(wxT("XPos"), rcDefault.GetLeft());
			theJsonConfig->SetChangeGlobalVal(wxT("YPos"), rcDefault.GetRight());
			theJsonConfig->SetChangeGlobalVal(wxT("Width"), rcDefault.GetWidth());
			theJsonConfig->SetChangeGlobalVal(wxT("Height"), rcDefault.GetHeight());
		}
	}
}

void CMainFrame::CreateToolBar()
{
	m_pAuiToolbar = std::make_unique<wxAuiToolBar>(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_HORZ_TEXT);

	_gMenuEvent->InitToolbarItems(m_pAuiToolbar.get());
	m_mgr.AddPane(m_pAuiToolbar.get(), wxAuiPaneInfo().Name("Toolbar").Caption("Toolbar").ToolbarPane().Top());
}

void CMainFrame::CreateCustomStatusBar()
{
	m_pStatusBar = std::make_unique<CSHWStatusBar>(this);
	SetStatusBar(m_pStatusBar.get());

	PositionStatusBar();
}

void CMainFrame::OnClose(wxCloseEvent& event)
{
	m_mgr.UnInit();

	PopEventHandler(true);
    Destroy();
}

void CMainFrame::RegisterEventHandler()
{
	_gMenuEvent = new CMenuEvent(this);
	PushEventHandler(_gMenuEvent);

	_gMenuEvent->LoadBookmarkToMenu(false);
}

void CMainFrame::OnSize(wxSizeEvent& event)
{
	if (!this->IsMaximized())
	{	//최대화에서 원래상태로 복원되었을 경우
		if (m_bMaximize)
		{
			int iMonitors = wxDisplay::GetCount();

			int nCurrDisp = wxDisplay::GetFromWindow(this);
			if (nCurrDisp < 0)
				nCurrDisp = 0;

			wxDisplay disp(nCurrDisp);

			theJsonConfig->SetChangeGlobalVal(wxT("Maximized"), 0);
			if (theJsonConfig->IsCenterOnScreen())
				CenterOnScreen();
		}
		else
		{
			wxSize szFrame = event.GetSize();

			theJsonConfig->SetChangeGlobalVal(wxT("Width"), szFrame.GetWidth());
			theJsonConfig->SetChangeGlobalVal(wxT("Height"), szFrame.GetHeight());

			theSplitterManager->SplitWindow(szFrame);
		}

		m_bMaximize = false;
	}
}

void CMainFrame::OnMaximized(wxMaximizeEvent& event)
{
	m_bMaximize = true;
	theJsonConfig->SetChangeGlobalVal(wxT("Maximized"), 1);
}

void CMainFrame::OnIconized(wxIconizeEvent& event)
{

}

#ifdef __WXMSW__
WXLRESULT CMainFrame::MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
	WXLRESULT rc = ::DefWindowProc(this->GetHWND(), nMsg, wParam, lParam);

	switch(nMsg)
	{
	/*	case WM_ACTIVATE:
			if((LOWORD(wParam)  == WA_ACTIVE) || (LOWORD(wParam)  == WA_CLICKACTIVE))
				_gContextManager->ActivateView();

			break;

		case WM_ACTIVATEAPP:
			if((int)wParam)
				_gContextManager->ActivateView();

			break;
	*/
		case WM_DEVICECHANGE:
			OnDeviceChange(wParam, lParam);
			break;

		case WM_NCACTIVATE:
			if((int)wParam)
				_gContextManager->ActivateView();

			break;

		case WM_NCLBUTTONDOWN:
			_gContextManager->ActivateView();
			break;

		default:
			break;
	}

	return rc;
}

void CMainFrame::OnDeviceChange(WXWPARAM nEventType, WXLPARAM dwData)
{
	if ((nEventType != DBT_DEVICEARRIVAL) && (nEventType != DBT_DEVICEREMOVECOMPLETE))
		return;

	PDEV_BROADCAST_HDR pdbh = reinterpret_cast<DEV_BROADCAST_HDR *>(dwData);

	DWORD dbhSize = pdbh->dbch_size;
	if(dbhSize == 0)
		return;

	if (pdbh->dbch_devicetype != DBT_DEVTYP_VOLUME)
		return;

	switch (nEventType)
	{
		case DBT_DEVICEARRIVAL:
		case DBT_DEVICEREMOVECOMPLETE:
		{
			theDriveInfo->Init();
			_gContextManager->UpdateDriveInformation();
		}
			break;

		default:
			break;
	}
}

#endif

void CMainFrame::OnMouseLBottonDown(wxMouseEvent& event)
{
	if(_gContextManager)
		_gContextManager->ActivateView();
}
