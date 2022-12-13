#ifndef VIEWMANAGERITEM_H_INCLUDED
#define VIEWMANAGERITEM_H_INCLUDED

class CTabManager;
class CViewPanel;
class CPathView;
class CFileListView;

class CContextManagerItem
{
public:
	CContextManagerItem() {};
	~CContextManagerItem() {};

public:
	CTabManager*   m_pTabManager;
	CViewPanel*    m_pViewPanel;
	CPathView*     m_pPathView;
	CFileListView* m_pFileListView;
};

#endif // VIEWMANAGERITEM_H_INCLUDED
