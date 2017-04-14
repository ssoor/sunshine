#include "ntdll.h"
#include "..\..\common\defer.h"

#define NAME_OCX	L"mswsock.dll"
#define NAME_WXCAM_DLL	L"\\wxcam.dll"

NTSTATUS NTAPI fake::LdrLoadDll(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle) {
	CNktHookLib::HOOK_INFO* pHookInfo = hook::GetInfo(ntdll::eModule, ntdll::func::LdrLoadDll);

	if (ModuleFileName && ModuleFileName->Buffer && ModuleFileName->Length) {
		size_t sizeFileNameLenght = ModuleFileName->Length / 2;
		LPWSTR pwszLibFileName = (LPWSTR)malloc((sizeFileNameLenght + 1) * sizeof(WCHAR));

		defer_free _(pwszLibFileName);
		pwszLibFileName[sizeFileNameLenght] = L'\0';
		memcpy(pwszLibFileName, ModuleFileName->Buffer, sizeFileNameLenght * sizeof(WCHAR));

		OutputDebugStringW(pwszLibFileName);
		OutputDebugStringW(&pwszLibFileName[sizeFileNameLenght - wcslen(NAME_OCX)]);
		OutputDebugStringW(&pwszLibFileName[sizeFileNameLenght - wcslen(NAME_WXCAM_DLL)]);

		if (0 == _wcsicmp(&pwszLibFileName[sizeFileNameLenght - wcslen(NAME_OCX)], NAME_OCX)) {
			return NULL;
		}

		if (0 == _wcsicmp(&pwszLibFileName[sizeFileNameLenght - wcslen(NAME_WXCAM_DLL)], NAME_WXCAM_DLL)) {
			return NULL;
		}
	}

	return __pfnLdrLoadDll(pHookInfo->lpCallOriginal)(PathToFile, Flags, ModuleFileName, ModuleHandle);
}