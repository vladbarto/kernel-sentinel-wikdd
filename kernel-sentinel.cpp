// kernel-sentinel.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "threadpool.h"
#include "console-gui.h"

int main()
{
    //main1();

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
                exit();
                return 0;
            }
            default: ;
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
