#ifndef COMPRESS_H_INCLUDED
#define COMPRESS_H_INCLUDED

class CSHWCompressReleaseImpl;
class CLock;
class CCompressRelease
{
private:
	CCompressRelease() {}

public:
	~CCompressRelease() {}
	static CCompressRelease* Get();

public:
	bool SetCompressInfo(const wxString& strFullPath, const wxString& strCompressedFile, COMPRESS_TYPE compressType);
	bool SetCompressInfo(const std::vector<wxString>& vecDatas, const wxString& strCompressedFile, COMPRESS_TYPE compressType);
	bool SetDeCompressedInfo(const wxString& strCompressedFile, const wxString& strDeCompressDir, COMPRESS_TYPE compressType);

	void SetDeCompressType(COMPRESS_TYPE _compType) { m_DeCompressType = _compType; }
	COMPRESS_TYPE GetDeCompressType() { return m_DeCompressType; }
	COMPRESS_TYPE GetCompressType() { return m_CompressType; }
	COMPRESS_TYPE GetCompressReleaseType(const wxString& strExt, bool bCompress = true);

	void DoCancel() { m_bCancel = true; }
	bool IsCancel() { return m_bCancel; }
	bool IsExtract() { return m_bDeCompress; }

	void SetAllDeCompressSame(bool bAllSame) { m_bAllDeCompressSame = bAllSame; }
	bool IsAllDeCompressSame() { return m_bAllDeCompressSame; }

	void SetOwnerDialog(wxDialog* pDialog);
	void Start();

	std::vector<wxString> GetCompressList() { return m_vCompressReleaseExt; }
	std::vector<wxString> GetCompressingDatas() { return m_vecCompressingDatas; }
	wxString GetCompressedFile() { return m_strCompressedFile; }
	wxString GetDeCompressDir() { return m_strDeCompressDir; }

	bool IsCompressedFile(const wxString& strExt);

	void ClearCompressInfo();
	void SetLock();
	void SetRelease();
private:
	void Init();

private:
	std::vector<wxString> m_vCompressReleaseExt;
	std::vector<wxString> m_vecCompressingDatas;

	wxString m_strCompressedFile = wxT("");
	wxString m_strDeCompressDir = wxT("");

	COMPRESS_TYPE m_CompressType   = COMPTYPE_NONE;
	COMPRESS_TYPE m_DeCompressType = COMPTYPE_NONE;

	static std::unique_ptr<CCompressRelease> m_pInstance;
	wxDialog* m_pCompressReleaseDlg = nullptr;

	bool m_bDeCompress = false;
	bool m_bCancel = false;
	bool m_bAllDeCompressSame = false;

	CSHWCompressReleaseImpl* m_pCompressReleaseImpl = nullptr;
	CLock lock;
};

#endif // COMPRESS_H_INCLUDED
