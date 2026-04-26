#include "logging\logging.h"

extern SRWLOCK gLogLock = SRWLOCK_INIT;
extern HANDLE gLogFile = INVALID_HANDLE_VALUE;

int LogError(const char* message) {
	wprintf(L"%hs (error=%d)\r\n", message, GetLastError());
	return -1;
}

VOID
InitializeLogging(const wchar_t* LogPath)
{
	InitializeSRWLock(&gLogLock);

	gLogFile = CreateFileW(
		LogPath,
		FILE_APPEND_DATA,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (gLogFile == INVALID_HANDLE_VALUE)
	{
		LogError("[Error] Failed to create log file\n");
	}
	else
	{
		LogError("[Info] Log file initialized!");
	}
}

VOID
WriteToLogFile(const wchar_t* Message)
{
	if (gLogFile == INVALID_HANDLE_VALUE || !Message) return;

	AcquireSRWLockExclusive(&gLogLock);

	DWORD BytesToWrite = (DWORD)wcslen(Message) * sizeof(WCHAR);
	DWORD Written;
	WriteFile(gLogFile, Message, BytesToWrite, &Written, NULL);

	WCHAR crlf[] = L"\r\n";
	WriteFile(gLogFile, crlf, sizeof(crlf), &Written, NULL);

	FlushFileBuffers(gLogFile);

	ReleaseSRWLockExclusive(&gLogLock);
}

VOID
CleanupLogging(void)
{
	if (gLogFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(gLogFile);
		gLogFile = INVALID_HANDLE_VALUE;
	}
}