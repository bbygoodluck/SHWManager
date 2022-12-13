#ifndef SHWFILESYSTEMWATCHERLISTENER_H_INCLUDED
#define SHWFILESYSTEMWATCHERLISTENER_H_INCLUDED

#include "ISHWFileSystemWatcherListener.h"
class CSHWFileSystemWatcherListener : public ISHWFileSystemWatcherListener
{
public:
	CSHWFileSystemWatcherListener();
	CSHWFileSystemWatcherListener(wxWindow* pOwner);
	~CSHWFileSystemWatcherListener();

	void SetOwner(wxWindow* pOwner) {
		m_pOwner = pOwner;
	}

	virtual void OnFileChange(const wxString& strName) override;
	virtual void OnFileAdded(const wxString& strName) override;
	virtual void OnFileRemoved(const wxString& strName) override;
	virtual void OnFileRenamed(const wxString& strOldName, const wxString& strNewName) override;

private:
	wxWindow* m_pOwner;
};

#endif // SHWFILESYSTEMWATCHERLISTENER_H_INCLUDED
