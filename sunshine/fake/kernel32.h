#pragma once

#include <nektra\Deviare-InProc\Include\NktHookLib.h>

namespace fake {
	namespace kernel32 {
		namespace hook {
			enum Function {
				CreateFileA,

				MaxFunctionNumber
			};

			static CNktHookLib::HOOK_INFO hiHooks[MaxFunctionNumber] = { 0 };

			inline CNktHookLib::HOOK_INFO* GetInfo(hook::Function enumFunction) {
				return &hook::hiHooks[enumFunction];
			}
		}

		inline CNktHookLib::HOOK_INFO* SetHookInfo(hook::Function enumFunction, void* lpProcToHook, void* lpNewProcAddr) {
			hook::hiHooks[enumFunction].nHookId = 0;
			hook::hiHooks[enumFunction].lpCallOriginal = NULL;

			hook::hiHooks[enumFunction].lpProcToHook = lpProcToHook;
			hook::hiHooks[enumFunction].lpNewProcAddr = lpNewProcAddr;

			return &hook::hiHooks[enumFunction];
		}

		typedef HANDLE (WINAPI* __pfnCreateFileA)(__in LPCSTR lpFileName, __in DWORD dwDesiredAccess, __in DWORD dwShareMode, __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes, __in DWORD dwCreationDisposition, __in DWORD dwFlagsAndAttributes, __in_opt HANDLE hTemplateFile);

		HANDLE WINAPI CreateFileA(__in LPCSTR lpFileName, __in DWORD dwDesiredAccess, __in DWORD dwShareMode, __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes, __in DWORD dwCreationDisposition, __in DWORD dwFlagsAndAttributes, __in_opt HANDLE hTemplateFile);
	}

}