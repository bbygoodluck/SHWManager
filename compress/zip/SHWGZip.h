#ifndef SHWGZIP_H_INCLUDED
#define SHWGZIP_H_INCLUDED

#include "./include/zlib.h"
class CSHWCompressReleaseImpl;
class CSHWGZip : public CSHWCompressReleaseImpl
{
public:
	CSHWGZip();
	virtual ~CSHWGZip();

public:
	void Run() override;

protected:
	wxThread::ExitCode Entry() override;
	void DoStart() override;

private:
	FILE* m_file;
	gzFile m_gzOut;

	wxString m_strSource = wxT("");
};
#endif // SHWGZIP_H_INCLUDED
