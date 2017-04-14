#include <Windows.h>
#include <WinTrust.h>

#include "wintrust.h"

LONG WINAPI fake::WinVerifyTrust(_In_ HWND   hWnd, _In_ GUID   *pgActionID, _In_ LPVOID pWVTData) {
	WINTRUST_DATA* pWinTrustData = (WINTRUST_DATA*)pWVTData;
	CNktHookLib::HOOK_INFO* pHookInfo = hook::GetInfo(hook::Wintrust_dll, wintrust::func::WinVerifyTrust);

	if (pWinTrustData->cbStruct == sizeof(WINTRUST_DATA) && pWinTrustData->pFile && pWinTrustData->pFile->pcwszFilePath) {
		LPCWSTR pwszFileName = wcsrchr(pWinTrustData->pFile->pcwszFilePath, L'\\');

		if (pwszFileName && 0 == _wcsicmp(++pwszFileName, L"wxcam.dll")) {
			WCHAR wszVerifyFileName[MAX_PATH] = { 0 };

			GetModuleFileNameW(NULL, wszVerifyFileName, MAX_PATH);
			pWinTrustData->pFile->pcwszFilePath = wszVerifyFileName;
		}
	}


	return __pfnWinVerifyTrust(pHookInfo->lpCallOriginal)(hWnd, pgActionID, pWVTData);
}
