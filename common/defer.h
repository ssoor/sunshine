#pragma once
#include <malloc.h>

template<class T>
class defer_increment {
public:
	defer_increment(T * pValue) {
		m_pValue = pValue;
	}
	~defer_increment() {
		if (m_pValue) {
			(*m_pValue)++;
		}
	}

private:
	T *m_pValue;
};
template<class T>
class defer_decrease {
public:
	defer_decrease(T * pValue) {
		m_pValue = pValue;
	}
	~defer_decrease() {
		if (m_pValue) {
			(*m_pValue)--;
		}
	}

private:
	T *m_pValue;
};

#ifdef _STDIO_DEFINED
class defer_file {
public:
	defer_file(FILE * pFile) {
		m_pFile = pFile;
	}

	~defer_file() {
		if (m_pFile) {
			fclose(m_pFile);
		}
	}

private:
	FILE * m_pFile;
};
#endif

class defer_free {
public:
	defer_free(void * pMem) {
		m_hMemary = pMem;
	}

	~defer_free() {
		if (m_hMemary) {
			::free(m_hMemary);
		}
	}

private:
	void * m_hMemary;
};

#ifdef _WINBASE_
class defer_localfree {
public:
	defer_localfree(void * pMem) {
		m_hMemary = pMem;
	}

	~defer_localfree() {
		if (m_hMemary) {
			::LocalFree(m_hMemary);
		}
	}

private:
	void * m_hMemary;
};

class defer_close_handle {
public:
	defer_close_handle(HANDLE hHandle) {
		m_hHandle = hHandle;
	}

	~defer_close_handle() {
		if (NULL != m_hHandle && INVALID_HANDLE_VALUE != m_hHandle) {
			::CloseHandle(m_hHandle);
		}
	}

private:
	HANDLE m_hHandle;
};

class defer_release_mutex {
public:
	defer_release_mutex(HANDLE hMutex) {
		m_hMutex = hMutex;
	}

	~defer_release_mutex() {
		if (NULL != m_hMutex && INVALID_HANDLE_VALUE != m_hMutex) {
			::ReleaseMutex(m_hMutex);
		}
	}

private:
	HANDLE m_hMutex;
};

class defer_sleep {
public:
	defer_sleep(DWORD dwMilliseconds) {
		m_dwMilliseconds = dwMilliseconds;
	}

	~defer_sleep() {
		::Sleep(m_dwMilliseconds);
	}

private:
	int m_dwMilliseconds;
};
#endif