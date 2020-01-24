# ESP32 RMT peripheral Vehicle Area Network (VAN bus) writer

Arduino friendly [VAN bus][van_network] writer library utilizing ESP32 RMT peripheral.

VAN bus is pretty similar to CAN bus. It was used in many cars (Peugeot, Citroen) made by PSA.

# *Please don't use this project! It was a proof of concept which failed. To have a fully working alternative, check this link: [ Arduino TSS463C VAN interface library][tss_463_lib]*
Although it is possible to generate VAN message packets with the RMT peripheral of the ESP32 it can't be used safely as it will overwrite messages sent by other peripherals on the bus. It kind-of works on the bench with just one multifunction display however **in a car it will cause troubles**.

### Schematics
![schema](https://github.com/morcibacsi/esp32_rmt_van_tx/raw/master/schema/esp32-sn65hvd230-mfd.png)

### On YouTube
[![WATCH IT ON YOUTUBE](https://img.youtube.com/vi/SQ3sJHYcR4E/0.jpg)](https://www.youtube.com/watch?v=SQ3sJHYcR4E "Watch it on YouTube")

### Arduino

Copy the following files to your **documents\Arduino\libraries\esp32_arduino_rmt_van_tx** folder

-   esp32_rmt_van_tx.c
-   esp32_arduino_rmt_van_tx.cpp
-   esp32_arduino_rmt_van_tx.h
-   esp32_rmt_van_tx.h
-   keywords.txt
-   library.properties

Check the **esp32_arduino_van_writer** folder for an example

### ESP-IDF

Check the **main** folder for an example

## Warning
Please note that the code does not contain any bus arbitratrion logic. So I don't recommend to use it in a car.

## TODO
- Write logic to listen if the bus is free for sending messages
- Find out the correct schematics to write to the bus (the current one above has only CAN_L connected)

## Thanks
I would like to thank Graham Auld for his work and CRC-15 implementation which can be found [here][graham_auld_crc]
Also thanks goes to Kim Eckert for his idea of [RMT durations][kim_eckert_rmt_rx]

## See also
- [VAN Analyzer for Saleae Logic Analyzer][van_analyzer]
- [ESP32 RMT peripheral VAN bus reader][esp32_van_rx]
- [Watch it working on youtube][esp32_van_tx_youtube]

[van_network]: https://en.wikipedia.org/wiki/Vehicle_Area_Network
[van_analyzer]: https://github.com/morcibacsi/VanAnalyzer/
[esp32_van_rx]: https://github.com/morcibacsi/esp32_rmt_van_rx
[graham_auld_crc]: http://graham.auld.me.uk/projects/vanbus/crc15.html
[kim_eckert_rmt_rx]: https://github.com/kimeckert/ESP32-RMT-Rx-raw
[esp32_van_tx_youtube]: https://youtu.be/SQ3sJHYcR4E](https://youtu.be/SQ3sJHYcR4E)
[tss_463_lib]: https://github.com/morcibacsi/arduino_tss463_van
