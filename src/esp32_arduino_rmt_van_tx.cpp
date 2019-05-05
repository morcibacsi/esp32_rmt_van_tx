#include "esp32_arduino_rmt_van_tx.h"

#ifdef __cplusplus
extern "C" {
#endif
    /* Include C code here */
#ifdef __cplusplus
}
#endif


ESP32_RMT_VAN_TX::ESP32_RMT_VAN_TX()
{
}

ESP32_RMT_VAN_TX::~ESP32_RMT_VAN_TX()
{
    rmt_van_tx_channel_stop(_channel);
}

void ESP32_RMT_VAN_TX::SendRawMessage(uint8_t data[], uint8_t lengthOfData, bool includeSoF, bool calculateCrc, bool includeEoD)
{
    rmt_van_tx_send_raw_message(data, lengthOfData, includeSoF, calculateCrc, includeEoD);
}

void ESP32_RMT_VAN_TX::Init(uint8_t channel, uint8_t txPin)
{
    _txPin = txPin;
    _channel = channel;
    rmt_van_tx_channel_init(channel, txPin);
}

void ESP32_RMT_VAN_TX::Stop(uint8_t channel)
{
    rmt_van_tx_channel_stop(channel);
}