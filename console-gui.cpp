#include "console-gui.h"

void renderGui() {
	printf("\n--------------------------------------\n");
	printf("Welcome to Kernel Sentinel!\r\n");

	printf("Currently you have the following options: \n");
	printf("\t 1. help\n");
	printf("\t 2. exit\n");

	printf("To execute a command type its corresponding number.\n");
	printf("Input: ");
}

void help() {
	printf("[help]\r\n");
	printf("\n--------------------------------------\n");
	printf("This is the Help interface of Kernel Sentinel.\n");
	printf("TBD: --args, indications, variablesss\n");
}

void exit() {
	printf("[exit]\r\n");
	printf("Exit invoked");
}