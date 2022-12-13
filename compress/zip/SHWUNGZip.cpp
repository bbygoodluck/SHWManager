#include "../../ginc.h"
#include "../SHWCompressReleaseImpl.h"
#include "SHWUNGZip.h"


CSHWUNGZip::CSHWUNGZip()
{

}

CSHWUNGZip::~CSHWUNGZip()
{

}

void CSHWUNGZip::Run()
{
	m_strCompressedFile = theCompressRelease->GetCompressedFile();
	m_strUnCompressDir = theCompressRelease->GetDeCompressDir();

	if(!m_bThreadCreated)
	{
		wxMessageBox((wxT("The main thread is not created")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	GetThread()->Run();
}


wxThread::ExitCode CSHWUNGZip::Entry()
{
	DoStart();

	wxQueueEvent((DlgDeCompress *)m_pDialog, new wxThreadEvent());
	return (wxThread::ExitCode)0;
}

void CSHWUNGZip::DoStart()
{
	char buf[BUFLEN] = {0x00, };

	wxString strFileName = theUtility->GetFileName(m_strCompressedFile, false);
	wxString strFullPathName(m_strUnCompressDir);
	bool bAddSlash = false;

	if (m_strUnCompressDir.Last() != '/' && m_strUnCompressDir.Last() != '\\')
	{
		strFullPathName += SLASH;
		strFullPathName += strFileName;
		bAddSlash = true;
	}
	else
		strFullPathName += strFileName;

	COMPRESS_TYPE compType = COMPTYPE_NONE;
	if(!theCompressRelease->IsAllDeCompressSame())
	{
		if(wxFileExists(strFullPathName))
		{
			theDeCompressDupChk->SetDuplicateFile(strFullPathName, strFileName, 0, 0);
			wxCommandEvent evt(wxEVT_DECOMPRESS_DUP_CHECK);
			wxPostEvent(theDeCompressDupChk, evt);

			theCompressRelease->SetLock();
			if(theCompressRelease->IsCancel())
				return;

			compType = theCompressRelease->GetDeCompressType();
		}
	}
	else
		compType = theCompressRelease->GetDeCompressType();

	if(compType == COMPTYPE_UNZIP_SKIP)
		return;

	if(compType == COMPTYPE_UNZIP_RENAME)
	{
		wxString strNewName = GetRename(strFullPathName, strFileName);
		strFullPathName = bAddSlash ? m_strUnCompressDir + SLASH + strNewName : m_strUnCompressDir + strNewName;
	}

	//zlib의 gzseek는 SEEK_END를 지원하지 않으므로 fseek를 이용하여 처리하여야 함.
	FILE* fIn = fopen(m_strCompressedFile.char_str(), "rb");
	fseek(fIn, -4, SEEK_END);
	unsigned long ulTotal = ftell(fIn);

	fclose(fIn);

	m_gzIn = gzopen(m_strCompressedFile.char_str(), "rb");
	m_OutFile = fopen(strFullPathName.char_str(), "wb");

	((DlgDeCompress *)m_pDialog)->SetExtractCurrentFile(strFullPathName);
	if((ulTotal >> 10) > 0)
		ulTotal = ulTotal >> 10;

	((DlgDeCompress *)m_pDialog)->SetExtractTotal(ulTotal);

	gzseek(m_gzIn, 0, SEEK_SET);
	int nLen = 0;
	unsigned long ulReadSize = 0;
	while(1)
	{
		nLen = gzread(m_gzIn, buf, sizeof(buf));
		if (nLen == 0) break;

		if ((int)fwrite(buf, 1, (unsigned)nLen, m_OutFile) != nLen)
			break;

		ulReadSize += nLen;
		if(ulTotal != 0)
			((DlgDeCompress *)m_pDialog)->SetExtractCurrent(ulReadSize >> 10);
	}

	fclose(m_OutFile);
	gzclose(m_gzIn);
}
