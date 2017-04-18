#include "aes_main.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <windows.h>

#include "../common/aes.h"

static void * GetFile(const char * pszFilePath, size_t * pnFilesize)
{
	FILE * pfHandle = NULL;
	void * pFuleContext = NULL;

	if (0 != fopen_s(&pfHandle, pszFilePath, "rb"))
		return NULL;

	fseek(pfHandle, 0, SEEK_END);
	*pnFilesize = ftell(pfHandle);
	fseek(pfHandle, 0, SEEK_SET);

	pFuleContext = malloc(*pnFilesize);

	fread(pFuleContext, *pnFilesize, 1, pfHandle);

	fclose(pfHandle);

	return pFuleContext;
}

static bool SetFile(const char * pszFilePath, const void * pFileData, int nFilesize)
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

int main(int argc, char *argv[], char *envp[]) {
	size_t sizeEncodeFileSize = 0;

	if (argc < 3) {
		return -1;
	}

	void * pEncodeFileContext = GetFile(argv[1], &sizeEncodeFileSize);
	if (!pEncodeFileContext) {
		return -2;
	}

	printf("aes key: %s\n", argv[3]);
	printf("aes input: %s, size: %u\n", argv[1], sizeEncodeFileSize);

	size_t sizeEncodeSize = sizeEncodeFileSize;

	if (sizeEncodeSize % AES_BLOCK_SIZE) {
		sizeEncodeSize += AES_BLOCK_SIZE;
		sizeEncodeSize -= sizeEncodeSize % AES_BLOCK_SIZE;
	}

	void * pEncodeBuffer = calloc(1,sizeEncodeSize);
	memcpy(pEncodeBuffer, pEncodeFileContext, sizeEncodeFileSize);

	DWORD key_schedule[60] = { 0 };
	BYTE iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

	printf("aes encode: %s, size: %u\n", argv[1], sizeEncodeSize);

	aes_key_setup((BYTE*)argv[3], key_schedule, 256);

	if (!aes_encrypt_cbc((BYTE*)pEncodeBuffer, sizeEncodeSize, (BYTE*)pEncodeBuffer, key_schedule, 256, iv)) {
		printf("aes encrypt is failed...\n");
	}

	printf("aes saveing: %s, size: %u\n", argv[2], sizeEncodeSize);

	if (!SetFile(argv[2], pEncodeBuffer, sizeEncodeSize)) {
		return -3;
	}

	aes_decrypt_cbc((BYTE*)pEncodeBuffer, sizeEncodeSize, (BYTE*)pEncodeBuffer, key_schedule, 256, iv);

	if (0 != memcmp(pEncodeBuffer, pEncodeFileContext, sizeEncodeFileSize)) {
		printf("aes decrypt is failed, delete %s...\n", argv[2]);
		DeleteFile(argv[2]);

		return -4;
	}

	return 0;
}