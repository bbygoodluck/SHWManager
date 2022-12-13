#ifndef EXPORT_H_INCLUDED
#define EXPORT_H_INCLUDED

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
	DLL_EXPORT void HOOK_Install();
	DLL_EXPORT void HOOK_UnInstall();
#endif

#ifdef __cplusplus
}
#endif

#endif // EXPORT_H_INCLUDED
