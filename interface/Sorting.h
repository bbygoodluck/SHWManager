#ifndef __SORTING_H__
#define __SORTING_H__

#include "DirData.h"
class CSorting
{
public:
	CSorting(int iSortType);
	~CSorting();

public:
	static bool DirSortOfName(const CDirData& lhs, const CDirData& rhs);
	static bool DirSortOfSize(const CDirData& lhs, const CDirData& rhs);
	static bool DirSortOfTime(const CDirData& lhs, const CDirData& rhs);

private:
	static int CmpDir(bool blhsDir, bool brhsDir);
	static int CmpName(const wxString& strLeft, const wxString& strRight);


private:
	int m_iSortType;
};
#endif
