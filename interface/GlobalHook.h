#ifndef GLOBALHOOK_H_INCLUDED
#define GLOBALHOOK_H_INCLUDED
class CGlobalHook
{
protected:

public:
	CGlobalHook();
	~CGlobalHook();

public:
#ifdef __WXMSW__
	bool InjectDll(DWORD dwPID, const wxString& strDllPath);
	bool EjectDll(DWORD dwPID, const wxString& strDllPath);
#else
#endif // __WXMSW__

private:
#ifdef __WXMSW__
	bool IsReadProcessMemory(DWORD dwPID);
#else
#endif
private:
	wxWindow* m_pWindow = nullptr;
};

#endif // GLOBALHOOK_H_INCLUDED
