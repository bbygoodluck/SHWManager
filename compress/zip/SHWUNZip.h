#ifndef SHWUNZIP_H_INCLUDED
#define SHWUNZIP_H_INCLUDED

class CSHWCompressReleaseImpl;
class CSHWUNZip : public CSHWCompressReleaseImpl
{
	struct UZ_FileInfo
	{
		char szFileName[MAX_PATH + 1];
		char szComment[MAX_COMMENT + 1];

		unsigned long ulVersion;
		unsigned long ulVersionNeeded;
		unsigned long ulFlags;
		unsigned long ulCompressionMethod;
		unsigned long ulDosDate;
		unsigned long ulCRC;
		unsigned long ulCompressedSize;
		unsigned long ulUncompressedSize;
		unsigned long ulInternalAttrib;
		unsigned long ulExternalAttrib;
		bool bFolder;
	};

public:
	CSHWUNZip();
	virtual ~CSHWUNZip();

public:
	void Run() override;

protected:
	wxThread::ExitCode Entry() override;
	void DoStart() override;

	bool CloseUnZip();
	bool GetExtractFileInfo(UZ_FileInfo& info);
	int  GetUnzipFileCount();

	bool OpenUnZip();
	bool DoExtractZip();
	bool DoExtractFileFromZip(const wxString& strDir);
	bool GotoFirstFile();
	bool GotoNextFile();

private:
	void* m_uzFile = nullptr;
};

#endif // SHWUNZIP_H_INCLUDED
