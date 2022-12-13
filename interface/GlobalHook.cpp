#include "../ginc.h"
#include "GlobalHook.h"
#include <tlhelp32.h>
#include "../system/ProcessInfo.h"

//#include "../Export.h"

#ifdef __WXMSW__
	#ifdef _UNICODE
		#define SHWTEXT(s) L##s
	#else
		#define SHWTEXT(s) #s
	#endif
#endif // __WXMSW__

CGlobalHook::CGlobalHook()
{
//	HOOK_Install();
}

CGlobalHook::~CGlobalHook()
{
//	HOOK_UnInstall();
}

#ifdef __WXMSW__
bool CGlobalHook::InjectDll(DWORD dwPID, const wxString& strDllPath)
{
	HANDLE                  hProcess, hThread;
	LPVOID                  pRemoteBuf;
	DWORD                   dwBufSize = strDllPath.Len() + 1;

	//Target 프로그램 핸들을 오픈한다.
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if(hProcess == nullptr)
		return false;

	//target 프로세스에 dll을 inject하기 위한 메모리를 할당한다.
	//메모리 = dllPath + null terminate(dllpath length + 1)
	pRemoteBuf = VirtualAllocEx(hProcess, 0, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	if(pRemoteBuf == nullptr)
		return false;
	//Write the path to the address of the memory we just allocated
	//in the target process
	LPCSTR lpDllPath = strDllPath.c_str();
	int nRetValue = WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)lpDllPath, dwBufSize, NULL);
	if(nRetValue == 0)
		return false;

	wxString strLoadLibrary(wxT(""));

#ifdef _UNICODE
	strLoadLibrary = wxT("LoadLibraryW");
#else
	strLoadLibrary = wxT("LoadLibraryA");
#endif
	//Create a Remote Thread in the target process which
	//calls LoadLibraryA as our dllpath as an argument -> program loads our dll
	LPTHREAD_START_ROUTINE lpThreadStartRoutine = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(SHWTEXT("kernel32.dll")), strLoadLibrary.c_str());
	if(lpThreadStartRoutine == nullptr)
		return false;

	hThread = CreateRemoteThread(hProcess, NULL, 0, lpThreadStartRoutine, pRemoteBuf, 0, NULL);
	if(hThread == nullptr)
		return false;

	//Wait for the execution our loader thread to finished
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return true;
}

bool CGlobalHook::EjectDll(DWORD dwPID, const wxString& strDllPath)
{
	bool                    bMore = false, bFound = false;
	HANDLE                  hSnapshot, hProcess, hThread;
	MODULEENTRY32           me = { sizeof(MODULEENTRY32) };
	LPTHREAD_START_ROUTINE  pThreadProc;

	if( INVALID_HANDLE_VALUE == (hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID)) )
		return false;

	bMore = Module32First(hSnapshot, &me);
	for( ; bMore ; bMore = Module32Next(hSnapshot, &me) )
	{
		wxString strModule(me.szModule);
		wxString strExePath(me.szExePath);

		if( !strModule.CmpNoCase(strDllPath) ||
            !strExePath.CmpNoCase(strDllPath) )
		{
			bFound = true;
			break;
		}
	}

	if( !bFound )
	{
		CloseHandle(hSnapshot);
		return false;
	}

	if( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
	{
		wxString strMsg = wxString::Format(wxT("OpenProcess(%u) failed!!!"), dwPID);
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);

		CloseHandle(hSnapshot);
		return false;
	}

	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(SHWTEXT("kernel32.dll")), "FreeLibrary");
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnapshot);

	return true;
}

int Compare( const void * Val1, const void * Val2 )
{
    if ( *(PDWORD)Val1 == *(PDWORD)Val2 )
    return 0;

    return *(PDWORD)Val1 > *(PDWORD)Val2 ? 1 : -1;
}

bool CGlobalHook::IsReadProcessMemory(DWORD dwPID)
{
	return true;
}

#else
#endif
