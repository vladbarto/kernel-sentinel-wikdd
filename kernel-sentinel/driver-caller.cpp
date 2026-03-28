#include "driver-caller.h"

int Error(const char* message) {
	printf("%s (error=%d)\r\n", message, GetLastError());
	return -1;
}

DWORD 
CtlCodeDecider(ULONG argc, PCHAR argv[]) {
	if (argc == 1 && argv[0][0] == '1' && argv[0][1] == 0) // null terminated string
		return (DWORD)IOCTL_SIOCTL_METHOD_BUFFERED;
	if (argc == 1 && argv[0][0] == '2' && argv[0][1] == 0)
		return (DWORD)IOCTL_SIOCTL_METHOD_BUFFERED_2;
	if (argc == 1 && argv[0][0] == '3' && argv[0][1] == 0)
		return (DWORD)IOCTL_SIOCTL_METHOD_BUFFERED_DRIVER_2;
	if (argc == 2 && argv[0][0] == '4' && argv[0][1] == 0)
		return (DWORD)IOCTL_SIOCTL_METHOD_BUFFERED_TP_INIT;
	if (argc == 1 && argv[0][0] == '5' && argv[0][1] == 0)
		return (DWORD)IOCTL_SIOCTL_METHOD_BUFFERED_TP_SUBMIT_WORK_ITEM;
	if (argc == 1 && argv[0][0] == '6' && argv[0][1] == 0)
		return (DWORD)IOCTL_SIOCTL_METHOD_BUFFERED_TP_UNLOAD;
	return IOCTL_NONE;
}

int __cdecl
UserModeDriverCaller(
	_In_ ULONG argc,
	_In_reads_(argc) PCHAR argv[]
) {

	if (!(argc == 1 || argc == 2)) {
		return Error("Wrong NUMBER of arguments provided. Required: 1 argument (value = 1 | 2 | 3 | 4)\r\n or 2 arguments (second value = <nr_of_threads>)\r\n");
	}

	if (argc == 2) {
		if (std::stoi(argv[1]) > MAX_NR_THREADS_ALLOWED) {
			return Error("number of threads number invalid or NaN");
		}
	}

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

	DWORD CtlCodeUsed = CtlCodeDecider(argc, argv);
	if (CtlCodeUsed == IOCTL_NONE) {
		CloseHandle(hDevice);
		return Error("Wrong Ctl Code provided as argument!\r\n");
	}

	StringCbCopyA(
		InputBuffer,
		sizeof(InputBuffer),
		"This String is from UserApplication; using IOCTL_SIOCTL_METHOD_BUFFERED"
	);

	if (CtlCodeUsed == IOCTL_SIOCTL_METHOD_BUFFERED_2){
		StringCchCatA(InputBuffer, sizeof(InputBuffer), "_2");
	}

	if (CtlCodeUsed == IOCTL_SIOCTL_METHOD_BUFFERED_DRIVER_2) {
		StringCchCatA(InputBuffer, sizeof(InputBuffer), "_DRIVER_2");
	}

	if (CtlCodeUsed == IOCTL_SIOCTL_METHOD_BUFFERED_TP_INIT) {
		StringCchCatA(InputBuffer, sizeof(InputBuffer), "_TP_INIT");
	}

	if (CtlCodeUsed == IOCTL_SIOCTL_METHOD_BUFFERED_TP_SUBMIT_WORK_ITEM) {
		StringCchCatA(InputBuffer, sizeof(InputBuffer), "_TP_SUBMIT_WORK_ITEM");
	}

	if (CtlCodeUsed == IOCTL_SIOCTL_METHOD_BUFFERED_TP_UNLOAD) {
		StringCchCatA(InputBuffer, sizeof(InputBuffer), "_TP_UNLOAD");
	}

	// Place InputBuffer into the brand new structure used to send params to my Driver
	DRIVER_INPUT_UM DriverInputBuffer = { 0 };
	DRIVER_INPUT_UM DriverOutputBuffer = { 0 };
	StringCchCatA(DriverInputBuffer.InputBuffer, sizeof(InputBuffer), InputBuffer);
	DriverInputBuffer.NumberOfThreads = (argc == 2)? std::stoi(argv[1]) : 0;
	

	BOOL bSuccess = DeviceIoControl(
		// Handle to the device
		hDevice,

		// CTL Code called ({1, 2, 3, 4} as defined in driver solution)
		CtlCodeUsed,

		// input buf and size
		&DriverInputBuffer,
		(DWORD)sizeof(DriverInputBuffer),

		// output buf and size
		&DriverOutputBuffer,
		sizeof(DriverOutputBuffer),

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