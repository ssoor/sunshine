#include "kernel32.h"

HMODULE WINAPI fake::LoadLibraryW(__in LPCWSTR lpLibFileName) {
	return ::LoadLibraryExW(lpLibFileName, NULL, 0);
}