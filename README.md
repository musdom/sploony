## Sploony

Sploony is a firmware that lets ESP32 SoC boards interact with the Lightning Network. One or multiple Sploonies can connect directly to a single LND to display machine-readable invoices, and act upon its invoice once paid.

![Battery powered sploony](/docs/image1.jpg)

This current iteration of Sploony is display pin compatible with this particular board: [ESP32 0.96 inch OLED](https://www.aliexpress.com/item/ESP32-0-96-inch-OLED-for-for-Arduino-OLED-WiFi-Modules-Bluetooth-Dual-ESP8266-OLED-BME280/32837517283.html)

Version without display: [ESP-32 Development Board](https://www.aliexpress.com/item/ESP-32-ESP-32S-Development-Board-WiFi-Bluetooth-Ultra-Low-Power-Consumption-Dual-Cores-ESP32-Board/32796032726.html)

The ESP32 platform is the cheapest hardware capable of practical secure connection to LND, many of which form the cheapest practical payments network. A board from Aliexpress costs US$7 plus shipping. This combo enables many new business use-cases: micro-Point of Sales, sub-cent vending machines, public lockboxes, charging stations etc.

Features:
1. Fetches invoice from LND and monitors its payment status
2. Generates QR code in device and displays on OLED screen. QR on 0.96" OLED displays readable with most modern phones.
2. Persists last invoice in SPI flash memory, and checks invoice status on powerup.
3. TLS secured connection to your own LND
4. Uses readonly and invoice macaroons, which won't compromise your funds if leaked.

To Do:
1. Check invoice expiry
2. Captive portal for WiFi and LN host config
3. Connect to BTCPayServer
4. Better modularity to support different peripherals: bigger TFT displays, Neopixel LEDs, keypad input etc.
5. Fleet management web UI