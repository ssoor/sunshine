#include "kernel32.h"


inline HINSTANCE GetModuleHandleByAddr(const void * pAddr)
{
	MEMORY_BASIC_INFORMATION miMemoryInfo = { 0 };

	VirtualQuery(pAddr, &miMemoryInfo, sizeof(MEMORY_BASIC_INFORMATION));

	return (HMODULE)miMemoryInfo.AllocationBase;
}

BOOL WINAPI fake::CreateProcessInternalW(HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES	lpProcessAttributes, LPSECURITY_ATTRIBUTES	lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken) {
	WCHAR szModuleFileName[MAX_PATH] = { 0 };
	GetModuleFileNameW(GetModuleHandleByAddr(GetModuleHandleByAddr), szModuleFileName, MAX_PATH); 
	
	BOOL bIsSucc = __pfnCreateProcessInternalW(hook::GetInfo(hook::Kernel32_dll, kernel32::func::CreateProcessInternalW)->lpCallOriginal)(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);

	if (bIsSucc) {
		NktHookLibHelpers::InjectDllByHandleW(lpProcessInformation->hProcess, szModuleFileName);
	}

	return bIsSucc;
}