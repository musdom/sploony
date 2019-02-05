### Lightning Network node

Sploony currently works with [lnd](https://github.com/lightningnetwork/lnd) `v0.5.1-beta`. You will have to set up your own lnd node if you haven't already, then expose the REST interface to your local network or forward port 8080 externally. This step is quite involved especially if you have dynamic IP, but if you managed to have wallets such as Zap or Lightning Joule remotely connect to your lnd, Sploonies work similarly.

In the future, we could possibly have pre-configured boxes such as [Casa Node](https://keys.casa/lightning-bitcoin-node/) automatically discover Sploonies in its local area network and pair to them in a few clicks.

### Hardware

ESP32 development board. Preferably one with a built-in OLED display.

This current iteration of Sploony is display pin compatible with this particular board: [ESP32 0.96 inch OLED](https://www.aliexpress.com/item/ESP32-0-96-inch-OLED-for-for-Arduino-OLED-WiFi-Modules-Bluetooth-Dual-ESP8266-OLED-BME280/32837517283.html)

Version without display: [ESP-32 Development Board](https://www.aliexpress.com/item/ESP-32-ESP-32S-Development-Board-WiFi-Bluetooth-Ultra-Low-Power-Consumption-Dual-Cores-ESP32-Board/32796032726.html)

### IDE

Sploony is developed using PlatformIO and the Arduino framework for ESP32. Install PlatformIDE for VSCode, following their quick start guide:
https://docs.platformio.org/en/latest/ide/vscode.html#ide-vscode.

Try building and uploading a simple sketch like this one, just to make sure everything works fine:
https://github.com/platformio/platform-espressif32/tree/master/examples/arduino-blink

### Flashing the firmware
1. Clone the repository `git clone https://github.com/musdom/sploony.git`
2. Open the project folder in VSCode.
3. In `config.h`, fill in your WiFi settings and lnd host address.
3. Copy `tls.cert`, `readonly.macaroon` and `invoice.macaroon` from your lnd directory into the `/data` directory.
4. Run `PlatformIO:Run Task` > `Upload File System image`. This will upload the contents of `/data` into the device's flash storage.
5. Run `PlatformIO:Upload`. It should fetch and install the Espressif 32 platform and library dependencies, compile and flash the firmware into the device.
