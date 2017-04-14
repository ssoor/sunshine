#include "engine_main.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <Psapi.h>

#include <nektra\Deviare-InProc\Include\NktHookLib.h>

#include "resource.h"
#include "..\common\defer.h"
#include "..\common\autostring.h"

#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"e:\\github.com\\nektra\\Deviare-InProc\\Libs\\2017\\NktHookLib_Debug.lib")


static bool SetFile(const char * pszFilePath, const void * pFileData, int nFilesize)
{
	FILE * pfHandle = NULL;
	char szSaveFilePath[MAX_PATH + 1] = { 0 };

	strcpy_s(szSaveFilePath, pszFilePath);
	ExpandEnvironmentStringsA(pszFilePath, szSaveFilePath, MAX_PATH);


	if (0 != fopen_s(&pfHandle, pszFilePath, "wb")) {
		return false;
	}

	if (1 != fwrite(pFileData, nFilesize, 1, pfHandle)) {
		return false;
	}

	fclose(pfHandle);

	return true;
}

static void * GetResourceContent(HINSTANCE hInstance, size_t sizeResourceID, const char * pszReosurceType, size_t * __out psizeResourceSize = NULL) {
	HRSRC hRsrc = ::FindResourceA(hInstance, MAKEINTRESOURCEA(sizeResourceID), pszReosurceType);
	if (NULL == hRsrc) {
		return NULL;
	}

	if (NULL == psizeResourceSize) {
		psizeResourceSize = &sizeResourceID;
	}

	*psizeResourceSize = ::SizeofResource(hInstance, hRsrc);

	HGLOBAL hGlobal = ::LoadResource(hInstance, hRsrc);
	if (NULL == hGlobal) {
		return NULL;
	}

	void * pResource = malloc(*psizeResourceSize);
	memcpy(pResource, LockResource(hGlobal), *psizeResourceSize);

	FreeResource(hGlobal);

	return pResource;
}

bool StartInstanceAndInject(LPCTSTR ptszExecFileName, LPCSTR pwszInejctDLLPath) {
	size_t nNeedSize = 0;
	TCHAR szExecuteFileName[MAX_PATH] = { 0 };
	if (!::ExpandEnvironmentStrings(ptszExecFileName, szExecuteFileName, MAX_PATH)) {
		return false;
	}
	bool bIsSuccess = false;
	STARTUPINFOW siStratupInfo = { 0 };
	PROCESS_INFORMATION piProcessInformation = { 0 };
	autostring astrInejctDLLPath(pwszInejctDLLPath);
	autostring astrExecuteFileName(szExecuteFileName);

	siStratupInfo.cb = sizeof(STARTUPINFO);

	if (ERROR_SUCCESS != NktHookLibHelpers::CreateProcessWithDllW(NULL, (LPWSTR)astrExecuteFileName.wptr(), NULL, NULL, FALSE, 0, NULL, NULL, &siStratupInfo, &piProcessInformation, astrInejctDLLPath.wptr())) {
		return false;
	}

	return true;
}

bool SetResourceToFile(HINSTANCE hInstance, size_t sizeResourceID, const char * pszReosurceType, LPTSTR pszSaveFileName) {
	size_t sizeResourceSize = 0;

	void* pBusinessDLL = GetResourceContent(hInstance, sizeResourceID, pszReosurceType, &sizeResourceSize);
	if (NULL == pBusinessDLL) {
		return false;
	}
	defer_free _(pBusinessDLL);

	if (!SetFile(pszSaveFileName, pBusinessDLL, sizeResourceSize)) {
		return false;
	}

	return true;
}


namespace Common {

	static bool PathRemoveFileName(__inout wchar_t * pszFilePath)
	{
		pszFilePath = wcsrchr(pszFilePath, L'\\');
		if (NULL == pszFilePath) {
			pszFilePath = wcsrchr(pszFilePath, L'/');

		}

		if (pszFilePath) {
			pszFilePath[0] = L'\0';
		}

		return NULL != pszFilePath;
	}

	static bool PathAddFileName(__inout LPTSTR pszFilePath, const LPTSTR pszFileName)
	{
		if (pszFilePath[_tcslen(pszFilePath) - 1] != _T('\\')) {
			_tcscat_s(pszFilePath, MAX_PATH, _T("\\"));
		}

		return 0 == _tcscat_s(pszFilePath, MAX_PATH, pszFileName);
	}

}

bool GetInstallPath4Module(LPTSTR pszInstanceFileName, size_t sizeInstanceFileNameSize, HINSTANCE hInstance, LPCTSTR pszAttachedFileName) {
	::GetModuleFileName(hInstance, pszInstanceFileName, sizeInstanceFileNameSize);

	LPTSTR pszFileNamePos = _tcsrchr(pszInstanceFileName, _T('\\'));
	if (pszFileNamePos) {
		pszFileNamePos[1] = _T('\0');
	}

	return 0 == _tcscat_s(pszInstanceFileName, sizeInstanceFileNameSize, pszAttachedFileName);
}

