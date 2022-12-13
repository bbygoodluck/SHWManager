#include "../ginc.h"
#include "DirData.h"
#include <wx/bitmap.h>

CDirData::CDirData()
	: m_strName(wxT(""))
	, m_strPath(wxT(""))
	, m_strDriveName(wxT(""))
	, m_strDisp(wxT(""))
	, m_strDriveSpace(wxT(""))
	, m_isDir(false)
	, m_isFile(false)
	, m_isDrive(false)
	, m_bSelected(false)
	, m_bMatched(false)
	, m_isCut(false)
	, m_isShortDisp(false)
	, m_ulattr(999999999)
	, m_llSize(0)
	, m_iIcon(-1)
	, m_iByteCount(0)
	, m_dt(0.0)
	, m_iType(item_type::unknown)

{
	Init();
}

CDirData::~CDirData()
{
}

void CDirData::Init()
{
	m_strName		= wxT("");
	m_strPath		= wxT("");
	m_strDriveName	= wxT("");
	m_strDriveSpace = wxT("");
	m_isDir			= false;
	m_isFile		= false;
	m_isDrive		= false;
	m_bSelected		= false;
	m_isCut			= false;
	m_ulattr		= 999999999;
	m_llSize		= 0;
	m_dt			= 0.0;
	m_iType			= item_type::unknown;
	m_iIcon			= -1;
	m_iByteCount    = 0;
}

void CDirData::SetName(const wxString& strName)
{
	m_strName = strName;
}

void CDirData::SetPath(const wxString& strPath)
{
	m_strPath = strPath;
}

void CDirData::SetType(item_type iType)
{
	m_iOvelayIndex = -1;
	m_iType = iType;
	switch(m_iType)
	{
		case item_type::dir:
			m_isDir = true;
			m_iIconIndex = 3;
			break;

		case item_type::file:
			m_isFile = true;
			m_iIconIndex = 0;
			break;

		case item_type::drive:
			m_isDrive = true;
			break;

		default:
			break;
	}
}

void CDirData::SetAttribute(unsigned long ulAttr)
{
	m_ulattr = ulAttr;
}

void CDirData::SetSize(const wxLongLong& llSize)
{
	m_llSize = llSize;
	if (m_iType == item_type::file)
	{
		wxString strSizeType;
		wxString strSize;
		m_colorType = wxColour(192, 192, 192);

		double dblFileSize = llSize.ToDouble();
		theUtility->GetSizeInfo(dblFileSize, m_strSize, m_strSizeType, m_colorType);
	}
}

void CDirData::SetDateTime(const wxDateTime& dt)
{
	m_dt = 0.0;
	m_dt = dt;
}

void CDirData::SetItemSelected(const bool bSelected)
{
	m_bSelected = bSelected;
}

void CDirData::SetIconIndex(int iIcon)
{
	m_iIcon = iIcon;
}

void CDirData::SetIcon(const wxIcon& ico)//wxIcon icon)
{
	m_Icon = ico;
}

void CDirData::SetDriveName(const wxString& strDriveName)
{
	m_strDriveName = strDriveName;
}

void CDirData::SetDriveSpaceInfo(const wxString& strDriveSpaceInfo)
{
	m_strDriveSpace = strDriveSpaceInfo;
}

void CDirData::SetDisp(const wxString& strDisp)
{
	m_strDisp = strDisp;
}

void CDirData::SetTypeName(const wxString& strTypeName)
{
	m_strTypeName = strTypeName;
}

const wxString CDirData::GetFullPath()
{
	return ((theUtility->Compare(SLASH, m_strPath.Right(1)) == 0) ? m_strPath + m_strName :  m_strPath + SLASH + m_strName);
}

size_t CDirData::GetByteCount()
{
	return strlen(m_strName);
}

void CDirData::SetVolume(const wxString& strVolume)
{
	m_strVolume = strVolume;
}
