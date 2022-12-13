#ifndef SHWZIP_H_INCLUDED
#define SHWZIP_H_INCLUDED

class CSHWCompressReleaseImpl;
class CSHWZip : public CSHWCompressReleaseImpl
{
	struct Z_FileInfo
	{
		int nFileCount;
		int nFolderCount;
		unsigned long ulUncompressedSize;
	};

public:
	CSHWZip();
	virtual ~CSHWZip();

public:
	void Run() override;

	bool CloseZip();

	bool AddFileToZip(const wxString& strPathName, const wxString& strAddName = wxT(""));
	bool AddFolderToZip(const wxString& strPathName, const wxString& strAddName = wxT(""));

	bool OpenZip(bool bAppend = false);
	void GetFileInfo(Z_FileInfo& info);

protected:
	wxThread::ExitCode Entry() override;
	void DoStart() override;

private:
	void* m_uzFile = nullptr;
	Z_FileInfo m_info;
};

#endif // SHWZIP_H_INCLUDED
