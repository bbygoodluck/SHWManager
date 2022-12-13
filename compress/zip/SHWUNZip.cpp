#include "../../ginc.h"

#include "./include/unzip.h"

#ifdef __WXMSW__
#include "./include/iowin32.h"
#else
#endif

#include "../SHWCompressReleaseImpl.h"
#include "SHWUNZip.h"

CSHWUNZip::CSHWUNZip()
{

}

CSHWUNZip::~CSHWUNZip()
{

}


void CSHWUNZip::Run()
{
	m_strCompressedFile = theCompressRelease->GetCompressedFile();
	m_strUnCompressDir = theCompressRelease->GetDeCompressDir();

	if(m_uzFile != nullptr)
		CloseUnZip();

	if(!OpenUnZip())
	{
		wxMessageBox((wxT("The OpenUnZip function failed")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	if(!m_bThreadCreated)
	{
		wxMessageBox((wxT("The main thread is not created")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	GetThread()->Run();
}

wxThread::ExitCode CSHWUNZip::Entry()
{
	if(!DoExtractZip())
		CloseUnZip();

	wxQueueEvent((DlgDeCompress *)m_pDialog, new wxThreadEvent());
	return (wxThread::ExitCode)0;
}
bool CSHWUNZip::DoExtractZip()
{
	if (!m_uzFile)
		return false;

	if (GetUnzipFileCount() == 0)
		return false;

	if(!GotoFirstFile())
		return false;

	do
	{
		if(theCompressRelease->IsCancel())
			break;

		if(!DoExtractFileFromZip(m_strUnCompressDir))
			return false;
	} while(GotoNextFile());

	CloseUnZip();
	return true;
}


void CSHWUNZip::DoStart()
{

}

bool CSHWUNZip::CloseUnZip()
{
	int nRet = UNZ_OK;
	if(m_uzFile)
	{
		unzCloseCurrentFile(m_uzFile);
		nRet = unzClose(m_uzFile);
		m_uzFile = nullptr;
	}

	return (nRet == UNZ_OK);
}

bool CSHWUNZip::OpenUnZip()
{
	CloseUnZip();
	if(m_strCompressedFile.IsEmpty())
		return false;

	m_uzFile = unzOpen(m_strCompressedFile);
	return (m_uzFile != nullptr);
}

bool CSHWUNZip::DoExtractFileFromZip(const wxString& strDir)
{
	if (!m_uzFile)
		return false;

	wxString strTargetDir(strDir);
	bool bAddSlash = theUtility->Compare(strTargetDir.Right(1), SLASH) == 0 ? false : true;

	UZ_FileInfo info;
	GetExtractFileInfo(info);

	wxString strName(info.szFileName);
	if(info.bFolder)
	{
		wxString strNewPath = strTargetDir;
		strNewPath += bAddSlash ? SLASH + strName : strName;

		if(!wxDirExists(strNewPath))
		{
			bool bCreate = wxMkdir(strNewPath);
			return bCreate;
		}

		return true;
	}

	unsigned long ulTotal = info.ulUncompressedSize;
	if((ulTotal >> 10) > 0)
		ulTotal = ulTotal >> 10;

	((DlgDeCompress *)m_pDialog)->SetExtractTotal(ulTotal);
	wxString strFilePathName(bAddSlash ? strTargetDir + SLASH + strName : strTargetDir + strName);

	COMPRESS_TYPE compType = COMPTYPE_NONE;
	if(!theCompressRelease->IsAllDeCompressSame())
	{
		if(wxFileExists(strFilePathName))
		{	/*
				압축해제시 중복체크를 위해서 subThread 내에서 직접 중복체크 Dialog를 호출 하였으나
				subThread 내에서 Dialog를 호출 할 경우 MainThread에서만 호출이 가능하다고 에러가 발생함
				    - Dialog 의 ShowModal은 MainThread(메인윈도우의 쓰레드)에서만 가능함
                    - 해결방법은 별도의 이벤트 호출을 통해서 처리해야 함
		    */
			theDeCompressDupChk->SetDuplicateFile(strFilePathName, strName, info.ulDosDate, info.ulUncompressedSize);
			wxCommandEvent evt(wxEVT_DECOMPRESS_DUP_CHECK);
			wxPostEvent(theDeCompressDupChk, evt);

			theCompressRelease->SetLock();
			if(theCompressRelease->IsCancel())
				return true;

			compType = theCompressRelease->GetDeCompressType();
		}
	}
	else
		compType = theCompressRelease->GetDeCompressType();

	if(compType == COMPTYPE_UNZIP_SKIP)
		return true;
	if(compType == COMPTYPE_UNZIP_RENAME)
	{
		wxString strNewName = GetRename(strFilePathName, strName);
		strFilePathName = bAddSlash ? strTargetDir + SLASH + strNewName : strTargetDir + strNewName;
	}

	HANDLE hOutputFile = ::CreateFile(strFilePathName,
										GENERIC_WRITE,
										0,
										NULL,
										CREATE_ALWAYS,
										FILE_ATTRIBUTE_NORMAL,
										NULL);

	if (INVALID_HANDLE_VALUE == hOutputFile)
		return false;

	if (unzOpenCurrentFile(m_uzFile) != UNZ_OK)
		return false;

	((DlgDeCompress *)m_pDialog)->SetExtractCurrentFile(strFilePathName);

	int nRet = UNZ_OK;
	char pBuffer[BUFFERSIZE] = {0x00, };

	unsigned long ulReadSize = 0;
	do
	{
		if(theCompressRelease->IsCancel())
			break;

		nRet = unzReadCurrentFile(m_uzFile, pBuffer, BUFFERSIZE);

		if (nRet > 0)
		{
#ifdef __WXMSW__
			// output
			DWORD dwBytesWritten = 0;
			if (!::WriteFile(hOutputFile, pBuffer, nRet, &dwBytesWritten, NULL) || dwBytesWritten != (DWORD)nRet)
			{
				nRet = UNZ_ERRNO;
				break;
			}

			ulReadSize += dwBytesWritten;
			((DlgDeCompress *)m_pDialog)->SetExtractCurrent(ulReadSize >> 10);
#endif
		}
	}
	while (nRet > 0);

#ifdef __WXMSW__
	CloseHandle(hOutputFile);
#else
#endif

	unzCloseCurrentFile(m_uzFile);

	if (nRet == UNZ_OK)
		SetFileModifyTime(strFilePathName, info.ulDosDate);

	return (nRet == UNZ_OK);
}

bool CSHWUNZip::GotoFirstFile()
{
	if (!m_uzFile)
		return false;

	return (unzGoToFirstFile(m_uzFile) == UNZ_OK);
}

bool CSHWUNZip::GotoNextFile()
{
	if (!m_uzFile)
		return false;

	return (unzGoToNextFile(m_uzFile) == UNZ_OK);
}

bool CSHWUNZip::GetExtractFileInfo(UZ_FileInfo& info)
{
	if (!m_uzFile)
		return false;

	unz_file_info uzfi;

	ZeroMemory(&info, sizeof(info));
	ZeroMemory(&uzfi, sizeof(uzfi));

	if (UNZ_OK != unzGetCurrentFileInfo(m_uzFile, &uzfi, info.szFileName, MAX_PATH, NULL, 0, info.szComment, MAX_COMMENT))
		return false;

	info.ulVersion           = uzfi.version;
	info.ulVersionNeeded     = uzfi.version_needed;
	info.ulFlags             = uzfi.flag;
	info.ulCompressionMethod = uzfi.compression_method;
	info.ulDosDate           = uzfi.dosDate;
	info.ulCRC               = uzfi.crc;
	info.ulCompressedSize    = uzfi.compressed_size;
	info.ulUncompressedSize  = uzfi.uncompressed_size;
	info.ulInternalAttrib    = uzfi.internal_fa;
	info.ulExternalAttrib    = uzfi.external_fa;

	// replace filename forward slashes with backslashes
	int nLen = wxString(info.szFileName).Len();

	while (nLen--)
	{
		if (info.szFileName[nLen] == '/')
			info.szFileName[nLen] = '\\';
	}

#ifdef __WXMSW__
	// is it a folder?
	info.bFolder = ((info.ulExternalAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
#else
#endif

	return true;
}

int  CSHWUNZip::GetUnzipFileCount()
{
	if (!m_uzFile)
		return 0;

	unz_global_info info;

	if (unzGetGlobalInfo(m_uzFile, &info) == UNZ_OK)
		return (int)info.number_entry;

	return 0;
}
