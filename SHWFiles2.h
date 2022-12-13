#ifndef SHWFILES2_H_INCLUDED
#define SHWFILES2_H_INCLUDED

/***************************************************************
 * Name:      SHWFiles2.h
 * Purpose:   Defines Application Class
 * Author:    BaeByoungYong (bbygoodluck@gmail.com)
 * Created:   2022-04-19
 * Copyright: BaeByoungYong (https://github.com/bbygoodluck)
 * License:
 **************************************************************/

class CSHWFiles2 : public wxApp
{
public:
	explicit CSHWFiles2();
	virtual ~CSHWFiles2();

	bool OnInit() override;
	int OnExit() override;

#ifdef NDEBUG
	wxSingleInstanceChecker* m_pSingleInstance;
#endif

private:
    wxString m_strTitle = wxT("");
    bool m_bAddFontRC = false;

private:
	bool Initialize();
	bool InitConfig();
};

#endif // SHWFILES2_H_INCLUDED
