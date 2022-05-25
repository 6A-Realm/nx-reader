#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <time.h>
#include <switch.h>

#define INNER_HEAP_SIZE 0x80000

extern "C"
{
    // Settings
    u32 __nx_applet_type = AppletType_None;
    u32 __nx_fs_num_sessions = 1;
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char nx_inner_heap[INNER_HEAP_SIZE];
    void __libnx_initheap(void)
    {
        void *addr = nx_inner_heap;
        size_t size = nx_inner_heap_size;
        extern char *fake_heap_start;
        extern char *fake_heap_end;
        fake_heap_start = (char *)addr;
        fake_heap_end = (char *)addr + size;
    }

    // All initalizing code ran on startup
    void __appInit(void)
    {
        smInitialize();
        setsysInitialize();
        SetSysFirmwareVersion fw;
        setsysGetFirmwareVersion(&fw);
        hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        setsysExit();
        fsInitialize();
        fsdevMountSdmc();
        hidInitialize();
        hidsysInitialize();
        
    }

    // All exiting code
    void __appExit(void)
    {
        hidExit();
        fsdevUnmountAll();
        fsExit();
        smExit();
        hidsysExit();
    }
}

// Changing varible that will store last pressed button's time
int OldTime = 0;

// Write to file in format `wait time, pressed button`
void WriteToLog(std::string message)
{
    FILE *FilePath = fopen("sdmc:/nxreader.log", "a");

    // Get current runtime
    time_t unixTime = time(NULL);
    struct tm* timeStruct = gmtime((const time_t *)&unixTime);

    int CurrentHour = timeStruct-> tm_hour;
    int CurrentMinutes = timeStruct-> tm_min;
    int CurrentSeconds = timeStruct-> tm_sec;
    
    // Mass convert to seconds
    int SecondsConverter = (((CurrentHour * 3600) + (CurrentMinutes * 60) + CurrentSeconds));
    int TimeDifference = (SecondsConverter - OldTime);

    fprintf(FilePath, "Wait: %03i   ", TimeDifference);
    fprintf(FilePath, message.c_str());
    fprintf(FilePath, "\n");
    fclose(FilePath);

    OldTime = SecondsConverter;
}

// Yeeted from sys botbase, custom blinking pattern.
static const HidsysNotificationLedPattern LedFlashPattern = {
    .baseMiniCycleDuration = 0x8,
    .totalMiniCycles = 0x2,
    .totalFullCycles = 0x2,
    .startIntensity = 0x3,
    .miniCycles = {
        {
            .ledIntensity = 0xF,
            .transitionSteps = 0xF,
            .finalStepDuration = 0x0,
        },
    },
};

static void FlashLed(const HidsysNotificationLedPattern* pattern)
{
    s32 total_entries;
    HidsysUniquePadId unique_pad_ids[2]={0};

    hidsysGetUniquePadsFromNpad(HidNpadIdType_Handheld, unique_pad_ids, 2, &total_entries);

    for (int i = 0; i < total_entries; i++)
        hidsysSetNotificationLedPattern(pattern, unique_pad_ids[i]);
}

int main(int argc, char **argv)
{
    // Configure and initialize all gamepads and screens
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    hidInitializeTouchScreen();

    // Start recording when told to record
    bool recording = false;

    while (appletMainLoop())
    {

        padUpdate(&pad);
        u64 KeyDown = padGetButtonsDown(&pad);

        // Press L and R buttons simultaneously to clear logs
        if (KeyDown & HidNpadButton_L && KeyDown & HidNpadButton_R)
        {
            // Open files in "w" mode will clear the file
            fclose(fopen("sdmc:/nxreader.log", "w"));
            FlashLed(&LedFlashPattern);
        }

        // Press ZL and ZR simultaneously to start/stop recording
        if (KeyDown & HidNpadButton_ZL && KeyDown & HidNpadButton_ZR)
        {

            // Flip between true and false
            recording = !recording;

            if (recording)
            {
                WriteToLog("Recording...");
            } else {
                WriteToLog("No longer recording...");
            }
            FlashLed(&LedFlashPattern);

        }

        if (recording)
        {

            // Check for touch screen presses
            HidTouchScreenState state = {0};
            if (hidGetTouchScreenStates(&state, 1)) {
                for(s32 i = 0; i < state.count; i++)
                {
                    // Convert to strings so it can be written into file
                    std::string TouchStateX = std::to_string(state.touches[i].x);
                    std::string TouchStateY = std::to_string(state.touches[i].y);
                    WriteToLog("Touch: X-AXIS = " + TouchStateX + ", Y-AXIS = " + TouchStateY);
                }
            }

            // Check for button presses
            if (KeyDown & HidNpadButton_X)
            {
                WriteToLog("Press: X");
            }

            if (KeyDown & HidNpadButton_A)
            {
                WriteToLog("Press: A");
            }

            if (KeyDown & HidNpadButton_B)
            {
                WriteToLog("Press: B");
            }

            if (KeyDown & HidNpadButton_Y)
            {
                WriteToLog("Press: Y");
            }

            if (KeyDown & HidNpadButton_Plus)
            {
                WriteToLog("Press: PLUS");
            }

            if (KeyDown & HidNpadButton_Minus)
            {
                WriteToLog("Press: MINUS");
            }

            if (KeyDown & HidNpadButton_Up)
            {
                WriteToLog("Press: DUP");
            }

            if (KeyDown & HidNpadButton_Right)
            {
                WriteToLog("Press: DRIGHT");
            }

            if (KeyDown & HidNpadButton_Down)
            {
                WriteToLog("Press: DDOWN");
            }

            if (KeyDown & HidNpadButton_Left)
            {
                WriteToLog("Press: DLEFT");
            }

            if (KeyDown & HidNpadButton_L)
            {
                WriteToLog("Press: L");
            }

            if (KeyDown & HidNpadButton_ZL)
            {
                WriteToLog("Press: ZL");
            }

            if (KeyDown & HidNpadButton_R)
            {
                WriteToLog("Press: R");
            }

            if (KeyDown & HidNpadButton_ZR)
            {
                WriteToLog("Press: ZR");
            }

            if (KeyDown & HidNpadButton_StickL)
            {
                WriteToLog("Press: LSTICK");
            }

            if (KeyDown & HidNpadButton_StickR)
            {
                WriteToLog("Press: RSTICK");
            }

            if (KeyDown & HiddbgNpadButton_Home)
            {
                WriteToLog("Press: HOME");
            }
        }
        // Wait to prevent issues
        svcSleepThread(1e+8L);
    }
    // Cleanup
    return 0;
}