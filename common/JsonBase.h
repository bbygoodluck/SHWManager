#ifndef __JSON_BASE_H__
#define __JSON_BASE_H__

#include "../rapidjson/document.h"
using namespace rapidjson;

#include "../rapidjson/istreamwrapper.h"
#include "../rapidjson/ostreamwrapper.h"
#include "../rapidjson/prettywriter.h"

class CJsonBase
{
public:
	CJsonBase() {}
	~CJsonBase() {}

	Document& GetJsonDoc() { return _jsonDoc; };
	virtual bool Load() = 0;
	bool HasMember(const wxString& _strMember, const wxString& _strKey = wxT(""))
	{
		if(_strKey.IsEmpty())
			return _jsonDoc.HasMember(_strMember.char_str());

		Value _Key(_strKey.c_str(), _jsonDoc.GetAllocator());
		return _jsonDoc[_Key].HasMember(_strMember.char_str());
	}

protected:
	bool DoLoad(const wxString& strPath);

protected:
	Document _jsonDoc;
	wxString m_strJsonPath;
};
#endif
