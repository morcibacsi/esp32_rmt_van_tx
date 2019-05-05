/*
    Arduino wrapper around the ESP32 library to monitor a 125kbps VAN bus
*/
#pragma once

#ifndef _esp32_arduino_rmt_van_tx_h
    #define _esp32_arduino_rmt_van_tx_h

    #include "esp32_rmt_van_tx.h"

    #ifdef __cplusplus
    extern "C" {
    #endif
        /* Include C code here */
    #ifdef __cplusplus
    }
    #endif

class ESP32_RMT_VAN_TX
{
private:
    uint8_t _txPin;
    uint8_t _channel;

public:
    ESP32_RMT_VAN_TX();
    ~ESP32_RMT_VAN_TX();

    void SendRawMessage(uint8_t data[], uint8_t lengthOfData, bool includeSoF, bool calculateCrc, bool includeEoD);
    void Init(uint8_t channel, uint8_t txPin);
    void Stop(uint8_t channel);
};
#endif
