#include "../../ginc.h"
#include "../SHWCompressReleaseImpl.h"
#include "SHWGZip.h"

CSHWGZip::CSHWGZip()
{

}

CSHWGZip::~CSHWGZip()
{

}


void CSHWGZip::Run()
{
	m_vecCompressList = theCompressRelease->GetCompressingDatas();
	m_strCompressedFile = theCompressRelease->GetCompressedFile();

	if(m_vecCompressList.size() == 0 || m_strCompressedFile.IsEmpty())
	{
		wxMessageBox((wxT("The Item size is 0 or not to be set compressed file name")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	if(m_vecCompressList.size() > 1)
	{
		wxMessageBox((wxT("The gzip can compress one file")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	if(!m_bThreadCreated)
	{
		wxMessageBox((wxT("The main thread is not created")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	m_strSource = m_vecCompressList.at(0);

	wxFileName fn(m_strSource);
	wxULongLong wxll = fn.GetSize();
	unsigned long ulTotal = wxll.ToULong() >> 10;

	((DlgCompress *)m_pDialog)->SetCurrentFile(m_strSource);
	((DlgCompress *)m_pDialog)->SetCurrentFileTotalSize(ulTotal);

	GetThread()->Run();
}


wxThread::ExitCode CSHWGZip::Entry()
{
	DoStart();

	wxQueueEvent(((DlgCompress *)m_pDialog), new wxThreadEvent());
	return (wxThread::ExitCode)0;
}

void CSHWGZip::DoStart()
{
	m_file = fopen(m_strSource.char_str(), "rb");
	if(m_file == NULL)
		return;

	m_gzOut = 	gzopen(m_strCompressedFile.char_str(), "wb");
	if(m_gzOut == NULL)
		return;


	char buf[BUFLEN] = {0x00, };
	int len = 0;

	unsigned long ulSize = 0;
	while(1)
	{
        len = (int)fread(buf, 1, sizeof(buf), m_file);
        if (len == 0) break;

		ulSize += len;
		((DlgCompress *)m_pDialog)->SetCurrentFileSize(ulSize >> 10);

        if (gzwrite(m_gzOut, buf, (unsigned)len) != len)
			break;
    }

    fclose(m_file);
    gzclose(m_gzOut);
}
