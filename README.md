# Sploony

Sploony is firmware that lets ESP32 SoC boards interact with the Lightning Network. One or multiple Sploonies can connect directly to a single [lnd](https://github.com/lightningnetwork/lnd) to display machine-readable invoices, and act upon its invoice once paid. See a video of it here: https://twitter.com/sakak_musdom/status/1084857547947880448

![Battery powered sploony](/docs/image1.jpg)

The ESP32 platform is the cheapest hardware capable of practical secure connection to lnd, many of which form the cheapest practical payments network. A board from Aliexpress costs US$7 plus shipping. This combo enables many new business use-cases: micro-Point of Sales, sub-cent vending machines, public lockboxes, charging stations etc.

### Setup

If you have an lnd node with a publicly reachable REST gateway, and an ESP32 development board, you can compile and run Sploony using PlatformIO or Arduino IDE. A quick guide can be found [here](/docs/SETUP.md).

### Features
* Fetches invoice from lnd and monitors its payment status
* Generates QR code in device and displays on OLED screen. QR on 0.96" OLED displays readable with most modern phones.
* Persists last invoice in SPI flash memory, and checks invoice status on powerup.
* TLS secured connection to your own lnd
* Uses readonly and invoice macaroons, which won't compromise your funds if leaked.

### To Do
* Check invoice expiry!!!
* Captive portal for WiFi and LN host config
* Connect to BTCPayServer
* Better modularity to support different peripherals: bigger TFT displays, Neopixel LEDs, keypad input etc.
* Fleet management web UI

### Notes
* Also check out https://github.com/arcbtc/1.21 if you're working with [Acinq Strike](https://strike.acinq.co/)