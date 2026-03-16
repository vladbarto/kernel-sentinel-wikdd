#include "dynamic-load-library.h"

BOOL LoadProcess(const char* ModuleName, const char* ProcessName) {
	HMODULE hModuleDll = GetModuleHandleA(ModuleName);
	if (hModuleDll == NULL) {
		return FALSE;
	}

	MyNtQueryInformationProcess = (pNtQueryInformationProcess)GetProcAddress(hModuleDll, ProcessName);

	return (NtQueryInformationProcess != NULL);
}