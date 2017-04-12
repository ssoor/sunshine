#pragma once
#include <Audiopolicy.h>
#include <mmdeviceapi.h>
#include <Endpointvolume.h>

void SeVolume(unsigned short usVolume /* 0 - 100% */);
void SeProcessVolume(DWORD dwTargetProcessID, unsigned short usVolume /* 0 - 100% */);
bool SetMMDeviceVolume(IMMDevice* pDeviceOut, unsigned short usVolume /* 0-100%*/);

IMMDevice* GetAudioDefaultEndpoint(IMMDeviceEnumerator* pEnumerator = NULL);
IMMDeviceCollection* GetAudioDeviceEnumerator(IMMDeviceEnumerator* pEnumerator = NULL);
