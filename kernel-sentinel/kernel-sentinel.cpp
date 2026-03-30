// kernel-sentinel.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "common_includer.h"
#include "threadpool.h"
#include "console-gui.h"
#include "kernel-sentinel.tmh"
#include "process-listing.h"
#include "driver-caller.h"

int main()
{
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
                PCHAR* arg = (PCHAR*) malloc(sizeof(PCHAR) * 1);
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
