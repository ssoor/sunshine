#include "kernel32.h"

#define NAME_OCX	L".ocx"
#define NAME_WXCAM_DLL	L"\\wxcam.dll"

HMODULE WINAPI fake::LoadLibraryW(__in LPCWSTR lpLibFileName) {
	size_t sizeFileNameLenght = wcslen(lpLibFileName);

	if (0 == _wcsicmp(&lpLibFileName[sizeFileNameLenght - wcslen(NAME_OCX)], NAME_OCX)) {
		return NULL;
	}

	if (0 == _wcsicmp(&lpLibFileName[sizeFileNameLenght - wcslen(NAME_WXCAM_DLL)], NAME_WXCAM_DLL)) {
		return NULL;
	}
	
	return __pfnLoadLibraryW(hook::GetInfo(hook::Kernel32_dll, kernel32::func::LoadLibraryW)->lpCallOriginal)(lpLibFileName);
}