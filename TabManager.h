#ifndef TABMANAGER_H_INCLUDED
#define TABMANAGER_H_INCLUDED

class CViewPanel;
class CTabManager : public wxAuiNotebook
{
public:
	CTabManager(wxWindow* parent, long style, bool bLTabManager = true);
	virtual ~CTabManager();

public:
	void AddTab(bool bRemote = false);
	void SetTabName(const wxString& strPath);
	bool IsLeftTab() { return m_bLTabManager; }
private:
	void Init();

private:
	wxBitmap m_PageBMP;
	bool m_bLTabManager = false;
	bool m_bLoaded = false;

	wxString m_strDefaultPath = wxT("");

private:
	void OnPageClose(wxAuiNotebookEvent& event);
	void OnPageChanged(wxAuiNotebookEvent& event);

	wxDECLARE_EVENT_TABLE();
};

#endif // TABMANAGER_H_INCLUDED
