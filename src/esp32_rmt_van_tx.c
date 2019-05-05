#include "esp32_rmt_van_tx.h"

volatile uint8_t _rmt_van_tx_txPin;
volatile uint8_t _rmt_van_tx_channel;

/* 
    Convert a byte to its manchester-encoded counterpart as a RMT duration 
    Idea borrowed from here: https://github.com/kimeckert/ESP32-RMT-Rx-raw
*/
void ConvertByteToManchesterEncodedRmtValues(uint8_t byteValue, int8_t rmtValues[], int rmtValuesIndex, int *rmtCount, bool isSecondByteOfCrc)
{
    int currentIndex = rmtValuesIndex;
    int previousBitValue = -1;

    if (currentIndex != -1)
    {
        previousBitValue = rmtValues[currentIndex] < 0 ? 1 : 0;
    }    

    for (int j = 7; j >= 0; j--)
    {
        int currentBitValue = (byteValue >> j) & 1;
        if (currentBitValue == 1)
        {
            if (previousBitValue == -1 || previousBitValue == 0)
            {
                currentIndex++;
                rmtValues[currentIndex] = -8;
            }
            if (previousBitValue == 1)
            {
                rmtValues[currentIndex] -= 8;
            }
        }
        else
        {
            if (previousBitValue == -1 || previousBitValue == 1)
            {
                currentIndex++;
                rmtValues[currentIndex] = 8;
            }
            if (previousBitValue == 0)
            {
                rmtValues[currentIndex] += 8;
            }
        }
        previousBitValue = currentBitValue;

        if (isSecondByteOfCrc == true && j == 1)
        {
            // when we are converting the second byte of the CRC we dont need the last two bits due to the manchester violation which marks the end of data
            break;
        }

        if (j == 4 || j == 0)
        {
            // add manchester bit
            currentIndex++;
            if (currentBitValue == 1)
            {
                rmtValues[currentIndex] = 8;
                previousBitValue = 0;
            }
            else
            {
                rmtValues[currentIndex] = -8;
                previousBitValue = 1;
            }
        }
    }
    *rmtCount = currentIndex;
}

/* Converts RMT durations to an array of rmt_item32_t */
void ConvertRmtValuesToRmtObjects(int8_t rmtValues[], int rmtValuesCount, rmt_item32_t rmtObjects[], int *rmtObjectsCount)
{
    int rmtObjectsIndex = 0;
    bool isFirstPartOfRmtObjectOccupied = false;

    for (int i = 0; i < rmtValuesCount; i++)
    {
        if (isFirstPartOfRmtObjectOccupied == false)
        {
            rmtObjects[rmtObjectsIndex].duration0 = abs(rmtValues[i]);
            rmtObjects[rmtObjectsIndex].level0 = (rmtValues[i] > 0) ? 0 : 1;
            isFirstPartOfRmtObjectOccupied = true;

            rmtObjects[rmtObjectsIndex].duration1 = 0;
            rmtObjects[rmtObjectsIndex].level1 = 0;
        }
        else
        {
            rmtObjects[rmtObjectsIndex].duration1 = abs(rmtValues[i]);
            rmtObjects[rmtObjectsIndex].level1 = (rmtValues[i] > 0) ? 0 : 1;
            rmtObjectsIndex++;
            isFirstPartOfRmtObjectOccupied = false;
        }
    }
//    rmtObjectsIndex++;
    *rmtObjectsCount = rmtObjectsIndex + 1;
}

