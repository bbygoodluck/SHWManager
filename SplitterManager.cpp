#include "ginc.h"
#include "SplitterManager.h"

wxBEGIN_EVENT_TABLE(CSplitterManager, wxSplitterWindow)
	EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, CSplitterManager::OnPositionChanged)
	EVT_SPLITTER_SASH_POS_CHANGING(wxID_ANY, CSplitterManager::OnPositionChanging)
	EVT_SPLITTER_DCLICK(wxID_ANY, CSplitterManager::OnDClick)
	EVT_SPLITTER_UNSPLIT(wxID_ANY, CSplitterManager::OnUnsplitEvent)
wxEND_EVENT_TABLE()


CSplitterManager::CSplitterManager(wxWindow* pWindow)
    : wxSplitterWindow(pWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, /*wxSP_3D*/ wxSP_NOBORDER | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN | wxCLIP_SIBLINGS | wxSP_NO_XP_THEME)
{

}

CSplitterManager::~CSplitterManager()
{

}

void CSplitterManager::OnPositionChanged(wxSplitterEvent& event)
{
	event.Skip();
}

void CSplitterManager::OnPositionChanging(wxSplitterEvent& event)
{
	event.Skip();
}

void CSplitterManager::OnDClick(wxSplitterEvent& event)
{
	event.Skip();
}

void CSplitterManager::OnUnsplitEvent(wxSplitterEvent& event)
{
	event.Skip();
}
