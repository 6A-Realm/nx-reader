#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <switch.h>

void printfile(const char* path)
{
    FILE* f = fopen(path, "r");
    if (f)
    {
        char mystring[100];
        while (fgets(mystring, sizeof(mystring), f))
        {
            int a = strlen(mystring);
            if (mystring[a-1] == '\n')
            {
                mystring[a-1] = 0;
                if (mystring[a-2] == '\r')
                    mystring[a-2] = 0;
            }
            puts(mystring);
        }
        printf(">>EOF<<\n");
        fclose(f);
    } else {
        printf("errno is %d, %s\n", errno, strerror(errno));
    }
}

int main(int argc, char **argv)
{
    consoleInit(NULL);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    Result rc = fsInitialize();
    if (R_FAILED(rc))
        printf("fsInit: %08X\n", rc);
    else
    {
        printfile("sdmc:/sys-reader.log");

    }

    // Main loop
    while(true)
    {
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        // padGetButtonsDown returns the set of buttons that have been newly pressed in this frame compared to the previous one
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus) break; // break in order to return to hbmenu

        consoleUpdate(NULL);
    }

    romfsExit();
    consoleExit(NULL);
    return 0;
}