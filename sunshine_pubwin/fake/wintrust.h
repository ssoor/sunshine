#pragma once
#include "hookinfo.h"

namespace fake {
	namespace wintrust {
		const hook::Module eModule = hook::Wintrust_dll;
		namespace func {
			enum Function {
				WinVerifyTrust,

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

	typedef LONG(WINAPI* __pfnWinVerifyTrust)(_In_ HWND   hWnd, _In_ GUID   *pgActionID, _In_ LPVOID pWVTData);

	LONG WINAPI WinVerifyTrust(_In_ HWND   hWnd, _In_ GUID   *pgActionID, _In_ LPVOID pWVTData);
}