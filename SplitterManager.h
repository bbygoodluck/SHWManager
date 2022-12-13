#ifndef SPLITTERMANAGER_H_INCLUDED
#define SPLITTERMANAGER_H_INCLUDED

class CSplitterManager : public wxSplitterWindow
{
public:
	CSplitterManager(wxWindow* pWindow);
	virtual ~CSplitterManager();

private:
	// event handlers
	void OnPositionChanged(wxSplitterEvent& event);
	void OnPositionChanging(wxSplitterEvent& event);
	void OnDClick(wxSplitterEvent& event);
	void OnUnsplitEvent(wxSplitterEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif // SPLITTERMANAGER_H_INCLUDED
