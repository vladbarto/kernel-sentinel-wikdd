#include "process-listing.h"

void PrintProcessNameAndID(DWORD processID)
{
    LoadProcess("ntdll.dll", "NtQueryInformationProcess");

    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    WCHAR* cmdLineBuffer = NULL;
    SIZE_T* BytesRead;
    // Get a handle to the process.

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID);

    // Get the process name.

    if (NULL != hProcess)
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
            &cbNeeded))
        {
            GetModuleBaseName(hProcess, hMod, szProcessName,
                sizeof(szProcessName) / sizeof(TCHAR));
        }

        // Get PEB based on PID
        PROCESSINFOCLASS ProcessInformationClass = ProcessBasicInformation; // Retrieves a pointer to a PEB structure 
        PVOID ProcessInformation;
        ULONG ProcessInformationLength = sizeof(PROCESS_BASIC_INFORMATION);
        ProcessInformation = (PROCESS_BASIC_INFORMATION*)malloc(ProcessInformationLength);
        PULONG ReturnLength;

        NTSTATUS status = MyNtQueryInformationProcess(
            hProcess,
            ProcessInformationClass,
            ProcessInformation,
            ProcessInformationLength,
            ReturnLength
        );

        if (status > 0) {
            printf(" Could not retrieve PEB for process with PID %d!", processID);
            goto CLEANUP;
        }

        MY_PROCESS_BASIC_INFORMATION* ProcessBasicInformation = (MY_PROCESS_BASIC_INFORMATION*)malloc(sizeof(MY_PROCESS_BASIC_INFORMATION));
        ProcessBasicInformation = (MY_PROCESS_BASIC_INFORMATION*)ProcessInformation;
        MY_PEB* peb = { 0 };
        BOOL success = ReadProcessMemory(
            hProcess,
            ProcessBasicInformation->PebBaseAddress,
            peb,
            sizeof(MY_PEB),
            BytesRead
        );

        if (!success) {
            goto CLEANUP;
        }

        MY_RTL_USER_PROCESS_PARAMETERS* UserProcessParams;
        success = ReadProcessMemory(
            hProcess,
            peb->ProcessParameters,
            UserProcessParams,
            sizeof(MY_RTL_USER_PROCESS_PARAMETERS),
            BytesRead
        );

        DWORD cmdLen = UserProcessParams->CommandLine.Length;
        cmdLineBuffer = (WCHAR*)malloc(sizeof(WCHAR) + cmdLen);

        ReadProcessMemory(
            hProcess,
            UserProcessParams->CommandLine.Buffer,
            cmdLineBuffer,
            cmdLen,
            BytesRead
        );

        // Print the process name and identifier.
        _tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);
        if (*BytesRead != 0)
            _tprintf(TEXT("\t |-> command line: %s\n"), cmdLineBuffer);
        else
            _tprintf(TEXT("\t |-> command line: <failed to retrieve command line>\n"));
    }

    // Release the handle to the process.
CLEANUP:
    CloseHandle(hProcess);
}

int listAllRunningProcessesOnSystem() {
	/*BYTE SystemInformationClass = SYSTEM_INFORMATION_CLASS.SystemBasicProcessInformation;
	PVOID SystemInformation;
	ULONG SystemInformationLength;
	PULONG ReturnLength;

	NTSTATUS status = NtQuerySystemInformation(
		SystemInformationClass,
		SystemInformation,
		SystemInformationLength,
		ReturnLength
	);*/

    // Get the list of process identifiers.

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return 1;
    }


    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            PrintProcessNameAndID(aProcesses[i]);
        }
    }

    return 0;
}