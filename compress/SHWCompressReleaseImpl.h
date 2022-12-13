#ifndef SHWCOMPRESSRELEASEIMPL_H_INCLUDED
#define SHWCOMPRESSRELEASEIMPL_H_INCLUDED

#include "../dialog/DlgCompress.h"
#include "../dialog/DlgDeCompress.h"

constexpr unsigned int BUFFERSIZE = 2048;
constexpr unsigned int MAX_COMMENT = 255;

class CSHWCompressReleaseImpl : public wxThreadHelper
{
protected:
	const unsigned int BUFLEN = 16384;

public:
	CSHWCompressReleaseImpl();
	~CSHWCompressReleaseImpl();

	bool SetProgressDialog(wxDialog* pDialog);
	virtual void Run() = 0;

protected:
	virtual wxThread::ExitCode Entry() = 0;
	virtual void DoStart() = 0;

	wxString GetRename(const wxString& strFullPathName, const wxString& strOldName);
#ifdef __WXMSW__
	bool GetLastModified(const wxString& strPathName, SYSTEMTIME& sysTime, bool bLocalTime);
	bool SetFileModifyTime(const wxString& strFilePathName, DWORD dwDosDate);
#else
	time_t GetLastModified(const wxString& strPathName);
#endif

protected:
	wxDialog* m_pDialog = nullptr;

	std::vector<wxString> m_vecCompressList;

	wxString m_strCompressedFile = wxT("");
	wxString m_strUnCompressDir = wxT("");

	bool m_bThreadCreated = false;
};

#endif // SHWCOMPRESSRELEASEIMPL_H_INCLUDED
