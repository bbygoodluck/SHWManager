#ifndef SHWUNGZIP_H_INCLUDED
#define SHWUNGZIP_H_INCLUDED

#include "./include/zlib.h"
class CSHWCompressReleaseImpl;
class CSHWUNGZip : public CSHWCompressReleaseImpl
{
public:
	CSHWUNGZip();
	virtual ~CSHWUNGZip();

public:
	void Run() override;

protected:
	wxThread::ExitCode Entry() override;
	void DoStart() override;


private:
	gzFile m_gzIn;
	FILE* m_OutFile;
};

#endif // SHWUNGZIP_H_INCLUDED
