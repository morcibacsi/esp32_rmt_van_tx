/* RMT VAN bus send example
 * 
 * The software is distributed under the MIT License
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

#include "esp32_rmt_van_tx.h"

void app_main()
{
    const uint8_t VAN_TX_PIN = 5;
    const uint8_t VAN_TX_CHANNEL = 1;

    // if true:  Send Airbag fault and Deadlocking active messages to the multifunction display
    // if false: Send temperature 15 C to the multifunction display
    const bool sendPopupMessage = true;
    
    uint8_t rmtMessageBytes[18] = {0x52, 0x48, 0x08, 0x40, 0x80, 0x03, 0x20, 0x01, 0x80, 0x00, 0x48, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t rmtTemperatureBytes[9] = {0x8a, 0x48, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x6D};
    uint8_t currentMsg = 0x18;

    rmt_van_tx_channel_init(VAN_TX_CHANNEL, VAN_TX_PIN);

    while(true)
    {
        if (sendPopupMessage)
        {
            // Send Airbag fault and Deadlocking active messages to the multifunction display
            if (currentMsg == 0x43)
            {
                currentMsg = 0x18;
            }
            else
            {
                currentMsg = 0x43;
            }
            rmtMessageBytes[11] = currentMsg;
            rmt_van_tx_send_raw_message(rmtMessageBytes, 18, true, true, true);
        }
        else
        {
            // Send temperature 15 C to the multifunction display
            rmt_van_tx_send_raw_message(rmtTemperatureBytes, 9, true, true, true);
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
