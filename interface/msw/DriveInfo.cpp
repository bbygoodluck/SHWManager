#include "../../ginc.h"
#include "DriveInfo.h"

std::unique_ptr<CDriveInfo> CDriveInfo::m_pInstance = nullptr;

CDriveInfo* CDriveInfo::Get()
{
	if (m_pInstance == nullptr)
		m_pInstance.reset(new CDriveInfo());

	return m_pInstance.get();
}

void CDriveInfo::Init()
{
    m_driveItems.clear();
	m_vecDriveList.clear();

    wxArrayString arrdrvList = wxFSVolume::GetVolumes();
	int nTotalDriveCount = arrdrvList.GetCount();

    for (int nCnt = 0; nCnt < nTotalDriveCount; nCnt++)
    {
        wxString strDriveItem = arrdrvList.Item(nCnt);

        int nSlash = strDriveItem.find_first_of(SLASH + SLASH);
		wxFSVolume fsv(strDriveItem);

		if(fsv.IsOk())
		{
			wxFSVolumeKind fskind   = fsv.GetKind();
			wxString strDriveName   = fsv.GetName();
			wxString strDisplayName = fsv.GetDisplayName();
			wxString strVolume      = strDriveName.Left(1);

			if(nSlash == 0)
			{
				wxString strIPInfo(wxT(""));
				if(GetNetIPFromDriveInfo(strDriveItem, strIPInfo, strDisplayName))
					continue;

				strDriveName   = strIPInfo;
				strVolume      = strIPInfo;
			}

			bool IsNetDrive = (fskind == wxFS_VOL_NETWORK) ? true : false;

			wxString strDisplay = MakeDisplayName(strVolume, strDisplayName, IsNetDrive);

			double dblTotalSpace = 0.0f;
			double dblUseSpace   = 0.0f;

			GetDiskSpace(strDriveName, dblTotalSpace, dblUseSpace);

			int nIconIndex = -1;
			int nOverlayIconIndex = -1;
			theUtility->GetIconIndex(strDriveName, nIconIndex, nOverlayIconIndex);

			CDriveItem driveItem;

			driveItem.SetVolume(strVolume);
			driveItem.SetDriveName(strDriveName);
			driveItem.SetDisplayName(strDisplay);
			driveItem.SetDriveKind(fskind);
			driveItem.SetDriveSpace(dblTotalSpace, dblUseSpace);
			driveItem.SetIconIndex(nIconIndex);
			driveItem.SetOverlayIconIndex(nOverlayIconIndex);

			m_driveItems.insert(std::make_pair(strVolume, driveItem));
			m_vecDriveList.push_back(strVolume);
		}
    }

    m_nDriveCount = m_vecDriveList.size();
}

bool CDriveInfo::GetNetIPFromDriveInfo(const wxString& strDriveInfo, wxString& strIPInfo, wxString& strDisplayName)
{
	wxString strTempIPInfo(wxT(""));
	wxStringTokenizer tokenizer(strDriveInfo, SLASH);
	int nTokenCount = tokenizer.CountTokens();
	if (nTokenCount > 0)
	{
		while (tokenizer.HasMoreTokens())
		{
			strTempIPInfo = tokenizer.GetNextToken();
			if(!strTempIPInfo.IsEmpty())
				break;
		}
	}

	strDisplayName = strTempIPInfo;
	strIPInfo = SLASH + SLASH + strTempIPInfo;
	return IsDriveExist(strIPInfo);
}

bool CDriveInfo::IsDriveExist(const wxString& strVolume)
{
	std::unordered_map<wxString, CDriveItem>::const_iterator cfIter = m_driveItems.find(strVolume);
	return cfIter != m_driveItems.end();
}

wxString CDriveInfo::MakeDisplayName(const wxString& strVolume, const wxString& strVolumeDisplayName, bool IsNetDrive)
{
	int nDisplayStartIndx = strVolumeDisplayName.find_first_of(wxT("("));
	wxString strDisplayDiskType = strVolumeDisplayName.Left(nDisplayStartIndx - 1);

	if(IsNetDrive)
		strDisplayDiskType = strDisplayDiskType.insert(0, wxT("NET "));

	wxString strDisp = nDisplayStartIndx == -1 ? strDisplayDiskType : wxString::Format(wxT("[-%s-] %s"), strVolume, strDisplayDiskType);
	return strDisp;
}

void CDriveInfo::GetDiskSpace(const wxString& strVolume, double& dblTotalSpace, double& dblFreeSpace)
{
	wxDiskspaceSize_t total;
	wxDiskspaceSize_t free;

	wxGetDiskSpace(strVolume, &total, &free);

	dblTotalSpace = total.ToDouble();
	dblFreeSpace = free.ToDouble();
}

void CDriveInfo::SetDriveSpace(const wxString& strVolume)
{
	CDriveItem* pDriveItem = GetDriveItem(strVolume);
	if(pDriveItem)
		SetDriveSpace(pDriveItem);
}

void CDriveInfo::SetDriveSpace(CDriveItem* pDriveItem)
{
	double dblTotalSpace = 0.0f;
	double dblUseSpace   = 0.0f;

	GetDiskSpace(pDriveItem->GetDriveName(), dblTotalSpace, dblUseSpace);

	pDriveItem->SetDriveSpace(dblTotalSpace, dblUseSpace);
}

CDriveItem* CDriveInfo::GetDriveItem(const wxString& strVolume)
{
	if(!IsDriveExist(strVolume))
		return nullptr;

	return GetItem(strVolume);
}

CDriveItem* CDriveInfo::GetDriveItem(int nIndex)
{
	if(m_nDriveCount < nIndex)
		return nullptr;

	wxString strVolume = m_vecDriveList.at(nIndex);
	return GetItem(strVolume);
}

CDriveItem* CDriveInfo::GetItem(const wxString& strVolume)
{
	std::unordered_map<wxString, CDriveItem>::iterator cfIter = m_driveItems.find(strVolume);

	if(cfIter == m_driveItems.end() )
		return nullptr;

	return &cfIter->second;
}
