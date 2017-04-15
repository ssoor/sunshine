#include "kernel32.h"


inline HINSTANCE GetModuleHandleByAddr(const void * pAddr)
{
	MEMORY_BASIC_INFORMATION miMemoryInfo = { 0 };

	VirtualQuery(pAddr, &miMemoryInfo, sizeof(MEMORY_BASIC_INFORMATION));

	return (HMODULE)miMemoryInfo.AllocationBase;
}
BOOL WINAPI fake::CreateProcessW(_In_opt_ LPCWSTR lpApplicationName, _Inout_opt_ LPWSTR lpCommandLine, _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes, _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes, _In_ BOOL bInheritHandles, _In_ DWORD dwCreationFlags, _In_opt_ LPVOID lpEnvironment, _In_opt_ LPCWSTR lpCurrentDirectory, _In_ LPSTARTUPINFOW lpStartupInfo, _Out_ LPPROCESS_INFORMATION lpProcessInformation) {
	WCHAR szModuleFileName[MAX_PATH] = { 0 };
	GetModuleFileNameW(GetModuleHandleByAddr(GetModuleHandleByAddr), szModuleFileName, MAX_PATH); 
	
	BOOL bIsSucc = __pfnCreateProcessW(hook::GetInfo(hook::Kernel32_dll, kernel32::func::CreateProcessW)->lpCallOriginal)(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

	if (bIsSucc) {
		NktHookLibHelpers::InjectDllByHandleW(lpProcessInformation->hProcess, szModuleFileName);
	}

	return bIsSucc;
}