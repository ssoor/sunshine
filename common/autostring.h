#pragma once
#include <windows.h>
#include <string.h>

class autostring {
public:
	autostring(const wchar_t * pszSourceStr, size_t sizeStringLen = 0) {
		InitVariant();

		if (0 == sizeStringLen) {
			sizeStringLen = wcslen(pszSourceStr);
		}

		sizeStringLen += 1;
		this->m_pUnicodeString = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeStringLen * sizeof(pszSourceStr[0]));
		memcpy(this->m_pUnicodeString, pszSourceStr, sizeStringLen * sizeof(pszSourceStr[0]));
	}
	autostring(const char * pszSourceStr, size_t sizeStringLen = 0, bool bIsUTF8 = false) {
		InitVariant();

		if (0 == sizeStringLen) {
			sizeStringLen = strlen(pszSourceStr);
		}

		if (bIsUTF8) {
			sizeStringLen += 1;
			this->m_pUTF8String = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeStringLen * sizeof(pszSourceStr[0]));
			memcpy(this->m_pUTF8String, pszSourceStr, sizeStringLen * sizeof(pszSourceStr[0]));
		}
		else {
			sizeStringLen += 1;
			this->m_pAscllString = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeStringLen * sizeof(pszSourceStr[0]));
			memcpy(this->m_pAscllString, pszSourceStr, sizeStringLen * sizeof(pszSourceStr[0]));
		}
	}
	~autostring() {
		if (m_pAscllString) {
			HeapFree(GetProcessHeap(), 0, m_pAscllString);
		}
		if (m_pUTF8String) {
			HeapFree(GetProcessHeap(), 0, m_pUTF8String);
		}
		if (m_pUnicodeString) {
			HeapFree(GetProcessHeap(), 0, m_pUnicodeString);
		}
	}

private:
	void InitVariant() {
		this->m_pAscllString = NULL;
		this->m_pUTF8String = NULL;
		this->m_pUnicodeString = NULL;
	}
public:
	LPCSTR ptr() {
		if (NULL == this->m_pAscllString) {
			int nLenOfWideChar;
			nLenOfWideChar = WideCharToMultiByte(CP_ACP, 0, this->wptr(), -1, NULL, 0, NULL, NULL);
			if (0 == nLenOfWideChar) {
				nLenOfWideChar = 1;
			}

			this->m_pAscllString = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nLenOfWideChar + 1);

			if (0 == WideCharToMultiByte(CP_ACP, 0, this->wptr(), -1, this->m_pAscllString, nLenOfWideChar, NULL, NULL)) {
				this->m_pAscllString[0] = '\0';
			}
		}

		return this->m_pAscllString;
	}

	const char * uptr() {
		if (NULL == this->m_pUTF8String) {
			int nLenOfWideChar;
			nLenOfWideChar = WideCharToMultiByte(CP_UTF8, 0, this->wptr(), -1, NULL, 0, NULL, NULL);
			if (0 == nLenOfWideChar) {
				nLenOfWideChar = 1;
			}

			this->m_pUTF8String = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (nLenOfWideChar + 1));

			if (0 == WideCharToMultiByte(CP_UTF8, 0, this->wptr(), -1, this->m_pUTF8String, nLenOfWideChar, NULL, NULL)) {
				this->m_pUTF8String[0] = L'\0';
			}
		}

		return this->m_pUTF8String;
	}

	LPCWSTR wptr() {
		if (NULL == this->m_pUnicodeString) {
			int nLenOfWideChar;
			nLenOfWideChar = MultiByteToWideChar(CP_ACP, 0, this->ptr(), -1, NULL, 0);
			if (0 == nLenOfWideChar) {
				nLenOfWideChar = 1;
			}

			this->m_pUnicodeString = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (nLenOfWideChar + 1) * sizeof(wchar_t));

			if (0 == MultiByteToWideChar(CP_ACP, 0, this->ptr(), -1, this->m_pUnicodeString, nLenOfWideChar)) {
				this->m_pUnicodeString[0] = L'\0';
			}
		}

		return this->m_pUnicodeString;
	}

	LPCTSTR tptr() {
#ifdef UNICODE
		return this->wptr();
#else
		return this->ptr();
#endif
	}

protected:
	char * m_pAscllString;
	char * m_pUTF8String;
	wchar_t * m_pUnicodeString;
};
