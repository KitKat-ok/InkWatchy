#include "ntp.h"

RTC_DATA_ATTR time_t previousNTPTimeFull = 0; // Full latest NTP sync time
RTC_DATA_ATTR time_t previousNTPTimeDifference = 0; // The delay between 2 previous NTP syncs 
RTC_DATA_ATTR int ntpDriftCorrection = 0; // The drift
RTC_DATA_ATTR time_t previousNTPCorrection = 0; // UNIX time of previous drift correction

bool firstNTPSync = true;
time_t initialRTCTime = 0;
int ntpTries = 0;

void syncNtp() {
    debugLog("Running syncNtp");
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP);
    timeClient.begin();
    if(timeClient.update() == true) {
        timeClient.setTimeOffset(TIME_OFFSET_S);
        time_t epochTime = timeClient.getEpochTime();
        SRTC.read(*timeRTC);
        time_t currentTime = SRTC.MakeTime(*timeRTC); // Save the time to a var and update it afterwards for better accuracy
        if(firstNTPSync == true) {
            debugLog("firstNTPSync is true, running again");
            firstNTPSync = false;
            initialRTCTime = currentTime;
            SRTC.BreakTime(epochTime, *timeRTC);
            saveRTC();
            timeClient.end();
            syncNtp();
            return;
        } else {
            int difference = abs(epochTime - currentTime);
            debugLog("difference: " + String(difference));
            if(difference > 5) {
                debugLog("Difference too high, running ntp once more");
                SRTC.BreakTime(epochTime, *timeRTC);
                saveRTC();
                timeClient.end();
                syncNtp();
                return;
            }
        }
        debugLog("NTP success");
        debugLog("NTP time: " + timeClient.getFormattedTime());
        currentTime = initialRTCTime;

        /* GuruSR:
        SmallRTC doesn't cause time to drift.  The drift values are all reset on init, they never change anything when like that.  Are you using the SRTC's make and break time functions, if not, that is what is happening, the standard makeTime and breakTime are not compliant to time.h, they add month and day onto the values.  With the correct TimeZone (TZ) used, using localTime against the RTC having UTC values, you'll get accurate DST calculations. 
        
        I haven't looked at your code, but the make and break Time functions outside of SmallRTC don't follow time.h for values, so the day and month will increase.
        */
        
        SRTC.BreakTime(epochTime, *timeRTC);
        saveRTC();

        if(previousNTPTimeFull == 0) {
            previousNTPTimeFull = epochTime;
            debugLog("Setting initial ntp time for drift");
        } else {
            debugLog("Drift calc - NTP time: " + getFormattedTime(epochTime));
            debugLog("Drift calc - RTC time: " + getFormattedTime(currentTime));
            int ntpDriftCorrectionTmp = currentTime - epochTime;
            if(ntpDriftCorrection == 0 || abs(ntpDriftCorrectionTmp) > ACCEPTABLE_DRIFT) {
                previousNTPTimeDifference = epochTime - previousNTPTimeFull;
                ntpDriftCorrection = ntpDriftCorrectionTmp;
            } else {
                //debugLog("Drift correction for delay: " + String(previousNTPTimeDifference) + " is fine");
            }
            debugLog("Drift calc - ntpDriftCorrection:" + String(ntpDriftCorrection));

            previousNTPTimeFull = epochTime;
            previousNTPCorrection = epochTime;
        }

        timeClient.end();
        wakeUpManageRTC(); // After syncing time, remake the alarm
    } else {
        debugLog("NTP error");
        if(ntpTries < 10) {
            ntpTries = ntpTries + 1;
            syncNtp();
        }
    }
}

void checkDrift() {
    time_t currentTime = SRTC.MakeTime(*timeRTC);
    if(currentTime < 0) {
        debugLog("SOMETHING IS WRONG :(");
        readRTC();
        currentTime = SRTC.MakeTime(*timeRTC);
    }
    debugLog("currentTime: " + String(currentTime));
    debugLog("previousNTPCorrection: " + String(previousNTPCorrection));
    debugLog("previousNTPTimeDifference: " + String(previousNTPTimeDifference));
    while(previousNTPCorrection != 0 && ntpDriftCorrection != 0 && abs(currentTime - previousNTPCorrection) >= previousNTPTimeDifference) {
        previousNTPCorrection = previousNTPCorrection + previousNTPTimeDifference;
        debugLog("Doing drift correction!");
        debugLog("ntpDriftCorrection: " + String(ntpDriftCorrection));
        debugLog("previousNTPCorrection: " + String(previousNTPCorrection));
        debugLog("previousNTPTimeDifference: " + String(previousNTPTimeDifference));
        currentTime = currentTime - ntpDriftCorrection; // Not sure
        debugLog("currentTime: " + String(currentTime));
        SRTC.BreakTime(currentTime, *timeRTC);
        saveRTC();
    }
}