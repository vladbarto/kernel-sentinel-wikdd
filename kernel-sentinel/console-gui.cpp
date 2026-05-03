#include "console-gui.h"

// ********** Scroll down for Kernel Sentinel GUI ********** 

/////////////////////////////////////////
// Here goes GUI definitions for       //
// the first 5 labs implementing       //
// legacy NT drivers and experimenting //
/////////////////////////////////////////
void renderGui() {
	printf("\n--------------------------------------\n");
	printf("Welcome to Kernel Sentinel!\r\n");

	printf("Currently you have the following options: \n");
	printf("\t 1. help\n");
	printf("\t 2. start\n");
	printf("\t 3. list processes\n");
	printf("\t 4. call first IOCTL\n");
	printf("\t 5. call second IOCTL\n");
	printf("\t 6. call Driver_2 for level 2\n");
	printf("\t 7. call TP Initializer\n");
	printf("\t 8. call TP Work Item submission\n");
	printf("\t 9. call TP stopper & unloader\n");

	printf("\t Any key. exit\n");

	printf("To execute a command type its corresponding number.\n");
	printf("Input: ");
}

void help() {
	printf("[help]\r\n");
	printf("\n--------------------------------------\n");
	printf("This is the Help interface of Kernel Sentinel.\n");
	printf("TBD: --args, indications, variablesss\n");
}

void start() {
	printf("[start]\r\n");
	printf("This is the starting point of threadpool of Kernel Sentinel.\n");
}

void listProcessesUM() {
	printf("[list processes]\r\n");
	printf("This feature lists all the processes at the time of querying.\n");
}

void firstIoctl() {
	printf("[call first IOCTL]\r\n");
}

void secondIoctl() {
	printf("[call second IOCTL]\r\n");
}

void IoctlDriver_2() {
	printf("[call Driver_2]\r\n");
}

void ThreadpoolInit() {
	printf("[call Threadpool Init in ThreadpoolDriver]\r\n");
}

void ThreadpoolUninit() {
	printf("[call Threadpool Stop & Unload in ThreadpoolDriver]\r\n");
}

void ThreadpoolSubmitWorkItem() {
	printf("[call Threadpool Submit Work Item in ThreadpoolDriver]\r\n");
}

void exit() {
	printf("[exit]\r\n");
	printf("Exit invoked");
}

int RunLegacyDriverFeatures() {
    while (true) {
        renderGui();
        char command = 0;
        int success = scanf_s(" %c", &command, 1);
        if (!success)
            return 0;

        switch (command) {
        case '1': {
            help();
            break;
        }
        case '2': {
            start();
            startJob();
            break;
        }
        case '3': {
            listProcessesUM();
            listAllRunningProcessesOnSystem();
            break;
        }
        case '4': {
            firstIoctl();
            char charArg[2] = "1";
            PCHAR* arg = (PCHAR*)malloc(sizeof(PCHAR) * 1);
            arg[0] = charArg;

            ULONG success = UserModeDriverCaller(1, arg);
            if (-1 == success)
                printf("Request resolution: Fail");
            break;
        }
        case '5': {
            secondIoctl();
            char charArg[2] = "2";
            PCHAR* arg = (PCHAR*)malloc(sizeof(PCHAR) * 1);
            arg[0] = charArg;

            ULONG success = UserModeDriverCaller(1, arg);
            if (-1 == success)
                printf("Request resolution: Fail");
            break;
        }
        case '6': {
            IoctlDriver_2();
            char charArg[2] = "3";
            PCHAR* arg = (PCHAR*)malloc(sizeof(PCHAR) * 1);
            arg[0] = charArg;

            ULONG success = UserModeDriverCaller(1, arg);
            if (-1 == success)
                printf("Request resolution: Fail");
            break;
        }
        case '7': {
            ThreadpoolInit();
            char charArg[2] = "4";
            PCHAR* arg = (PCHAR*)malloc(sizeof(PCHAR) * 2);
            arg[0] = charArg;
            printf("Number of threads: ");
            if (!scanf_s(" %9s", arg[1], 10))
            {
                printf("Error while reading nr of threads!");
                __debugbreak();
                free(arg);
                free(arg);
                return -1;
            }
            __debugbreak();
            printf("Number of threads desired = %s\r\n", arg[1]);
            ULONG success = UserModeDriverCaller(2, arg);
            if (-1 == success)
                printf("Request resolution: Fail");
            free(arg);
            free(arg);
            break;
        }
        case '8': {
            ThreadpoolSubmitWorkItem();
            char charArg[2] = "5";
            PCHAR* arg = (PCHAR*)malloc(sizeof(PCHAR) * 1);
            arg[0] = charArg;

            ULONG success = UserModeDriverCaller(1, arg);
            if (-1 == success)
                printf("Request resolution: Fail");
            break;
        }
        case '9': {
            ThreadpoolUninit();
            char charArg[2] = "6";
            PCHAR* arg = (PCHAR*)malloc(sizeof(PCHAR) * 1);
            arg[0] = charArg;

            ULONG success = UserModeDriverCaller(1, arg);
            if (-1 == success)
                printf("Request resolution: Fail");
            break;
        }
        default:
            exit();
            return 0;
        }
    }

    return 0;
}

