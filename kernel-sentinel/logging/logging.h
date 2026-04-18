#pragma once
#include <Windows.h>
#include <stdio.h>

extern SRWLOCK gLogLock;
extern HANDLE gLogFile;

VOID
InitializeLogging(const wchar_t* LogPath);

VOID
WriteToLogFile(const wchar_t* Message);

VOID
CleanupLogging(void);
