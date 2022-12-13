#include "../../ginc.h"
#include "./include/zip.h"

#ifdef __WXMSW__
	#include "./include/iowin32.h"
#else
#endif

#include "../SHWCompressReleaseImpl.h"
#include "SHWZip.h"

CSHWZip::CSHWZip()
{

}

CSHWZip::~CSHWZip()
{
	CloseZip();
}

bool CSHWZip::CloseZip()
{
	int nRet = m_uzFile ? zipClose(m_uzFile, nullptr) : ZIP_OK;
	m_uzFile = nullptr;

	wxZeroMemory(m_info);

	return (nRet == ZIP_OK);
}

void CSHWZip::Run()
{
	if(m_uzFile != nullptr)
		CloseZip();

	m_vecCompressList = theCompressRelease->GetCompressingDatas();
	m_strCompressedFile = theCompressRelease->GetCompressedFile();

	if(m_vecCompressList.size() == 0 || m_strCompressedFile.IsEmpty())
	{
		wxMessageBox((wxT("The Item size is 0 or not to be set compressed file name")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	if(!OpenZip(true))
	{
		wxMessageBox((wxT("The OpenZip function failed")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	if(!m_bThreadCreated)
	{
		wxMessageBox((wxT("The main thread is not created")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	GetThread()->Run();
}

wxThread::ExitCode CSHWZip::Entry()
{
	DoStart();

	wxQueueEvent(((DlgCompress *)m_pDialog), new wxThreadEvent());
	return (wxThread::ExitCode)0;
}

void CSHWZip::DoStart()
{
	int nCompleted = 0;

	for(auto item : m_vecCompressList)
	{
		if(theCompressRelease->IsCancel())
			break;

		wxString strAddName(wxT(""));
		if(wxDirExists(item))
		{
			nCompleted++;
			((DlgCompress *)m_pDialog)->SetCompressTotal(nCompleted);

			strAddName = theUtility->GetPathName(item);
			AddFolderToZip(item, strAddName);
			continue;
		}

		nCompleted++;
		((DlgCompress *)m_pDialog)->SetCompressTotal(nCompleted);

		strAddName = theUtility->GetFileName(item);
		AddFileToZip(item, strAddName);
	}
}

bool CSHWZip::AddFileToZip(const wxString& strPathName, const wxString& strAddName)
{
	if(theCompressRelease->IsCancel())
		return true;

	if (!m_uzFile)
		return false;

		int nRet;
	// save file attributes
	zip_fileinfo zfi;

	zfi.internal_fa = 0;
#ifdef __WXMSW__
	zfi.external_fa = ::GetFileAttributes(strPathName);

	// save file time
	SYSTEMTIME st;
	GetLastModified(strPathName, st, true);

	zfi.dosDate = 0;
	zfi.tmz_date.tm_year = st.wYear;
	zfi.tmz_date.tm_mon = st.wMonth - 1;
	zfi.tmz_date.tm_mday = st.wDay;
	zfi.tmz_date.tm_hour = st.wHour;
	zfi.tmz_date.tm_min = st.wMinute;
	zfi.tmz_date.tm_sec = st.wSecond;

	// load input file
	HANDLE hInputFile = ::CreateFile(strPathName,
									GENERIC_READ,
									0,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_READONLY,
									NULL);

	if (hInputFile == INVALID_HANDLE_VALUE)
		return false;
#endif

	nRet = zipOpenNewFileInZip(m_uzFile,
								strAddName,
								&zfi,
								NULL,
								0,
								NULL,
								0,
								NULL,
								Z_DEFLATED,
								Z_DEFAULT_COMPRESSION);

	if (nRet == ZIP_OK)
	{
		wxFileName fn(strPathName);
		wxULongLong ull = fn.GetSize();
		unsigned long ulTotal = ull.ToULong();
		if((ulTotal >> 10) > 0)
			ulTotal = ulTotal >> 10;

		((DlgCompress *)m_pDialog)->SetCurrentFile(strPathName);
		((DlgCompress *)m_pDialog)->SetCurrentFileTotalSize(ulTotal);

		m_info.nFileCount++;

		// read the file and output to zip
		char pBuffer[BUFFERSIZE] = {0x00, };

#ifdef __WXMSW__
		DWORD dwBytesRead = 0;
		DWORD dwFileSize = 0;

		while (nRet == ZIP_OK && ::ReadFile(hInputFile, pBuffer, BUFFERSIZE, &dwBytesRead, NULL))
		{
			if(theCompressRelease->IsCancel())
				break;

			dwFileSize += dwBytesRead;
			((DlgCompress *)m_pDialog)->SetCurrentFileSize(dwFileSize >> 10);

			if (dwBytesRead)
				nRet = zipWriteInFileInZip(m_uzFile, pBuffer, dwBytesRead);
			else
				break;
		}
#endif
		m_info.ulUncompressedSize += dwFileSize;
	}

	zipCloseFileInZip(m_uzFile);

#ifdef __WXMSW__
	::CloseHandle(hInputFile);
#endif
	return (nRet == ZIP_OK);
}

bool CSHWZip::AddFolderToZip(const wxString& strPathName, const wxString& strAddName)
{
	if(theCompressRelease->IsCancel())
		return true;

	if(!m_uzFile)
		return false;

	m_info.nFolderCount++;

	zip_fileinfo zfi;
	zfi.internal_fa = 0;

#ifdef __WXMSW__
	zfi.external_fa = ::GetFileAttributes(strPathName);

	SYSTEMTIME st;
	GetLastModified(strPathName, st, true);

	zfi.dosDate = 0;
	zfi.tmz_date.tm_year = st.wYear;
	zfi.tmz_date.tm_mon = st.wMonth - 1;
	zfi.tmz_date.tm_mday = st.wDay;
	zfi.tmz_date.tm_hour = st.wHour;
	zfi.tmz_date.tm_min = st.wMinute;
	zfi.tmz_date.tm_sec = st.wSecond;

	wxString strFolderFileName(strAddName);
	strFolderFileName += SLASH;
	int nRet = zipOpenNewFileInZip(m_uzFile,
		strFolderFileName,
		&zfi,
		NULL,
		0,
		NULL,
		0,
		NULL,
		Z_DEFLATED,
		Z_DEFAULT_COMPRESSION);

	zipCloseFileInZip(m_uzFile);

	wxString strSearch(strPathName);
	strSearch += SLASH;
	strSearch += wxT("*");

	WIN32_FIND_DATA finfo;
	HANDLE hSearch = FindFirstFile(strSearch, &finfo);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(theCompressRelease->IsCancel())
				break;

			wxString strFullPath(strPathName);
			wxString strNewAddName(strFolderFileName);
			if (finfo.cFileName[0] != '.')
			{
				strFullPath += SLASH;
				strFullPath += finfo.cFileName;

				strNewAddName += finfo.cFileName;

				if (finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					AddFolderToZip(strFullPath, strNewAddName);
				else
					AddFileToZip(strFullPath, strNewAddName);
			}
		} while (FindNextFile(hSearch, &finfo));

		FindClose(hSearch);
	}
#else
#endif

	return true;
}

bool CSHWZip::OpenZip(bool bAppend)
{
	CloseZip();
	if(m_strCompressedFile.IsEmpty() || !m_strCompressedFile.Len())
		return false;

	if(bAppend && !wxFileExists(m_strCompressedFile))
		bAppend = false;

	m_uzFile = zipOpen(m_strCompressedFile, bAppend ? 1 : 0);
	return (m_uzFile != nullptr);
}

void CSHWZip::GetFileInfo(Z_FileInfo& info)
{
	info = m_info;
}
