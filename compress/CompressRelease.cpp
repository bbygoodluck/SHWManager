#include "../ginc.h"
#include "CompressRelease.h"
#include "CompressReleaseInc.h"

std::unique_ptr<CCompressRelease> CCompressRelease::m_pInstance = nullptr;

CCompressRelease* CCompressRelease::Get()
{
	if (m_pInstance.get() == nullptr)
	{
		m_pInstance.reset(new CCompressRelease());
		m_pInstance->Init();
	}

	return m_pInstance.get();
}

void CCompressRelease::Init()
{
	m_vCompressReleaseExt.emplace_back(wxT("zip"));
	m_vCompressReleaseExt.emplace_back(wxT("gz"));
}

bool CCompressRelease::IsCompressedFile(const wxString& strExt)
{
	bool bCompressedFile = false;
	wxString strExtItem(strExt);
	for(auto item : m_vCompressReleaseExt)
	{
		if(theUtility->Compare(item.MakeLower(), strExtItem.MakeLower()) == 0)
		{
			bCompressedFile = true;
			break;
		}
	}

	return bCompressedFile;
}

void CCompressRelease::SetOwnerDialog(wxDialog* pDialog)
{
	m_pCompressReleaseDlg = pDialog;
}

void CCompressRelease::Start()
{
	if(m_pCompressReleaseDlg == nullptr)
	{
		wxMessageBox(wxT("Need to set owner dialog : m_pCompressReleaseDlg is nullptr"), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	if(m_CompressType == COMPTYPE_NONE)
		return;

	if(m_pCompressReleaseImpl)
		wxDELETE(m_pCompressReleaseImpl);

	m_pCompressReleaseImpl = nullptr;

	switch(m_CompressType)
	{
		case COMPTYPE_ZIP:
			m_pCompressReleaseImpl = new CSHWZip();
			break;

		case COMPTYPE_UNZIP:
			m_pCompressReleaseImpl = new CSHWUNZip();
			break;

		case COMPTYPE_GZ:
			m_pCompressReleaseImpl = new CSHWGZip();
			break;

		case COMPTYPE_UNGZ:
			m_pCompressReleaseImpl = new CSHWUNGZip();
			break;

		default:
			return;
	}

	m_pCompressReleaseImpl->SetProgressDialog(m_pCompressReleaseDlg);
	m_pCompressReleaseImpl->Run();
}

bool CCompressRelease::SetCompressInfo(const wxString& strFullPath, const wxString& strCompressedFile, COMPRESS_TYPE compressType)
{
	std::vector<wxString> vecDatas;
	vecDatas.emplace_back(strFullPath);

	return SetCompressInfo(vecDatas, strCompressedFile, compressType);
}

bool CCompressRelease::SetCompressInfo(const std::vector<wxString>& vecDatas, const wxString& strCompressedFile, COMPRESS_TYPE compressType)
{
	m_bDeCompress = false;
	m_strCompressedFile = strCompressedFile;
	m_vecCompressingDatas = vecDatas;

	m_CompressType = compressType;

	return true;
}

bool CCompressRelease::SetDeCompressedInfo(const wxString& strCompressedFile, const wxString& strDeCompressDir, COMPRESS_TYPE compressType)
{
	m_bDeCompress = true;
	m_strCompressedFile = strCompressedFile;
	m_strDeCompressDir = strDeCompressDir;

	m_CompressType = compressType;

	return true;
}

//메모리 해제
void CCompressRelease::ClearCompressInfo()
{	//압축중 취소
	if(!m_bDeCompress)
	{	//압축중인 .zip 파일 삭제
		if(m_bCancel)
			wxRemoveFile(m_strCompressedFile);
	}
	//아직 압축(또는 압축해제) 실행중
	if(m_pCompressReleaseImpl->GetThread() && m_pCompressReleaseImpl->GetThread()->IsRunning())
		m_pCompressReleaseImpl->GetThread()->Wait();
	//메모리 해제
	if(m_pCompressReleaseImpl)
		delete m_pCompressReleaseImpl;
	//초기화
	m_pCompressReleaseImpl = nullptr;
	m_bCancel = false;
	m_bAllDeCompressSame = false;
	m_CompressType = COMPTYPE_NONE;

	m_vecCompressingDatas.clear();
}

void CCompressRelease::SetLock()
{
	lock.getCondition()->Wait();
}

void CCompressRelease::SetRelease()
{
	lock.getCondition()->Signal();
}

COMPRESS_TYPE CCompressRelease::GetCompressReleaseType(const wxString& strExt, bool bCompress = true)
{
	if(IsCompressedFile(strExt))
	{
		if(bCompress)
		{
			if(strExt.CmpNoCase(wxT("zip")) == 0)
				return COMPTYPE_ZIP;

			if(strExt.CmpNoCase(wxT("gz")) == 0)
				return COMPTYPE_GZ;
		}
		else
		{
			if(strExt.CmpNoCase(wxT("zip")) == 0)
				return COMPTYPE_UNZIP;

			if(strExt.CmpNoCase(wxT("gz")) == 0)
				return COMPTYPE_UNGZ;
		}
	}

	return COMPTYPE_NONE;
}
