#ifndef DRIVEINFO_H_INCLUDED
#define DRIVEINFO_H_INCLUDED

#include "DriveItem.h"
class CDriveInfo final
{
private:
    CDriveInfo() {}

public:
    ~CDriveInfo() {}
    static CDriveInfo* Get();

	//드라이브용량가져오기
	void GetDiskSpace(const wxString& strVolume, double& dblTotalSpace, double& dblFreeSpace);
	void SetDriveSpace(const wxString& strVolume);
	void SetDriveSpace(CDriveItem* pDriveItem);

public:
    void Init();
	bool IsDriveExist(const wxString& strVolume);
	int GetDriveCount() { return m_nDriveCount; }

	CDriveItem* GetDriveItem(int nIndex);
	CDriveItem* GetDriveItem(const wxString& strVolume);

private:
	wxString MakeDisplayName(const wxString& strVolume, const wxString& strVolumeDisplayName, bool IsNetDrive);
	bool GetNetIPFromDriveInfo(const wxString& strDriveInfo, wxString& strIPInfo, wxString& strDisplayName);

	CDriveItem* GetItem(const wxString& strVolume);

private:
	static std::unique_ptr<CDriveInfo> m_pInstance;

	std::unordered_map<wxString, CDriveItem> m_driveItems;
	wxVector<wxString> m_vecDriveList;

	int m_nDriveCount = 0;
};

#endif // DRIVEINFO_H_INCLUDED
