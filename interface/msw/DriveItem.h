#ifndef DRIVEITEM_H_INCLUDED
#define DRIVEITEM_H_INCLUDED

class CDriveItem final
{
public:
	explicit CDriveItem() {}
	~CDriveItem() {}

	void SetVolume(const wxString& strVolume) { m_strVolume = strVolume; }
	wxString GetVolume() { return m_strVolume; }

	void SetDriveName(const wxString& strDriveName) { m_strDriveName = strDriveName; }
	wxString GetDriveName() { return m_strDriveName; }

	void SetDisplayName(const wxString& strDisplayName) { m_strDisplayName = strDisplayName; }
	wxString GetDisplayName() { return m_strDisplayName; }

	void SetDriveKind(wxFSVolumeKind fsvKind) { m_fsvDriveKind = fsvKind; }
	wxFSVolumeKind GetDriveKind() { return m_fsvDriveKind; }

	void SetDriveTypeName(const wxString& strDriveType) { m_strTypeName = strDriveType;}
	wxString GetDriveType() { return m_strTypeName; }

	void SetDriveSpace(double dblTotalSpace, double dblUseSpace) {
		m_dblTotalSpace = dblTotalSpace;
		m_dblUseSpace   = dblUseSpace;
	}

	wxString GetDriveSpace()
	{
		if( (m_fsvDriveKind == wxFS_VOL_CDROM) ||
			(m_fsvDriveKind == wxFS_VOL_DVDROM) )
				return wxT("");

		wxString strTotalSpace = wxString::Format(wxT("%5.1f GB"), (m_dblTotalSpace / GIGABYTE));
		wxString strFreeSpace = wxString::Format(wxT("%5.1f GB"), (m_dblUseSpace / GIGABYTE));

		return (strFreeSpace + wxT(" / ") + strTotalSpace);
	}

	void GetDriveSpace(double& dblTotalSpace, double& dblUseSpace) {
		dblTotalSpace = m_dblTotalSpace;
		dblUseSpace = m_dblUseSpace;
	}

	bool IsNetDrive() {
		return m_fsvDriveKind == wxFS_VOL_NETWORK;
	}

	void SetIconIndex(int iIconIndex) { m_iIconIndex = iIconIndex; }
	const int GetIconIndex() const { return m_iIconIndex; }

	void SetOverlayIconIndex(int iOverlayIndex) { m_iOverlayIndex = iOverlayIndex; }
	const int GetOverlayIconIndex() const { return m_iOverlayIndex; }

private:
	wxString m_strVolume        = wxT("");
   	wxString m_strDriveName     = wxT("");
	wxString m_strDisplayName   = wxT("");
	wxString m_strSpace         = wxT("");
	wxString m_strTypeName      = wxT("");
	wxString m_strDisplayEtc    = wxT("");

	wxFSVolumeKind m_fsvDriveKind = wxFS_VOL_MAX;

	int m_iIconIndex = -1;
	int m_iOverlayIndex = -1;

	double m_dblTotalSpace = 0.0f;
	double m_dblUseSpace   = 0.0f;
};

#endif // DRIVEITEM_H_INCLUDED
