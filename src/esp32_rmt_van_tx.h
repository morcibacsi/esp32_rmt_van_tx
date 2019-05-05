/*
    ESP32 library to write to a 125kbps VAN bus
*/

#pragma once

#ifndef _esp32_rmt_van_tx_h
    #define _esp32_rmt_van_tx_h

    #include "driver/rmt.h"
    #include "driver/periph_ctrl.h"
    #include "soc/rmt_reg.h"

    #ifdef __cplusplus
    extern "C" {
    #endif

    void rmt_van_tx_send_raw_message(uint8_t data[], uint8_t lengthOfData, bool includeSoF, bool calculateCrc, bool includeEoD);
    void rmt_van_tx_channel_init(uint8_t channel, uint8_t txPin);
    void rmt_van_tx_channel_stop(uint8_t channel);

    #ifdef __cplusplus
    } // extern C
    #endif
#endif