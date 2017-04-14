#include <Windows.h>

#include "kernel32.h"


HANDLE WINAPI fake::CreateFileA(__in LPCSTR lpFileName, __in DWORD dwDesiredAccess, __in DWORD dwShareMode, __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes, __in DWORD dwCreationDisposition, __in DWORD dwFlagsAndAttributes, __in_opt HANDLE hTemplateFile) {
	return __pfnCreateFileA(hook::GetInfo(hook::Kernel32_dll,kernel32::func::CreateFileA)->lpCallOriginal)(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
