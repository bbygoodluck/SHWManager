#ifndef __DIR_DATA_H__
#define __DIR_DATA_H__

class CDirData
{
public:
	CDirData();
	~CDirData();

	enum item_type {
		unknown = -1,
		dir,
		file,
		drive
	};

	unsigned int m_iconvalue = 0;
	int m_iIconIndex = 0;
	int m_iOvelayIndex = 0;
	bool m_bIconImageSet = false;
public:
	//이름설정
	void SetName(const wxString& strName);
	//경로설정
	void SetPath(const wxString& strPath);
	//화면표시
	void SetDisp(const wxString& strDisp);
	//타입내용
	void SetTypeName(const wxString& strTypeName);
	void SetType(item_type iType);
	void SetAttribute(unsigned long ulAttr);
	void SetSize(const wxLongLong& llSize);
	void SetDateTime(const wxDateTime& dt);
	void SetItemSelected(const bool bSelected);
	void SetIconIndex(int iIcon);
	void SetIcon(const wxIcon& ico);
	//드라이브 이름설정
	void SetDriveName(const wxString& strDriveName);
	//드라이브 용량정보
	void SetDriveSpaceInfo(const wxString& strDriveSpaceInfo);
	//드라이브 정보 가져오기
	const wxString& GetDriveSpaceInfo() const { return m_strDriveSpace; }

	void SetMatch(bool isMatch) {
		m_bMatched = isMatch;
	}

	void SetCut(bool isCut) {
		m_isCut = isCut;
	}

	void SetShortName(bool isShortName) {
		m_isShortDisp = isShortName;
	}
	//이름가져오기
	const wxString& GetName() const { return m_strName; }
	//화면표시 가져오기
	const wxString& GetDisp() const { return m_strDisp; }
	//경로가져오기
	const wxString& GetPath() const { return m_strPath; }
	//타입명 가져오기
	const wxString& GetTypeName() const { return m_strTypeName; }
	//전체경로가져오기
	const wxString GetFullPath();
	const item_type GetType() { return m_iType; }

	unsigned long GetAttribute() { return m_ulattr; }
	wxString GetAttributeToString() const {
		wxString strAttr(wxT(""));

		if (m_ulattr & ATTR_RDONLY) strAttr += "R";
		else strAttr += "_";

		//Hidden
		if (m_ulattr & ATTR_HIDDEN) strAttr += "H";
		else strAttr += "_";

		//System
		if (m_ulattr & ATTR_SYSTEM) strAttr += "S";
		else strAttr += "_";

		//Arch
		if (m_ulattr & ATTR_ARCHIVE) strAttr += "A";
		else strAttr += "_";

		return strAttr;
	}

	wxLongLong GetSize() const { return m_llSize; }
	wxDateTime GetDateTime() const { return m_dt; }

	wxString GetDateTimeToString() const {
		if(!m_dt.IsValid())
			return wxT("0000-00-00 00:00");

		wxLongLong llValue = m_dt.GetValue();
		if(llValue < 0)
			return wxT("0000-00-00 00:00");

		int iYear = m_dt.GetYear();
		int iMonth = m_dt.GetMonth() + 1;
		int iDay = m_dt.GetDay();
		int iHour = m_dt.GetHour();
		int iMin = m_dt.GetMinute();

		return (wxString::Format(TIME_FORMAT_DEFAULT, iYear, iMonth, iDay, iHour, iMin));
	}

	bool IsDir() const { return m_isDir; }
	bool IsFile() const { return m_isFile; }
	bool IsDrive() const { return m_isDrive; }

	const bool IsItemSelected() const { return m_bSelected; }
	const int GetIconIndex() const { return m_iIcon; }
	const wxIcon& GetIcon() const { return m_Icon; }

	const wxString& GetDriveName() const { return m_strDriveName; }

	const bool IsMatch() const { return m_bMatched; }
	const bool IsCut() const   { return m_isCut; }
	const bool IsShortName() const { return m_isShortDisp; }

	//2019.04.23 ADD
	const wxString& GetSizeString() const { return m_strSize; }
	const wxString& GetSizeTypeString() const { return m_strSizeType; }
	wxColour GetSizeColor() { return m_colorType; }

	//2019.08.07 ADD
	void SetExt(const wxString& strExt) {
		m_strExt = strExt;
	}

	wxString GetExt() { return m_strExt;}
	size_t GetByteCount();

	void SetVolume(const wxString& strVolume);
	wxString GetVolume() const { return m_strVolume; }
#ifdef __WXMSW__
	void SetDriveType(wxFSVolumeKind fsvKind) { m_fsvDriveKind = fsvKind; }
	wxFSVolumeKind GetDriveType() { return m_fsvDriveKind; }
#endif
private:
	void Init();

private:
	//이름
	wxString m_strName;
	//전체경로
	wxString m_strPath;
	//드라이브명
	wxString m_strDriveName;
	//화면표시
	wxString m_strDisp;
	//타입설명
	wxString m_strTypeName;
	//드라이브 용량정보
	wxString m_strDriveSpace;
	//폴더플래그
	bool m_isDir;
	//파일플래그
	bool m_isFile;
	//드라이브플래그
	bool m_isDrive;
	//선택 플래그
	bool m_bSelected;
	//match flag
	bool m_bMatched;
	//잘라내기 속성
	bool m_isCut;
	//파일명 + ... 표시 플래그
	bool m_isShortDisp;
	//속성
	unsigned long m_ulattr;
	//아이콘 인덱스
	int m_iIcon;
	//드라이브 문자바이트수
	int m_iByteCount;
	//사이즈
	wxLongLong m_llSize;
	//시간
	wxDateTime m_dt;
	//아이템종류
	item_type m_iType;
	//아이콘
	wxIcon m_Icon;
	//2019.04.24 ADD
	wxString m_strSizeType;
	wxString m_strSize;
	wxColour m_colorType;
	//2019.08.07 ADD
	wxString m_strExt;
	wxString m_strVolume = wxT("");

#ifdef __WXMSW__
	wxFSVolumeKind m_fsvDriveKind = wxFS_VOL_MAX;
#endif
};
#endif
