#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <map>
#include <time.h>

#include <switch.h>

#define INNER_HEAP_SIZE 0x80000

extern "C"
{
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

    void __appInit(void)
    {
        if (R_FAILED(smInitialize()))
        {
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));
        }
        Result rc = setsysInitialize();
        if (R_SUCCEEDED(rc))
        {
            SetSysFirmwareVersion fw;
            rc = setsysGetFirmwareVersion(&fw);
            if (R_SUCCEEDED(rc))
                hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
            setsysExit();
        }
        rc = fsInitialize();
        if (R_FAILED(rc))
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));
        fsdevMountSdmc();

        rc = hidInitialize();
        if (R_FAILED(rc))
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));
    }

    void __appExit(void)
    {
        hidExit();
        fsdevUnmountAll();
        fsExit();
        smExit();
    }
}

void WriteToLog(std::string message)
{
    FILE *fp = fopen("sdmc:/nxreader.log", "a");

    time_t unixTime = time(NULL);
    struct tm* timeStruct = gmtime((const time_t *)&unixTime);

    int hours = timeStruct-> tm_hour;
    int minutes = timeStruct-> tm_min;
    int seconds = timeStruct-> tm_sec;

    fprintf(fp, "%02i:%02i:%02i   ", hours, minutes, seconds);
    fprintf(fp, message.c_str());
    fprintf(fp, "\n");

    fclose(fp);
}

static const HidsysNotificationLedPattern breathingpattern = {
    .baseMiniCycleDuration = 0x8,
    .totalMiniCycles = 0x2,
    .totalFullCycles = 0x2,
    .startIntensity = 0x2,
    .miniCycles = {
        {
            .ledIntensity = 0xF,
            .transitionSteps = 0xF,
            .finalStepDuration = 0x0,
        },
    },
};

static void sendPatternStatic(const HidsysNotificationLedPattern* pattern)
{
    s32 total_entries;
    HidsysUniquePadId unique_pad_ids[2]={0};

    Result rc = hidsysGetUniquePadsFromNpad(HidNpadIdType_Handheld, unique_pad_ids, 2, &total_entries);
    if (R_FAILED(rc))
        return;

    for (int i = 0; i < total_entries; i++)
        rc = hidsysSetNotificationLedPattern(pattern, unique_pad_ids[i]);
}

void flashLed()
{
    Result rc = hidsysInitialize();
    if (R_FAILED(rc))
        fatalThrow(rc);
    sendPatternStatic(&breathingpattern);
    hidsysExit();
}

int main(int argc, char **argv)
{
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    hidInitializeTouchScreen();

    bool recording = false;

    while (appletMainLoop())
    {

        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_ZL && kDown & HidNpadButton_ZR && kDown & HidNpadButton_L && kDown & HidNpadButton_R)
        {
            fclose(fopen("sdmc:/nxreader.log", "w"));
            flashLed();
        }

        if (kDown & HidNpadButton_ZL && kDown & HidNpadButton_ZR)
        {
            recording = !recording;

            if (recording)
            {
                WriteToLog("Recording...");
                flashLed();
            } else {
                WriteToLog("No longer recording...");
                flashLed();
            }
        }

        if (recording)
        {

            HidTouchScreenState state = {0};
            if (hidGetTouchScreenStates(&state, 1)) {
                for(s32 i = 0; i < state.count; i++)
                {
                    std::string TouchStateX = std::to_string(state.touches[i].x);
                    std::string TouchStateY = std::to_string(state.touches[i].y);
                    WriteToLog("X = " + TouchStateX + ", Y = " + TouchStateY);
                }
            }

            if (kDown & HidNpadButton_X)
            {
                WriteToLog("X");
            }

            if (kDown & HidNpadButton_A)
            {
                WriteToLog("A");
            }

            if (kDown & HidNpadButton_B)
            {
                WriteToLog("B");
            }

            if (kDown & HidNpadButton_Y)
            {
                WriteToLog("Y");
            }

            if (kDown & HidNpadButton_Plus)
            {
                WriteToLog("PLUS");
            }

            if (kDown & HidNpadButton_Minus)
            {
                WriteToLog("MINUS");
            }

            if (kDown & HidNpadButton_Up)
            {
                WriteToLog("DUP");
            }

            if (kDown & HidNpadButton_Right)
            {
                WriteToLog("DRIGHT");
            }

            if (kDown & HidNpadButton_Down)
            {
                WriteToLog("DDOWN");
            }

            if (kDown & HidNpadButton_Left)
            {
                WriteToLog("DLEFT");
            }

            if (kDown & HidNpadButton_L)
            {
                WriteToLog("L");
            }

            if (kDown & HidNpadButton_ZL)
            {
                WriteToLog("ZL");
            }

            if (kDown & HidNpadButton_R)
            {
                WriteToLog("R");
            }

            if (kDown & HidNpadButton_ZR)
            {
                WriteToLog("ZR");
            }

            if (kDown & HidNpadButton_StickL)
            {
                WriteToLog("LSTICK");
            }

            if (kDown & HidNpadButton_StickR)
            {
                WriteToLog("RSTICK");
            }
                        
            if (kDown & HiddbgNpadButton_Home)
            {
                WriteToLog("HOME");
            }

            if (kDown & HiddbgNpadButton_Capture)
            {
                WriteToLog("CAPTURE");
            }
        }

        svcSleepThread(1e+8L);
    }
    return 0;
}