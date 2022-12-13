#ifndef SPLITTERIMPL_H_INCLUDED
#define SPLITTERIMPL_H_INCLUDED

#include "SplitterManager.h"
#include "TabManager.h"

class CSplitterImpl
{
private:
	explicit CSplitterImpl() {};
    ~CSplitterImpl();

public:
	static CSplitterImpl *Get();

    CSplitterManager* GetSplitter()
    {
        if(m_pSpliiterManager)
            return m_pSpliiterManager.get();

        return nullptr;
    }

	void SetSashGravity(double gravity);
	void SplitWindow(const wxSize& _size = wxSize(0,0));

private:
    void Init();
   	void CreateAndDeleteSplitTab();
	void SetWindowArrange(int nViewStyle, const wxSize& size);

private:
	bool m_Initialized = false;
	static CSplitterImpl* m_pSplitterInstance;

	std::unique_ptr<CSplitterManager> m_pSpliiterManager = nullptr;
	std::unique_ptr<CTabManager> m_pTabLeft = nullptr;
	std::unique_ptr<CTabManager> m_pTabRight = nullptr;

};

#endif // SPLITTERIMPL_H_INCLUDED
