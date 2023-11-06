#include <Arduino.h>
#include <FFat.h>
#include <Wire.h>

#include <nvs_flash.h>

#include "config.h"
#include "M5GFX.h"
#include "M5Unified.h"
#include "lvgl.h"
#include "m5gfx_lvgl.h"
#include "esp_camera.h"

#include <array>
#include <numeric>

#include "App.h"

#ifdef WEBSERVER

#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>

const char *ssid     = WIFI_SSID;
const char *password = WIFI_PASSWORD;

WebServer server(80);
void drawGraph();
void handleRoot(void);
void handleNotFound(void);
#endif

#ifdef TEST_NIMBLE
#include "NimBLEDevice.h"
int scanTime = 5 * 1000;  // In milliseconds, 0 = scan forever
BLEScan *pBLEScan;

bool active = false;
void startBLEscan(void);
#endif

void heap_report(void) {
    Serial.printf("Total heap: %u\n", ESP.getHeapSize());
    Serial.printf("Free heap:  %u\n", ESP.getFreeHeap());
    Serial.printf("lowest level of free heap since boot:  %u\n",
                  ESP.getMinFreeHeap());
    Serial.printf("largest block of heap that can be allocated at once:  %u\n",
                  ESP.getMaxAllocHeap());
}

void setup(void) {
    CONFIG_ESPTOOLPY_FLASHFREQ_80M;
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    auto cfg            = M5.config();
    cfg.serial_baudrate = 115200;
    cfg.led_brightness  = 192;
    cfg.clear_display   = true;
    M5.begin(cfg);

    // BM8563 Init (clear INT)
    M5.In_I2C.writeRegister8(0x51, 0x00, 0x00, 100000L);
    M5.In_I2C.writeRegister8(0x51, 0x01, 0x00, 100000L);
    M5.In_I2C.writeRegister8(0x51, 0x0D, 0x00, 100000L);

    M5.In_I2C.bitOn(AW9523_ADDR, 0x03, 0b10000000, 100000L);  // BOOST_EN

#if MONKEY_TEST_ENABLE
    M5.Speaker.setAllChannelVolume(0);
#endif
    M5.Display.setBrightness(60);

#ifdef WEBSERVER

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    server.on("/", handleRoot);
    server.on("/test.svg", drawGraph);
    server.on("/inline",
              []() { server.send(200, "text/plain", "this works as well"); });
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.printf("HTTP server started");
#endif
#ifdef TEST_NIMBLE
    startBLEscan();
#endif
    lv_init();
    m5gfx_lvgl_init();

    App_Init();

    heap_report();
}

time_t last_blink_time = 0;
void loop(void) {
    M5.update();
    lv_timer_handler();
#ifdef WEBSERVER
    server.handleClient();
#endif
    delay(10);
}

#ifdef TEST_NIMBLE

class scanCallbacks
    : public NimBLEScanCallbacks {

    void onResult(NimBLEAdvertisedDevice *advertisedDevice) {
        Serial.printf("Advertised Device Result: %s\n",
                      advertisedDevice->toString().c_str());
    }

    void onScanEnd(NimBLEScanResults results) {
        Serial.printf("Scan Ended\n");
        heap_report();

        // active = !active;
        pBLEScan->setActiveScan(active);
        Serial.printf("scan start, active = %d\n", active);
        pBLEScan->start(scanTime);
    }
};

void startBLEscan(void) {
    Serial.printf("start BLE scan...\n");

    NimBLEDevice::init("");
    pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setScanCallbacks(new scanCallbacks());
    pBLEScan->setActiveScan(active);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
    pBLEScan->start(scanTime);
}

#endif

#ifdef WEBSERVER
void drawGraph() {
    String out = "";
    char temp[100];
    out +=
        "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
        "width=\"400\" height=\"150\">\n";
    out +=
        "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" "
        "stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
    out += "<g stroke=\"black\">\n";
    int y = rand() % 130;
    for (int x = 10; x < 390; x += 10) {
        int y2 = rand() % 130;
        sprintf(temp,
                "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
                "stroke-width=\"1\" />\n",
                x, 140 - y, x + 10, 140 - y2);
        out += temp;
        y = y2;
    }
    out += "</g>\n</svg>\n";

    server.send(200, "image/svg+xml", out);
    heap_report();
}

void handleRoot() {
    char temp[400];
    int sec = millis() / 1000;
    int min = sec / 60;
    int hr  = min / 60;

    snprintf(temp, 400,

             "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

             hr, min % 60, sec % 60);
    server.send(200, "text/html", temp);
}

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, "text/plain", message);
}

void start_ws(void) {
}
#endif