/* 
    Calculates the CRC-15 of the input data to according to the VAN ISO/11519–3 standard 
    Borrowed from here: http://graham.auld.me.uk/projects/vanbus/crc15.html
*/
int Crc15(uint8_t data[], uint8_t lengthOfData)
{
    // computes crc value
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    uint8_t bit = 0;

    const uint8_t order = 15;
    uint8_t polynom[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x9D }; //0xF9D;
    uint8_t xor[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF };     //0x7FFF;
    uint8_t mask[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF };    //0x7FFF;

    int crc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0x00 };

    // main loop, algorithm is fast bit by bit type
    for (i = 0; i < lengthOfData; i++)
    {
        uint8_t currentByte = data[i];

        // rotate one data byte including crcmask
        for (j = 0; j < 8; j++)
        {
            bit = 0;
            if ((crc[7 - ((order - 1) >> 3)] & (1 << ((order - 1) & 7)))>0)
            {
                bit = 1;
            }
            if ((currentByte & 0x80) > 0)
            {
                bit ^= 1;
            }
            currentByte <<= 1;
            for (k = 0; k < 8; k++)     // rotate all (max.8) crc bytes
            {
                crc[k] = ((crc[k] << 1) | (crc[k + 1] >> 7)) & mask[k];
                if (bit > 0)
                {
                    crc[k] ^= polynom[k];
                }
            }
        }
    }

    int finalCrc = 0;

    // perform xor value
    for (i = 0; i < 8; i++)
    {
        crc[i] ^= xor[i];
        if (crc[i] != 0)
        {
            if (finalCrc == 0)
            {
                finalCrc |= crc[i];
            }
            else
            {
                finalCrc = crc[i] | finalCrc << 8;
            }
        }
    }

    // multiply result by 2 to turn 15 bit result into 16 bit representation
    return finalCrc << 1;
}

/* Send VAN message using RMT */
void rmt_van_tx_send_raw_message(uint8_t data[], uint8_t lengthOfData, bool includeSoF, bool calculateCrc, bool includeEoD)
{
    int8_t rmtValues[255] = { 0 };
    int rmtValueIndex = -1;

    if (includeSoF)
    {
        ConvertByteToManchesterEncodedRmtValues(0x0E, rmtValues, -1, &rmtValueIndex, false);
    }

    for (int i = 0; i < lengthOfData; i++)
    {
        ConvertByteToManchesterEncodedRmtValues(data[i], rmtValues, rmtValueIndex, &rmtValueIndex, false);
    }

    if (calculateCrc)
    {
        int crcOfData = Crc15(data, lengthOfData);

        int crcLowByte = crcOfData & 0xff;
        int crcHighByte = (crcOfData >> 8) & 0xff;

        ConvertByteToManchesterEncodedRmtValues((uint8_t)crcHighByte, rmtValues, rmtValueIndex, &rmtValueIndex, false);
        ConvertByteToManchesterEncodedRmtValues((uint8_t)crcLowByte, rmtValues, rmtValueIndex, &rmtValueIndex, true);
    }

    if (includeEoD)
    {
        rmtValueIndex++;
        rmtValues[rmtValueIndex] = 16;
    }

    rmt_item32_t rmtObjects[255];
    int rmtObjectsCount;
    ConvertRmtValuesToRmtObjects(rmtValues, rmtValueIndex + 1, rmtObjects, &rmtObjectsCount);

    rmt_write_items(_rmt_van_tx_channel, rmtObjects, rmtObjectsCount, true);
}

/* Initialize RMT transmit channel */
void rmt_van_tx_channel_init(uint8_t channel, uint8_t txPin)
{
    _rmt_van_tx_channel = channel;
    _rmt_van_tx_txPin = txPin;

    rmt_config_t rmt_tx;

    rmt_tx.channel       = channel;
    rmt_tx.gpio_num      = txPin;
    rmt_tx.clk_div       = 80; // 1 MHz, 1 us - we set up sampling to every 1 microseconds as we need to have multiple of 8 us durations (8us is the so called 'time slice' on a 125kbs VAN bus)
    rmt_tx.mem_block_num = 2;
    rmt_tx.rmt_mode      = RMT_MODE_TX;
    rmt_tx.tx_config.loop_en = false;
	rmt_tx.tx_config.carrier_duty_percent = 50;
	rmt_tx.tx_config.carrier_freq_hz = 10000;
	rmt_tx.tx_config.carrier_level   = RMT_CARRIER_LEVEL_HIGH;
	rmt_tx.tx_config.carrier_en      = false;
	rmt_tx.tx_config.idle_level      = RMT_IDLE_LEVEL_HIGH;
    rmt_tx.tx_config.idle_output_en  = true;

    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);

    // start the channel
    rmt_tx_start(_rmt_van_tx_channel, 1);
}

/* Uninstall the RMT driver */
void rmt_van_tx_channel_stop(uint8_t channel)
{
    rmt_driver_uninstall(channel);
}