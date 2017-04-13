#pragma once
#include "hookinfo.h"
#include <Winternl.h>

namespace fake {
	namespace ntdll {
		const hook::Module eModule = hook::Ntdll_dll;
		namespace func {
			enum Function {
				LdrLoadDll,
				NtCreateSection,

				MaxFunctionNumber
			};
		}

		inline CNktHookLib::HOOK_INFO* SetHookInfo(func::Function enumFunction, void* lpProcToHook, void* lpNewProcAddr) {
			if (func::MaxFunctionNumber != hook::GetCount(eModule)) {
				hook::CreateHookInfo(eModule, func::MaxFunctionNumber);
			}

			CNktHookLib::HOOK_INFO* pHookInfo = hook::GetInfo(eModule, enumFunction);

			if (pHookInfo) {
				pHookInfo->nHookId = 0;
				pHookInfo->lpCallOriginal = NULL;

				pHookInfo->lpProcToHook = lpProcToHook;
				pHookInfo->lpNewProcAddr = lpNewProcAddr;
			}

			return pHookInfo;
		}
	}

	NTSTATUS NTAPI LdrLoadDll(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle);
	typedef NTSTATUS (NTAPI* __pfnLdrLoadDll)(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle);

	typedef struct _OBJECT_ATTRIBUTES {
		ULONG           Length;
		HANDLE          RootDirectory;
		PUNICODE_STRING ObjectName;
		ULONG           Attributes;
		PVOID           SecurityDescriptor;
		PVOID           SecurityQualityOfService;
	}  OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

	NTSTATUS NTAPI NtCreateSection(__out PHANDLE SectionHandle, __in ACCESS_MASK DesiredAccess, __in_opt POBJECT_ATTRIBUTES ObjectAttributes, __in_opt PLARGE_INTEGER MaximumSize, __in ULONG SectionPageProtection, __in ULONG AllocationAttributes, __in_opt HANDLE FileHandle);
	typedef NTSTATUS (NTAPI* __pfnNtCreateSection)(__out PHANDLE SectionHandle, __in ACCESS_MASK DesiredAccess, __in_opt POBJECT_ATTRIBUTES ObjectAttributes, __in_opt PLARGE_INTEGER MaximumSize, __in ULONG SectionPageProtection, __in ULONG AllocationAttributes, __in_opt HANDLE FileHandle);

}