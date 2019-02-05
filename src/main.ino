#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include "qrcode.h"
#include "FS.h"
#include "SPIFFS.h"
#include "config.h"

#define FORMAT_SPIFFS_IF_FAILED true

WiFiMulti WiFiMulti;

String lndCertificate = "";
String readOnlyMacaroon = "";
String invoiceMacaroon = "";
String restBuf = "";
String invoice = "";
String paymentHash = "";
unsigned long satAmt = 0;
unsigned int invoiceIndex = 0;
String hostUid = "";

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, 5, 4);

// Create the QR code
QRCode qrcode;
uint8_t qrVersion = 10; // max 279 alphanumeric
// qrcode_getBufferSize(qrVersion)
uint8_t qrcodeData[407]; //v10 = 407, v9 = 352

String readFile(fs::FS &fs, const char * path, bool macaroon){
    Serial.printf("Reading file: %s\r\n", path);
    String fsString = "";
    File file = fs.open(path);
    if(!file || file.isDirectory()){
      Serial.println("- failed to open file for reading");
      return "";
    }
    Serial.println("- read from file:");
    if(macaroon) {
      while(file.available()) {
        uint8_t charBuf = file.read();
        if (charBuf < 0x10) {
          fsString += "0";
        }
        fsString += String(charBuf, HEX);
      }
      fsString.toUpperCase();
    } else {
        while(file.available()) {
          char charBuf = file.read();
          fsString += charBuf;
        }
    }
    return fsString;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
}

bool checkSettled() {
  DynamicJsonBuffer jsonBuffer;
  // decode payment request and store hash once
  if (paymentHash == "") {
    Serial.println("decoding payment request");
    httpsRequest("/v1/payreq/" + invoice, "", 0);
    JsonObject& root = jsonBuffer.parseObject(restBuf);
    paymentHash = root["payment_hash"].as<String>();
    satAmt = root["num_satoshis"];
    Serial.println("paymentHash: " + paymentHash);
    Serial.println("value: " + String(satAmt, DEC));
  }

  Serial.println("checking invoice status");
  httpsRequest("/v1/invoice/" + paymentHash, "", 0);
  // DynamicJsonBuffer jsonBuffer2;
  JsonObject& root2 = jsonBuffer.parseObject(restBuf);
  invoiceIndex = root2["add_index"];
  if (root2["settled"]) {
    bool settled = root2["settled"];
    Serial.println(settled);
    return settled;
  } else {
    return false;
  }
}

void newInvoice() {
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawStringMaxWidth(0, 0, 128, "Fetching new invoice...");
  display.display();
    // get new invoice
  String invoiceOptions = "{\"value\":" + String(INV_VALUE) +",\"expiry\":" + String(INV_EXPIRY) + ",\"memo\":\"" + hostUid + "\"}";
  httpsRequest("/v1/invoices", invoiceOptions, 1);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(restBuf);
  invoice = root["payment_request"].as<String>();
  // empty payment hash from previous invoice
  paymentHash = "";
  // update FS with new invoice
  writeFile(SPIFFS, "/last_invoice.txt", invoice.c_str());
}

// method: 0 = GET, 1 = POST
void httpsRequest(String endpoint, String payload, uint8_t method) {
  // Serial.println(payreq);

  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(lndCertificate.c_str());

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, LND_HOST + endpoint)) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = 0;
        if (!method) {
          https.addHeader("Grpc-Metadata-macaroon", readOnlyMacaroon);
          httpCode = https.GET();
        } else {
          https.addHeader("Grpc-Metadata-macaroon", invoiceMacaroon);
          httpCode = https.POST(payload);
        }        
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            restBuf = https.getString();
            // Serial.println(restBuf);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
}

void displayQR(uint8_t x_offset, uint8_t y_offset) {
  // generate new QR bitmap
  // String lnUri = "lightning:" + invoice;
  qrcode_initText(&qrcode, qrcodeData, qrVersion, 0, invoice.c_str());

  display.drawRect(0 + x_offset, 0 + y_offset, qrcode.size+4, qrcode.size+4);
  display.drawRect(1 + x_offset, 1 + y_offset, qrcode.size+2, qrcode.size+2);
  x_offset += 2;
  y_offset += 2;

  for (uint16_t y = 0; y < qrcode.size; y += 1) {
    // Each horizontal module
    for (uint16_t x = 0; x < qrcode.size; x += 1) {
        if (qrcode_getModule(&qrcode, x, y)) {
          // Serial.print("\u2588\u2588");
        } else {
          display.fillRect(x + x_offset, y + y_offset, 1, 1);
          // Serial.print("  ");
        }
    }
    // Serial.print("\n");
  }
}

void displayMain() {
  // clear the display
  display.clear();
  display.drawStringMaxWidth(0, 0, 64, String(satAmt, DEC) + " sat");
  display.drawStringMaxWidth(0, 12, 64, "Invoice #" + String(invoiceIndex, DEC));
  display.drawStringMaxWidth(0, 24, 64, String(ESP.getFreeHeap()));
  displayQR(64, 0);
  display.display();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  lndCertificate = readFile(SPIFFS, "/tls.cert", 0);
  Serial.println(lndCertificate);
  readOnlyMacaroon = readFile(SPIFFS, "/readonly.macaroon", 1);
  Serial.println(readOnlyMacaroon);
  invoiceMacaroon = readFile(SPIFFS, "/invoice.macaroon", 1);
  Serial.println(invoiceMacaroon);
  invoice = readFile(SPIFFS, "/last_invoice.txt", 0);

  // Initialising the UI will init the display too.
  display.init();
  // uint8_t contrast, uint8_t precharge = 241, uint8_t comdetect = 64
  display.setContrast(10, 5, 0); // min 10, 5, 0
  display.flipScreenVertically();

  int qrBuf = qrcode_getBufferSize(9);
  Serial.println(qrBuf);

  uint64_t chipid=ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
  hostUid = "esp32-" + String((uint32_t)chipid, HEX);
  Serial.println("hostname: " + hostUid);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID_1, PSK_1);
  WiFiMulti.addAP(SSID_2, PSK_2);
  WiFiMulti.addAP(SSID_3, PSK_3);
  WiFi.setHostname(hostUid.c_str());

  // wait for WiFi connection
  display.drawStringMaxWidth(0, 0, 128, "Connecting to WiFi...");
  display.display();
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println("wifi connected");
  Serial.println(WiFi.localIP());

  // initialize invoice states
  if (invoice != "") {
    if (checkSettled()) {
      Serial.println("Previous invoice paid");
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.drawStringMaxWidth(0, 0, 128, "Previous invoice paid");
      display.display();
      delay(1000);
      newInvoice();
    }
  } else {
    newInvoice();
  }
  checkSettled();
  displayMain();
}

void loop() {
  // check if last or stored invoice has been paid
  if (checkSettled()) {
    Serial.println("Invoice paid");
    for (uint8_t i = 0; i < 30; i++) {
      display.clear();
      // blink oled effect
      if (i % 2 == 0) {
        display.invertDisplay();
      } else {
        display.normalDisplay();
      }
      display.setFont(ArialMT_Plain_16);
      display.drawStringMaxWidth(0, 22, 128, String(satAmt, DEC) + " sat RECEIVED!");
      display.display();
      delay(150);
    }

    newInvoice();
    checkSettled();
    displayMain();
  }
  delay(2000);
}