bool GetInstallPath4Reg(LPTSTR pszInstanceFileName,size_t sizeInstanceFileNameSize,__in HKEY hKey, __in LPCSTR lpSubKey, __in LPCSTR lpValueName,LPCTSTR pszAttachedFileName) {

	HKEY hKeySubKey = NULL;
	DWORD dwDataSize = sizeInstanceFileNameSize;
	if (ERROR_SUCCESS != RegOpenKeyEx(hKey, lpSubKey, 0, KEY_QUERY_VALUE, &hKeySubKey)) {
		return false;
	}

	if (ERROR_SUCCESS != RegQueryValueEx(hKeySubKey, lpValueName, 0, &dwDataSize, (LPBYTE)pszInstanceFileName, &dwDataSize)) {
		RegCloseKey(hKeySubKey);
		return false;
	}
	RegCloseKey(hKeySubKey);

	if (pszInstanceFileName[_tcslen(pszInstanceFileName) - 1] != _T('\\')) {
		_tcscat_s(pszInstanceFileName, sizeInstanceFileNameSize, _T("\\"));
	}

	return 0 == _tcscat_s(pszInstanceFileName, sizeInstanceFileNameSize, pszAttachedFileName);
}


bool GetLogicalPath(LPCTSTR lpszDosPath, LPTSTR pszLogicalPath, size_t szeLogicalPathSize)
{
	TCHAR szCurrentDrive[3] = _T(" :");
	TCHAR szDosDriveName[MAX_PATH] = { 0 };
	TCHAR szDriveStrings[MAX_PATH] = { 0 };

	if (lpszDosPath == NULL || !GetLogicalDriveStrings(_countof(szDriveStrings) - 1, szDriveStrings)) {
		return false;
	}

	LPTSTR pDriveString = szDriveStrings;

	do {
		szCurrentDrive[0] = pDriveString[0];

		if (QueryDosDevice(szCurrentDrive, szDosDriveName, _countof(szDosDriveName)) && 0 == _tcsnicmp(lpszDosPath, szDosDriveName, _tcslen(szDosDriveName))) {
			_tcscpy_s(szDriveStrings, MAX_PATH, szCurrentDrive);
			_tcscat_s(szDriveStrings, MAX_PATH, &lpszDosPath[_tcslen(szDosDriveName)]);

			_tcscpy_s(pszLogicalPath, szeLogicalPathSize, szDriveStrings);
			return true;
		}

		while (*pDriveString++) {} // Go to the next NULL character.
	} while (*pDriveString); // end of string

	return false;
}

bool TerminateProcessAndQueryProcessPath(LPCTSTR pszProcessName,LPTSTR pszProcessImageFileName) {
	bool bIsFind = false;
	DWORD dwNeeded = 0;
	DWORD dwProcesses[1024] = { 0 };

	if (!EnumProcesses(dwProcesses, sizeof(dwProcesses), &dwNeeded)) {
		return false;
	}

	for (size_t j = 0; j < (dwNeeded / sizeof(DWORD)); j++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, dwProcesses[j]);
		if (NULL == hProcess) {
			continue;
		}

		if (!GetProcessImageFileName(hProcess, pszProcessImageFileName, MAX_PATH) || !GetLogicalPath(pszProcessImageFileName, pszProcessImageFileName, MAX_PATH)) {
			continue;
		}

		LPCTSTR pszCurrentProcessName = _tcsrchr(pszProcessImageFileName, _T('\\'));
		if (pszCurrentProcessName && 0 == _tcsicmp(++pszCurrentProcessName, pszProcessName)) {
			bIsFind = true;
			::TerminateProcess(hProcess, -1);
		}
	}

	return bIsFind;
}

DWORD CALLBACK threadSunshinePubwin(void* pParameter) {
	defer_free _(pParameter);
	LPTSTR pszResourceFileName = (LPTSTR)pParameter;

	DWORD dwNeeded = 0;
	DWORD dwProcesses[1024] = { 0 };
	TCHAR szProcessImageFileName[MAX_PATH] = { 0 };

	STARTUPINFO siStratupInfo = { 0 };
	PROCESS_INFORMATION piProcessInformation = { 0 };

	for (int i = 0; i < 600; i++)
	{
		Sleep(100);
		if (TerminateProcessAndQueryProcessPath(_T("PubwinClient.exe"), szProcessImageFileName)) {
			siStratupInfo.cb = sizeof(STARTUPINFOW);
			StartInstanceAndInject(szProcessImageFileName, pszResourceFileName);
			break;
		}
	}

	return 0;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpszCmdLine, _In_ int nShowCmd) {
	void * pBusinessDLL = NULL;

	DWORD dwThreadID = 0;
	HANDLE hFindThread[10] = { 0 };
	TCHAR szExecuteFileName[MAX_PATH] = { 0 };
	TCHAR szResourceFileName[MAX_PATH] = { 0 };

	::GetTempPath(MAX_PATH, szResourceFileName);
	::GetTempFileName(szResourceFileName, "bin", 0, szResourceFileName);

	if (SetResourceToFile(hInstance, IDR_BIN_SUNSHINE_PUBWIN, "BIN", szResourceFileName)) {
		do
		{
			if (GetInstallPath4Reg(szExecuteFileName, MAX_PATH, HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Hintsoft\\PubwinClient"), _T("InstallDir"), _T("PubwinClient.exe")) && StartInstanceAndInject(szExecuteFileName, szResourceFileName)) {
				break;
			}

			if (GetInstallPath4Module(szExecuteFileName, MAX_PATH, hInstance, _T("PubwinClient.exe")) && StartInstanceAndInject(szExecuteFileName, szResourceFileName)) {
				break;
			}

			hFindThread[0] = CreateThread(NULL, 0, threadSunshinePubwin, _tcsdup(szResourceFileName), 0, &dwThreadID);
		} while (false);
	}

	for (size_t i = 0; i < (sizeof(hFindThread) / sizeof(hFindThread[0])); i++) {
		WaitForSingleObject(hFindThread[i], INFINITE);
		CloseHandle(hFindThread[0]);
	}

	return 0;
}