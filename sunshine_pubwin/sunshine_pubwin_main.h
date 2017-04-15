#pragma once
#include <stdio.h>
#include <tchar.h>

#include <github.com\nektra\Deviare-InProc\Include\NktHookLib.h>

namespace global {
	extern HINSTANCE hCurrentModule;
}


inline bool IsCurrentProcess(const TCHAR * pszProcessName)
{
	bool bRet = false;
	TCHAR szProcessPath[MAX_PATH + 1] = { 0 };

	if (NULL == pszProcessName) {
		return false;
	}

	if (::GetModuleFileName(NULL, szProcessPath, MAX_PATH)) {
		const TCHAR * pszCurrentProcessName = _tcsrchr(szProcessPath, TEXT('\\'));

		if (NULL == pszCurrentProcessName)
			bRet = !_tcsicmp(szProcessPath, pszProcessName);
		else
			bRet = !_tcsicmp(++pszCurrentProcessName, pszProcessName);
	}

	return bRet;
}

inline HINSTANCE GetModuleHandleByAddr(const void * pAddr)
{
	MEMORY_BASIC_INFORMATION miMemoryInfo = { 0 };

	VirtualQuery(pAddr, &miMemoryInfo, sizeof(MEMORY_BASIC_INFORMATION));

	return (HMODULE)miMemoryInfo.AllocationBase;
}

inline void LockModuleForHandle(HINSTANCE hModule)
{
	TCHAR szModuleName[MAX_PATH + 1] = { 0 };

	::GetModuleFileName(hModule, szModuleName, MAX_PATH);
	::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN, szModuleName, &hModule);
}

inline bool SetFile(const char * pszFilePath, const void * pFileData, size_t nFilesize)
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


inline void * GetResourceContent(HINSTANCE hInstance, size_t sizeResourceID, const char * pszReosurceType, size_t * __out psizeResourceSize = NULL) {
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