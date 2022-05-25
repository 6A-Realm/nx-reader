#include <string.h>
#include <stdio.h>
#include <switch.h>

int main(int argc, char **argv)
{
    consoleInit(NULL);

    // Configure input layout
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad
    PadState pad;
    padInitializeDefault(&pad);

    printf("Press PLUS to exit.\nPress MINUS to view logs/refresh.");

    // Main loop
    while(true)
    {
        padUpdate(&pad);
        u64 KeyDown = padGetButtonsDown(&pad);

        // Press Plus (+) button to exit out of app
        if (KeyDown & HidNpadButton_Plus) break;
        consoleUpdate(NULL);

        // Press Minus (-) button to print logs
        if (KeyDown & HidNpadButton_Minus)
        {
            consoleClear();

            // Open file
            fsInitialize();
            FILE* fp = fopen("sdmc:/nxreader.log", "r");
            char FileString[100];
            
            // Print next line if its not the end
            while (fgets(FileString, 100, fp))
            {
                puts(FileString);
            }
            printf("**END OF LOGS, OPEN ON PC TO VIEW FULL LOGS**\n");

            // Close file
            fclose(fp);
            romfsExit();
        }
    }
    consoleExit(NULL);
    return 0;
}