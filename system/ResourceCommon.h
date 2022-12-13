#ifndef RESOURCECOMMON_H_INCLUDED
#define RESOURCECOMMON_H_INCLUDED

#ifdef __WXMSW__
	#include <Pdh.h>
	#include <PdhMsg.h>
	#include <Iphlpapi.h>
#endif

class CResourceCommon : public wxEvtHandler
{
public:
	CResourceCommon() {}
	~CResourceCommon() {}

	void Start() {
		m_bStarted = true;
		m_timer.Start(1000);
	}

	void Stop() {
		m_bStarted = false;
		m_timer.Stop();
	}

	void AddEventListener(wxWindow* pWindow) {
		m_evtListeners.push_back(pWindow);
	}

	virtual void Clear() {
		m_evtListeners.clear();

		wxVector<wxWindow *>().swap(m_evtListeners);
		m_evtListeners.reserve(0);
	}

protected:
	void SendEvent() {
		wxVector<wxWindow *>::const_iterator iter = nullptr;
		for(iter = m_evtListeners.begin(); iter != m_evtListeners.end(); ++iter)
		{
			wxWindow* pWindow = *iter;
			wxCommandEvent evt(wxEVT_RESOURCE_MONITORING);

			if(!m_bStarted)
				return;

			if(pWindow)
				wxPostEvent(pWindow, evt);
		}
	}

protected:
	wxTimer m_timer;
	bool m_bStarted = false;

	wxVector<wxWindow *> m_evtListeners;

protected:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void OnTimer(wxTimerEvent& event) = 0;

};

#endif // RESOURCECOMMON_H_INCLUDED
