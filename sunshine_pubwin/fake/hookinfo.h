#pragma once
#include "..\..\common\pcre\pcre.h"
#include <nektra\Deviare-InProc\Include\NktHookLib.h>

#define PCRE_MAX_OVECCOUNT 0x30

namespace rule {
	enum Type {
		File,
		Library,
		Network,
		Process,
		Registry,

		MaxTypeNumber
	};

	struct _rule {
		size_t sizeCount;
		pcre** pcreCompile;
	};

	extern _rule rHookRule[MaxTypeNumber];
}

namespace hook {
	enum Module {
		Ntdll_dll,
		User32_dll,
		Kernel32_dll,

		Wintrust_dll,
		MaxModuleNumber
	};

	struct _module_hookinfo {
		size_t sizeCount;
		CNktHookLib::HOOK_INFO* hiHookInfos;
	};

	extern _module_hookinfo tHooks[MaxModuleNumber];

	inline void DeleteHookInfo(hook::Module enumModule) {
		hook::tHooks[enumModule].sizeCount = 0;
		free(hook::tHooks[enumModule].hiHookInfos);
	}

	inline void CreateHookInfo(hook::Module enumModule, int nCount) {
		if (hook::tHooks[enumModule].hiHookInfos) {
			DeleteHookInfo(enumModule);
		}

		hook::tHooks[enumModule].sizeCount = nCount;
		hook::tHooks[enumModule].hiHookInfos = (CNktHookLib::HOOK_INFO*)malloc(sizeof(CNktHookLib::HOOK_INFO) * nCount);
	}

	inline size_t GetCount(hook::Module enumModule) {
		return hook::tHooks[enumModule].sizeCount;
	}

	inline CNktHookLib::HOOK_INFO* GetInfo(hook::Module enumModule, size_t nIndex) {
		if (hook::tHooks[enumModule].sizeCount > nIndex) {
			return &hook::tHooks[enumModule].hiHookInfos[nIndex];
		}

		return NULL;
	}
}