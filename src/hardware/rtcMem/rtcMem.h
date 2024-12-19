#pragma once

#include "defines.h"

struct rtcMem
{
    // Battery
    batteryInfo bat;
    bool isBatterySaving : 1;
#if ATCHY_VER == YATCHY
    bool previousStatInStateBefore : 1;
    bool previousStatInStateAfter : 1;
    bool previousFiveVolt : 1;
#endif
    // Accelerometer
#if AXC_ENABLED
    StableBMA SBMA; // Class
    bool initedAxc : 1;
    bool stepsInited : 1;
    uint8_t stepDay; // For steps to reset each days
#endif
    // Display
    GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display;
    uint8_t updateCounter;
// Mcp23018
#if ATCHY_VER == YATCHY
    mcp23018 gpioExpander;
#endif
    // RTC
    SmallRTC SRTC;
    char posixTimeZone[POSIX_TIMEZONE_MAX_LENGTH];
    // Temp
#if TEMP_CHECKS_ENABLED
    uint8_t fixCounts;
    float initialTemp;
    float previousTemp;
#endif
    // Wifi logic
    uint64_t lastSyncUnix;
    uint64_t lastTryUnix;
// Ntp
// Time drift correction
#if TIME_DRIFT_CORRECTION
    uint64_t driftStartUnix;
#endif
// Watchdog
#if WATCHDOG_TASK
    bool everythingIsFine : 1;
#endif
    // Power settings
    bool disableAllVibration : 1;
    bool disableWakeUp : 1;
    // wManageOne
    tmElements_t wFTime;
    bool disableSomeDrawing : 1;
    uint8_t batteryPercantageWF : 7;
    // wManageAll
    uint8_t watchfaceSelected = 0;
// Inkfield watchface
#if WATCHFACE_INKFIELD_SZYBET
    struct
    {
        uint8_t dayBar;
        uint8_t percentOfDay;
        uint8_t percentSteps;
        uint16_t weatherMinutes;
        // inkput
        int watchfacePos; // It needs to be int because it's used in checkMinMax
        bool positionEngaged : 1;
    } inkfield;
// Shades watchface
#if WATCHFACE_SHADES_SZYBET
    struct
    {
        uint16_t stepsSaved;
    } shades;
#endif
#endif
    // Watchface modules
    uint64_t latestModuleUpdate;
    int currentModule; // Int because checkMinMax again
    int8_t previousModuleCount;
    int8_t previousCurrentModule;
    // wFApi
    #if API_MODULE
    wfModule wfApi;
    #endif
    // wFBit
    wfModule wfBit;
    bool isBtcDataAvail : 1;
    bool isBtcDataNew : 1;
    bool smallBtcData : 1;
    uint btcLastUpdate;
};

extern rtcMem rM;