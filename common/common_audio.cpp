#include "common_audio.h"

class defer_release {
public:
	defer_release(IUnknown* pUnknown) {
		m_pUnknown = pUnknown;
	}
	~defer_release() {
		m_pUnknown->Release();
	}

private:
	IUnknown* m_pUnknown;
};

IMMDevice* GetAudioDefaultEndpoint(IMMDeviceEnumerator* pEnumerator /* = NULL */) {
	if (pEnumerator) {
		pEnumerator->AddRef();
	}
	if (NULL == pEnumerator && FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator))) {
		return NULL;
	}
	defer_release _(pEnumerator);

	IMMDevice* pDeviceOut = NULL;
	if (FAILED(pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDeviceOut))) {
		return NULL;
	}

	return pDeviceOut;
}

IMMDeviceCollection* GetAudioDeviceEnumerator(IMMDeviceEnumerator* pEnumerator /* = NULL */) {
	if (pEnumerator) {
		pEnumerator->AddRef();
	}
	if (NULL == pEnumerator && FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator))) {
		return NULL;
	}
	defer_release _(pEnumerator);

	IMMDeviceCollection* pCollection = NULL;
	if (FAILED(pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATEMASK_ALL, &pCollection))) {
		return NULL;
	}

	return pCollection;
}

IAudioSessionEnumerator* GetAudioSessionEnumerator(IMMDevice* pDeviceOut) {
	IAudioSessionManager2* pAudioSessionManager = 0;
	if (FAILED(pDeviceOut->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&pAudioSessionManager))) {
		return NULL;
	}
	defer_release _(pAudioSessionManager);

	IAudioSessionEnumerator* pAudioSessionEnumerator = NULL;
	if (FAILED(pAudioSessionManager->GetSessionEnumerator(&pAudioSessionEnumerator))) {
		return NULL;
	}

	return pAudioSessionEnumerator;
}


bool SetMMDeviceVolume(IMMDevice* pDeviceOut, unsigned short usVolume /* 0-100%*/) {
	int nCount = 0;
	UINT uStepCount = 0;
	UINT uCurrentStep = 0;

	do
	{
		IAudioEndpointVolume* pAudioEndpointVolume = NULL;
		if (FAILED(pDeviceOut->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume))) {
			break;
		}
		defer_release _0(pAudioEndpointVolume);

		if (FAILED(pAudioEndpointVolume->GetVolumeStepInfo(&uCurrentStep, &uStepCount))) {
			break;
		}

		nCount = int(uCurrentStep) - int((usVolume * uStepCount) / 100);
		while (0 != nCount) {
			if (nCount < 0) {
				nCount++;
				if (FAILED(pAudioEndpointVolume->VolumeStepUp(&GUID_NULL))) {
					break;
				}
			}
			else {
				nCount--;
				if (FAILED(pAudioEndpointVolume->VolumeStepDown(&GUID_NULL))) {
					break;
				}
			}
		}
	} while (false);

	return (0 == nCount);
}

void SeVolume(unsigned short usVolume /* 0 - 100% */) {
	IMMDeviceCollection* pCollection = GetAudioDeviceEnumerator();
	if (NULL == pCollection) {
		return;
	}
	defer_release _0(pCollection);

	unsigned int nCount = 0;
	pCollection->GetCount(&nCount);
	for (unsigned int i = 0; i < nCount; i++) {
		UINT uStepCount = 0;
		UINT uCurrentStep = 0;
		IMMDevice* pDeviceOut = NULL;
		if (FAILED(pCollection->Item(i, &pDeviceOut))) {
			continue;
		}

		defer_release _1(pDeviceOut);
		SetMMDeviceVolume(pDeviceOut, usVolume);
	}
}

void SeProcessVolume(DWORD dwTargetProcessID, unsigned short usVolume /* 0 - 100% */) {
	IMMDevice* pDeviceOut = GetAudioDefaultEndpoint();
	if (NULL == pDeviceOut) {
		return;
	}
	defer_release _0(pDeviceOut);

	IAudioSessionEnumerator* pAudioSessionEnumerator = GetAudioSessionEnumerator(pDeviceOut);
	if (NULL == pAudioSessionEnumerator) {
		return;
	}
	defer_release _1(pAudioSessionEnumerator);

	int nSessionCount = 0;
	if (FAILED(pAudioSessionEnumerator->GetCount(&nSessionCount))) {
		return;
	}
	IAudioSessionControl* pAudioSessionControl = NULL;
	IAudioSessionControl2* pAudioSessionControl2 = NULL;
	for (int i = 0; i < nSessionCount; i++)
	{
		if (FAILED(pAudioSessionEnumerator->GetSession(i, &pAudioSessionControl))) {
			continue;
		}
		defer_release _2(pAudioSessionControl);

		if (FAILED(pAudioSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pAudioSessionControl2))) {
			continue;
		}
		defer_release _3(pAudioSessionControl2);

		DWORD dwProcessID = 0;
		if (FAILED(pAudioSessionControl2->GetProcessId(&dwProcessID)) || dwProcessID != dwTargetProcessID) {
			continue;
		}

		if (FAILED(pAudioSessionControl2->IsSystemSoundsSession())) {
			continue;
		}

// 		AudioSessionState mAudiosessionState;
// 		if (FAILED(pAudioSessionControl2->GetState(&mAudiosessionState)) || mAudiosessionState != AudioSessionStateActive) {
// 			continue;
// 		}

		ISimpleAudioVolume* pSimpleAudioVolume = NULL;
		if (FAILED(pAudioSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSimpleAudioVolume))) {
			continue;
		}
		defer_release _5(pSimpleAudioVolume);

		pSimpleAudioVolume->SetMasterVolume(float(usVolume) / 100, NULL);
	}
}

