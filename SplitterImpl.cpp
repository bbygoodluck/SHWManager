#include "ginc.h"
#include "SplitterImpl.h"

CSplitterImpl* CSplitterImpl::m_pSplitterInstance = nullptr;

CSplitterImpl* CSplitterImpl::Get()
{
	if (m_pSplitterInstance == nullptr)
    {
        m_pSplitterInstance = new CSplitterImpl();
        m_pSplitterInstance->Init();
    }

	return m_pSplitterInstance;
}

CSplitterImpl::~CSplitterImpl()
{
}

void CSplitterImpl::Init()
{
    wxFrame* pMainFrame = (wxFrame *)((wxTopLevelWindow*)wxTheApp->GetTopWindow());
    m_pSpliiterManager = std::make_unique<CSplitterManager>(pMainFrame);

    if(!m_pTabLeft)
		m_pTabLeft = std::make_unique<CTabManager>(m_pSpliiterManager.get(), theJsonConfig->GetTabStyle());

//    CreateAndDeleteSplitTab();
}

void CSplitterImpl::SetSashGravity(double gravity)
{
    if(m_pSpliiterManager == nullptr)
        Init();

    m_pSpliiterManager->SetSashGravity(gravity);
}

void CSplitterImpl::SplitWindow(const wxSize& _size)
{
	CreateAndDeleteSplitTab();

    wxSize szClient(_size);

	int iCenterWidth = szClient.GetWidth() / 2;
	int iCenterHeight = szClient.GetHeight() / 2;

	int iSplitStyle = theJsonConfig->GetSplitStyle();
    if(m_pSpliiterManager)
    {
        if (iSplitStyle == WINDOW_SPLIT_NONE)
        {
			if (m_pSpliiterManager->IsSplit())
				m_pSpliiterManager->Unsplit();

			_gActivatedTab = m_pTabLeft.get();
			m_pSpliiterManager->Initialize(m_pTabLeft.get());
        }
        else
        {
        	SetWindowArrange(iSplitStyle, szClient);
			_gActivatedTab = m_pTabLeft.get();
        }
    }

    m_Initialized = true;
}

void CSplitterImpl::SetWindowArrange(int nViewStyle, const wxSize& size)
{
	int nCenterWidth = size.GetWidth() / 2;
	int nCenterHeight = size.GetHeight() / 2;

	if (m_pSpliiterManager->IsSplit())
		m_pSpliiterManager->Unsplit();

	if (m_pTabRight->GetPageCount() == 0)
		m_pTabRight->AddTab(false);

	m_pTabLeft->Show(true);
	m_pTabRight->Show(true);

	if (nViewStyle == WINDOW_SPLIT_HORZ)
	{
		m_pSpliiterManager->SetMinimumPaneSize(nCenterHeight);
		m_pSpliiterManager->SplitHorizontally(m_pTabLeft.get(), m_pTabRight.get());
	}
	else
	{
		m_pSpliiterManager->SetMinimumPaneSize(nCenterWidth);
		m_pSpliiterManager->SplitVertically(m_pTabLeft.get(), m_pTabRight.get());
	}
}

void CSplitterImpl::CreateAndDeleteSplitTab()
{
    bool bSplit = theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE ? true : false;
    if (bSplit && !m_pTabRight)
        m_pTabRight = std::make_unique<CTabManager>(m_pSpliiterManager.get(), theJsonConfig->GetTabStyle(), false);
    else
    {
        if(!bSplit && m_pTabRight.get())
		{
			if (m_pSpliiterManager->IsSplit())
				m_pSpliiterManager->Unsplit();

			int nPageCount = m_pTabRight->GetPageCount();
			for(int nIndex = 0; nIndex < nPageCount; nIndex++)
			{
				wxWindow* pViewPanel = m_pTabRight->GetPage(nIndex);
				_gContextManager->DeleteFileListViewItem(pViewPanel);
			}

			CTabManager* pRightTab = m_pTabRight.release();
			if(pRightTab)
			{
				wxDELETE(pRightTab);
				m_pTabRight = nullptr;
			}
		}
    }
}
