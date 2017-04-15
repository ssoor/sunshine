#include "ntdll.h"
#include "..\..\common\autostring.h"

#pragma comment(lib,"github.com\\ssoor\\sunshine\\common\\ntdll\\ntdll.lib")

//
// The success status codes 0 - 63 are reserved for wait completion status.
// FacilityCodes 0x5 - 0xF have been allocated by various drivers.
//
#define STATUS_SUCCESS                          ((NTSTATUS)0x00000000L) // ntsubauth

namespace fake {
	bool GetLogicalPath(LPCWSTR lpszDosPath, LPWSTR pszLogicalPath, size_t szeLogicalPathSize)
	{
		WCHAR szCurrentDrive[3] = L" :";
		WCHAR szDosDriveName[MAX_PATH] = { 0 };
		WCHAR szDriveStrings[MAX_PATH] = { 0 };

		if (lpszDosPath == NULL || !GetLogicalDriveStringsW(_countof(szDriveStrings) - 1, szDriveStrings)) {
			return false;
		}

		LPWSTR pDriveString = szDriveStrings;

		do {
			szCurrentDrive[0] = pDriveString[0];

			if (QueryDosDeviceW(szCurrentDrive, szDosDriveName, _countof(szDosDriveName)) && 0 == _wcsnicmp(lpszDosPath, szDosDriveName, wcslen(szDosDriveName))) {
				wcscpy_s(szDriveStrings, MAX_PATH, szCurrentDrive);
				wcscat_s(szDriveStrings, MAX_PATH, &lpszDosPath[wcslen(szDosDriveName)]);

				wcscpy_s(pszLogicalPath, szeLogicalPathSize, szDriveStrings);
				return true;
			}

			while (*pDriveString++) {} // Go to the next NULL character.
		} while (*pDriveString); // end of string

		return false;
	}

	NTSTATUS GetFullPathByHandle(IN HANDLE ObjectHandle, OUT WCHAR* strFullPath, size_t sizeFullPathSize) {
		typedef struct _OBJECT_NAME_INFORMATION {
			UNICODE_STRING Name;
		} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

		ULONG uResultLength = 0;
		NTSTATUS ntStatus = STATUS_SUCCESS;
		POBJECT_NAME_INFORMATION pNameInfo = NULL;

		enum {
			ObjectBasicInformation,
			ObjectNameInformation,
			ObjectTypeInformation,
			ObjectAllTypesInformation,
			ObjectHandleInformation
		};
		NtQueryObject(ObjectHandle, (OBJECT_INFORMATION_CLASS)ObjectNameInformation, NULL, 0, &uResultLength);
		if (0 == uResultLength) {
			return false;
		}

		pNameInfo = (POBJECT_NAME_INFORMATION)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, uResultLength);
		ntStatus = NtQueryObject(ObjectHandle, (OBJECT_INFORMATION_CLASS)ObjectNameInformation, pNameInfo, uResultLength, &uResultLength);

		if (NT_SUCCESS(ntStatus)) {
			wcscpy_s(strFullPath, sizeFullPathSize, pNameInfo->Name.Buffer);
		}

		HeapFree(GetProcessHeap(), 0, pNameInfo);

		return ntStatus;
	}

	BOOL GetPath(IN POBJECT_ATTRIBUTES ObjectAttributes, OUT WCHAR* strFullPath, size_t sizeFullPathSize)
	{
		if (NULL == ObjectAttributes || (NULL == ObjectAttributes->RootDirectory && NULL == ObjectAttributes->ObjectName)) {
			return FALSE;
		}

		if (NULL != ObjectAttributes->RootDirectory && STATUS_SUCCESS != GetFullPathByHandle(ObjectAttributes->RootDirectory, strFullPath, sizeFullPathSize)) {
			return FALSE;
		}

		if (NULL != ObjectAttributes->ObjectName && ObjectAttributes->ObjectName->Length > 0) {
			wcscat_s(strFullPath, sizeFullPathSize, L"\\");
			wcscat_s(strFullPath, sizeFullPathSize, ObjectAttributes->ObjectName->Buffer);
		}

		return TRUE;
	}
}

NTSTATUS NTAPI fake::NtCreateSection(__out PHANDLE SectionHandle, __in ACCESS_MASK DesiredAccess, __in_opt POBJECT_ATTRIBUTES ObjectAttributes, __in_opt PLARGE_INTEGER MaximumSize, __in ULONG SectionPageProtection, __in ULONG AllocationAttributes, __in_opt HANDLE FileHandle) {
	WCHAR szSectionFileName[MAX_PATH] = { 0 };
	CNktHookLib::HOOK_INFO* pHookInfo = hook::GetInfo(ntdll::eModule, ntdll::func::NtCreateSection);

	if (AllocationAttributes != SEC_IMAGE || 0 == (SectionPageProtection & PAGE_EXECUTE)) {
		return __pfnNtCreateSection(pHookInfo->lpCallOriginal)(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
	}

	if (NULL == ObjectAttributes && NULL == FileHandle) {
		return __pfnNtCreateSection(pHookInfo->lpCallOriginal)(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
	}

	if (!GetPath(ObjectAttributes, szSectionFileName, MAX_PATH) && !NT_SUCCESS(GetFullPathByHandle(FileHandle, szSectionFileName, MAX_PATH))) {
		return __pfnNtCreateSection(pHookInfo->lpCallOriginal)(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
	}

	if (!GetLogicalPath(szSectionFileName, szSectionFileName, MAX_PATH)) {
		return __pfnNtCreateSection(pHookInfo->lpCallOriginal)(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
	}

	int nWatchOvectors[PCRE_MAX_OVECCOUNT] = { 0 };
	autostring astrSectionFileName(szSectionFileName);

	for (size_t i = 0; i < rule::rHookRule[rule::Library].sizeCount; i++)
	{
		if (pcre_exec(rule::rHookRule[rule::Library].pcreCompile[i], NULL, astrSectionFileName.ptr(), astrSectionFileName.len(), 0, 0, nWatchOvectors, PCRE_MAX_OVECCOUNT) >= 0) {
			return  STATUS_NO_MEMORY;
		}
	}

	return __pfnNtCreateSection(pHookInfo->lpCallOriginal)(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
}