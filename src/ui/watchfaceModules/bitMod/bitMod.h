#pragma once

#include "defines.h"

#if BITCOIN_MODULE

struct bitcoinData {
    int height; // Btc clock
    float price;
    float change1h;
    float change24;
    float change7d;
    float change30d;
    long btcLastSyncUnix;
};

void bitcoinSync(uint8_t tries);
void wfBitcheckShow(bool *showBool, bool *redrawBool);
void wfBitrequestShow(buttonState button, bool *showBool);

#endif
