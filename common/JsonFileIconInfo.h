#ifndef JSONIMAGEMAP_H_INCLUDED
#define JSONIMAGEMAP_H_INCLUDED

#include "JsonBase.h"
class CJsonFileIconInfo : public CJsonBase
{
    struct _stFileIconInfo{
        int m_iIconIndex;
        int m_iOverayIndex;
    };

private:
	CJsonFileIconInfo();
    void Init();

public:
	~CJsonFileIconInfo();
	static CJsonFileIconInfo* Get();

	virtual bool Load();
	bool GetFileIconInfo(const wxString& strExt, int& IconIndex, int& overlayIndex);

private:
    std::unordered_map<wxString, _stFileIconInfo> m_mapFileIconInfo;

private:
	static std::unique_ptr<CJsonFileIconInfo> m_pInstance;
};

#endif // JSONIMAGEMAP_H_INCLUDED
