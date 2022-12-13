/******************************************************************************************
 *
 * CJsonFileIconInfo
 *   파일의 아이콘 정보를 미리 저장하기 위한 클래스
 *
*******************************************************************************************/
#include "common.h"
#include "JsonFileIconInfo.h"

#include <iostream>
#include <fstream>
#include <streambuf>

std::unique_ptr<CJsonFileIconInfo> CJsonFileIconInfo::m_pInstance(nullptr);

CJsonFileIconInfo::CJsonFileIconInfo()
{

}

CJsonFileIconInfo::~CJsonFileIconInfo()
{
	_jsonDoc.RemoveAllMembers();
}

CJsonFileIconInfo* CJsonFileIconInfo::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CJsonFileIconInfo());

	return m_pInstance.get();
}

bool CJsonFileIconInfo::Load()
{
	wxString strJson(theUtility->GetWorkDir() + SLASH + wxT("settings") + SLASH + wxT("fileiconinfo.json"));
	if (!wxFileName::FileExists(strJson))
		return false;

	bool bReturn = DoLoad(strJson);
	if(bReturn)
        Init();

    return bReturn;
}

void CJsonFileIconInfo::Init()
{
    const Value& FileExtIconInfo = _jsonDoc["FileExtIconInfo"];
	assert(FileExtIconInfo.IsArray());

    for (SizeType i = 0; i < FileExtIconInfo.Size(); i++)
    {
        wxString strExt = FileExtIconInfo[i]["extname"].GetString();
        wxString strExtSend = wxT(".") + strExt;

        int iIconIndex = 0;
        int iOverlayIndex = 0;

        theUtility->GetIconIndex(strExtSend, iIconIndex, iOverlayIndex, true);

        _stFileIconInfo stIconInfo;
        stIconInfo.m_iIconIndex = iIconIndex;
        stIconInfo.m_iOverayIndex = iOverlayIndex;

        std::unordered_map<wxString, _stFileIconInfo>::value_type val(strExt, stIconInfo);
        m_mapFileIconInfo.insert(val);
    }
}

bool CJsonFileIconInfo::GetFileIconInfo(const wxString& strExt, int& IconIndex, int& overlayIndex)
{
    wxString strTempExt(strExt);
    strTempExt = strTempExt.MakeLower();

    std::unordered_map<wxString, _stFileIconInfo>::const_iterator fIter = m_mapFileIconInfo.find(strTempExt);
    if(fIter == m_mapFileIconInfo.end())
        return false;

    _stFileIconInfo stIconInfo = fIter->second;
    IconIndex = stIconInfo.m_iIconIndex;
    overlayIndex = stIconInfo.m_iOverayIndex;

    return true;
}