///////////////////////////////////
// Here goes GUI definitions for //
// our Procmon-like monitoring   //
// tool -- Kernel Sentinel       //
///////////////////////////////////
#include "global_data.h"
#include "communication\driver_commands.h"


VOID
Help() {
    printf("Available commands are:\r\n");
    printf("\t help\r\n");
    printf("\t version\r\n");
    printf("\t exit\r\n");
    printf("\t sentinel start_filter < process | thread | image | registry | file >\r\n");
    printf("\t sentinel stop_filter < process | thread | image | registry | file >\r\n");
}

NTSTATUS
ReadLine(char *buf, int count)
{
    char* result = fgets(buf, count, stdin);
    if (result != NULL) {
        return STATUS_SUCCESS;
    }

    return STATUS_UNSUCCESSFUL;
}

NTSTATUS
CompareStrings
(
    _In_ char* AStr,
    _In_ char* BStr
)
{
    int res = strcmp(AStr, BStr);
    if (res != 0) {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
FirstPosition(_In_ PCHAR word, _In_ BYTE argc)
{
    char sentinel[] = "sentinel";
    char help[]     = "help";
    char version[]  = "version";
    char exit[]     = "exit";

    if (NT_SUCCESS(CompareStrings(word, sentinel)))
    {
        if (argc != MAX_ARGUMENTS)
        {
            printf("Start_filter command expects an argument mentioning what to start filtering\r\n");
            return STATUS_INVALID_PARAMETER;
        }
        else
            return STATUS_SUCCESS;
    }

    if (NT_SUCCESS(CompareStrings(word, help))) {
        Help();
        return STATUS_SUCCESS;
    }

    if (NT_SUCCESS(CompareStrings(word, version))) {
        ULONG DriverVersion = 0;
        return CmdGetDriverVersion(&DriverVersion);
    }

    if (NT_SUCCESS(CompareStrings(word, exit))) {
        printf("Process is exiting normally...\r\n");
        return STATUS_UNSUCCESSFUL;
    }

    printf("Unexpected command! Exiting abruptly!\r\n");
    return STATUS_INVALID_PARAMETER;
}

NTSTATUS
SecondPosition(_In_ PCHAR word, _In_ BYTE argc)
{
    char start_filter[] = "start_filter";
    char stop_filter[] = "stop_filter";

    if (NT_SUCCESS(CompareStrings(word, start_filter))) {
        if (argc != MAX_ARGUMENTS)
        {
            printf("sentinel is like git -- expecting arguments. Type 'help' to see available commands\r\n");
            return STATUS_INVALID_PARAMETER;
        }
        else
            return STATUS_SUCCESS;
    }

    if (NT_SUCCESS(CompareStrings(word, stop_filter))) {
        if (argc != MAX_ARGUMENTS)
        {
            printf("Stop_filter command expects an argument mentioning what to stop filtering\r\n");
            return STATUS_INVALID_PARAMETER;
        }
        else
            return STATUS_SUCCESS;
    }

    printf("Unknown argument: %s. Exiting process\r\n", word);
    return STATUS_INVALID_PARAMETER;
}

NTSTATUS
ThirdPosition(_In_ PCHAR word, _In_ PCHAR secondArg)
{
    char process[] = "process";
    char thread[] = "thread";
    char image[] = "image";
    char registry[] = "registry";
    char file[] = "file";

    char start_filter[] = "start_filter";
    char stop_filter[] = "stop_filter";

    MY_DRIVER_COMMAND_NOTIFICATION_TYPE_CODE notificationType = commNone;

    if (NT_SUCCESS(CompareStrings(word, process))) {
        notificationType = commProcessFilter;
    }

    if (NT_SUCCESS(CompareStrings(word, thread))) {
        notificationType = commThreadFilter;
    }

    if (NT_SUCCESS(CompareStrings(word, image))) {
        notificationType = commImageFilter;
    }

    if (NT_SUCCESS(CompareStrings(word, registry))) {
        notificationType = commRegistryFilter;
    }

    if (NT_SUCCESS(CompareStrings(word, file))) {
        notificationType = commFileFilter;
    }

    if (NT_SUCCESS(CompareStrings(secondArg, start_filter))) {
        NTSTATUS status = CmdStartMonitoring(notificationType);
        if (NT_SUCCESS(status)) {
            printf("Started monitoring: %s\r\n", word);
        }
    }
    else if (NT_SUCCESS(CompareStrings(secondArg, stop_filter))) {
        NTSTATUS status = CmdStopMonitoring(notificationType);
        if (NT_SUCCESS(status)) {
            printf("Stopped monitoring : % s\r\n", word);
        }
    }
    printf("Unknown argument. Exiting process\r\n");
    return STATUS_INVALID_PARAMETER;
}

NTSTATUS 
RunKernelSentinelFeatures() 
{
    int bufferSize = 1024;
    char *commandLine = (char*) malloc (bufferSize * sizeof(char));
    NTSTATUS status = STATUS_SUCCESS;
    printf("Kernel sentinel tool\r\n");

    // Starting filter driver communication
    CommDriverPreinitialize();
    status = CommDriverInitialize();
    if (status < 0)
    {
        free(commandLine);
        return status;
    }

    BOOLEAN bExit = FALSE;

    while (!bExit) {
        printf("$ ");
        memset(commandLine, 0, sizeof(char) * bufferSize);
        status = ReadLine(commandLine, bufferSize);
        if (!NT_SUCCESS(status)) {
            printf("Error occured when reading command from line. Exiting process...\r\n");
            free(commandLine);
            status = STATUS_UNSUCCESSFUL;
            break;
        }

        char* next_token = NULL;
        char delimiter[5] = " \t\r\n";
        char* word = strtok_s(commandLine, delimiter, &next_token);
        char* argv[MAX_ARGUMENTS] = { 0, 0, 0 };

        BYTE Position = 0;
        while (word != NULL) {
            size_t cchLength = 0;
            argv[Position++] = word;
            word = strtok_s(NULL, delimiter, &next_token);
        }

        // Interpreting the user command
        for (BYTE i = 0; i < Position; i++) {
            //printf("[%s]\n", argv[i]);
            switch (i) {
            case 0:
                status = FirstPosition(argv[i], Position);
                if (!NT_SUCCESS(status))
                {
                    status = STATUS_INVALID_PARAMETER_1;
                    bExit = TRUE;
                }
                break;
            case 1:
                status = SecondPosition(argv[i], Position);
                if (!NT_SUCCESS(status))
                {
                    status = STATUS_INVALID_PARAMETER_2;
                    bExit = TRUE;
                }
                break;
            case 2:
                status = ThirdPosition(argv[i], argv[i-1]); // Calling will launch request to KM
                if (!NT_SUCCESS(status))
                {
                    status = STATUS_INVALID_PARAMETER_3;
                    bExit = TRUE;
                }
                break;
            default: printf("[%s]\n", word);
            }
        }

        if (!NT_SUCCESS(status)) {
            bExit = TRUE;
        }

        
    }

    // Ending the driver communication with the filter
    CommDriverUninitialize();
    free(commandLine);
    return status;
}
