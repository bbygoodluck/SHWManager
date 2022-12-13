#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

class CUtility final
{
private:
	explicit CUtility() {};

public:
	~CUtility() {};

	static CUtility* Get();

#ifdef __WXMSW__
	int GetWindowsVersion();
#endif
    //색상정보가져오기
	wxColour GetColor(unsigned long &ulColor)
	{
		unsigned long r = GetRedColor(ulColor);
		unsigned long g = GetGreenColor(ulColor);
		unsigned long b = GetBlueColor(ulColor);

		return wxColour(r, g, b);
	}

	//RGB colour 값 구하기
	const unsigned long GetRedColor(unsigned long ulColor)   { return ((ulColor & 0x00FF0000) >> 16); }
	const unsigned long GetGreenColor(unsigned long ulColor) { return ((ulColor & 0x0000FF00) >> 8); }
	const unsigned long GetBlueColor(unsigned long ulColor)  { return (ulColor & 0x000000FF); }

    wxString GetSystemLang();           //platform 언어가져오기
    void LoadImageList();               //이미지리스트 로드
    void GetIconIndex(const wxString& strPath, int &nIconIndex, int &nOverlayIndex, bool bExtFind = false);  //아이콘 인덱스

    const wxString& GetWorkDir() const  //실행디렉토리 가져오기
    {
        return m_strWorkingDirectory;
    }

	//프로그램 실행
	void LaunchAndExec(const wxString& strExecProgram, const wxString& strPath);

    //패스이름 가져오기
    wxString GetPathName(const wxString& strFullPath);
    //부모 패스정보 가져오기
    wxString GetParentPath(const wxString& strFullPath);

    //복사할때 사용
	wxString ChangeName(const wxString& strName);
	//파일명 가져오기
	wxString GetFileName(const wxString& strFullPath, bool IsAppendExt = true);

	//파일사이즈 정보가져오기
	void GetSizeInfo(const double dblFileSize, wxString& strFileSize, wxString& strFileSizeType, wxColour& dispColor);
	//확장자 가져오기
	wxString GetExt(const wxString& strFileName);
	//패스깊이
	int GetDepth(const wxString& strFullPath, const wxString& strSeperator = SLASH);
    //화면 다시그리기
	void RefreshWindow(wxWindow* pWindow, const wxRect& rect = wxRect(0, 0, 0, 0), bool bUpdate = false);
	bool IsAllowAttr(const unsigned long _lattr);
	//비교하기
	int Compare(const wxString& strOld, const wxString& strNew);
	//최대값 가져오기
	wxString GetMaxData(wxWindow* pWindow, const wxFont& _font, const wxString& strSrc, const wxString& strTgt);
	wxString GetMaxData(wxWindow* pWindow, const wxFont& _font, const wxVector<wxString>& vecData);
	//사이즈에 콤마찍기
	wxString SetComma(const wxString& strSize);
	//키이름 가져오기
	wxString GetKeyName(const wxKeyEvent& event);
	const char* GetVirtualKeyCodeName(int nKeyCode);

	wxString MakeFullPathName(const wxString& strCurrentPath, const wxString& strNewPathName);
	bool CheckCreatable(const wxString& strFilePathName);

#ifdef __WXMSW__
	//IME 설정
	void SetImeModeToEnglish(wxWindow* pWindow);
#endif // __WXMSW__

#ifndef NDEBUG
	void LogDisplay(const wxString& strMsg);
#endif

	//Volume
	wxString GetVolume(const wxString& strFullPath);
	//자르기
	wxVector<wxString> Split(const wxString& strSource, const wxString& strDelimiter);
	//메세지 표시
	void ShowErrorMessage(const wxString& strItem, int iRetCode);
private:
	void SetWorkingDirectory();
	void Init();

private:
	static std::unique_ptr<CUtility> m_pInstance;
	wxString m_strWorkingDirectory = wxT("");

	unsigned long larrAttr[3] = {0, };
};


#endif // UTILITY_H_INCLUDED
