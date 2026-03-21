#include "driver-caller.h"

int Error(const char* message) {
	printf("%s (error=%d)\r\n", message, GetLastError());
	return -1;
}

DWORD 
CtlCodeDecider(PCHAR argv[]) {
	if (argv[0][0] == '1' && argv[0][1] == 0) // null terminated string
		return (DWORD)IOCTL_SIOCTL_METHOD_BUFFERED;
	if (argv[0][0] == '2' && argv[0][1] == 0)
		return (DWORD)IOCTL_SIOCTL_METHOD_BUFFERED_2;
	return IOCTL_NONE;
}

int __cdecl
UserModeDriverCaller(
	_In_ ULONG argc,
	_In_reads_(argc) PCHAR argv[]
) {
	argc;
	argv;

	if (argc != 1) {
		return Error("Wrong NUMBER of arguments provided. Required: 1 argument (value = 1 | 2)\r\n");
	}

	printf("%s\r\n", argv[0]);

	HANDLE hDevice = NULL;
	ULONG bytesReturned = 0;

	char OutputBuffer[100] = { 0 };
	char InputBuffer[100] = { 0 };

	hDevice = CreateFileA(
		"\\\\.\\kernel-sentinel-driver",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hDevice == INVALID_HANDLE_VALUE) {
		return Error("Error: CreateFile failed");
	}

	DWORD CtlCodeUsed = CtlCodeDecider(argv);
	if (CtlCodeUsed == IOCTL_NONE) {
		CloseHandle(hDevice);
		return Error("Wrong Ctl Code (1 or 2) provided as argument!\r\n");
	}


	StringCbCopyA(
		InputBuffer,
		sizeof(InputBuffer),
		"This String is from UserApplication; using IOCTL_SIOCTL_METHOD_BUFFERED"
	);

	if (CtlCodeUsed == IOCTL_SIOCTL_METHOD_BUFFERED_2){
		StringCchCatA(InputBuffer, sizeof(InputBuffer), "2");
	}

	BOOL bSuccess = DeviceIoControl(
		// Handle to the device
		hDevice,

		// CTL Code called (1 or 2 as defined in driver solution)
		CtlCodeUsed,

		// input buf and size
		&InputBuffer,
		(DWORD)strlen(InputBuffer) + 1,

		// output buf and size
		&OutputBuffer,
		sizeof(OutputBuffer),

		// response
		&bytesReturned,
		NULL
	);

	if (!bSuccess) {
		CloseHandle(hDevice);
		return Error("Error in DeviceIoControl\r\n");
	}

	CloseHandle(hDevice);
	return 0;
}