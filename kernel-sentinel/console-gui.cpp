#include "console-gui.h"

void renderGui() {
	printf("\n--------------------------------------\n");
	printf("Welcome to Kernel Sentinel!\r\n");

	printf("Currently you have the following options: \n");
	printf("\t 1. help\n");
	printf("\t 2. start\n");
	printf("\t 3. list processes\n");
	printf("\t 4. call first IOCTL\n");
	printf("\t 5. call second IOCTL\n");
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

void exit() {
	printf("[exit]\r\n");
	printf("Exit invoked");
}