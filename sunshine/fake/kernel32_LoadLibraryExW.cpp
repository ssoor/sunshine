#include "kernel32.h"

#define NAME_OCX	L".ocx"
#define NAME_WXCAM_DLL	L"\\wxcam.dll"

HMODULE WINAPI fake::LoadLibraryExW(__in LPCWSTR lpLibFileName, __reserved HANDLE hFile, __in DWORD dwFlags) {
	size_t sizeFileNameLenght = wcslen(lpLibFileName);

	if (0 == _wcsicmp(&lpLibFileName[sizeFileNameLenght - wcslen(NAME_OCX)], NAME_OCX)) {
		return NULL;
	}

	if (0 == _wcsicmp(&lpLibFileName[sizeFileNameLenght - wcslen(NAME_WXCAM_DLL)], NAME_WXCAM_DLL)) {
		return NULL;
	}
	
	return __pfnLoadLibraryExW(hook::GetInfo(hook::Kernel32_dll, kernel32::func::LoadLibraryExW)->lpCallOriginal)(lpLibFileName, hFile, dwFlags);
}