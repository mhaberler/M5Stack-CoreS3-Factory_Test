## CoreS3 User Demo

lifted from https://github.com/luisllamasbinaburo/M5Stack-CoreS3-Factory_Test
made to build with PlatformIO

my proving ground for testing M5Unifed/M5GFX + LVGL + Sd/SdFat interop


## Targets

arduino2 = current Arduino 2.x environment
arduino3 = Arduino 3 alpha 2, see https://github.com/platformio/platform-espressif32/issues/1211

sd - stock SD support
sdfat - uses [SdFat](https://registry.platformio.org/libraries/adafruit/SdFat%20-%20Adafruit%20Fork)


## bugs with Arduino 3 alpha 2:
- sound output does not work 
- camera example hangs, no more touch reaction 
- everything else seems to work

NB arduino3 uses a modified M5GFX repo: 	https://github.com/mhaberler/M5GFX.git#arduino-3.0.0-improved

## Webserver
I added a standard Arduino Webserver as a load + interop test - works ok on 2 and 3

uncomment -DWEBSERVER to enable

credentials are taken from environment

```
	-DWIFI_SSID=\"${sysenv.WIFI_SSID}\"
	-DWIFI_PASSWORD=\"${sysenv.WIFI_PASSWORD}\"
``````

## BLE scanning
add https://github.com/h2zero/esp-nimble-cpp.git and setting up an basic scanner works great and simultaneously with WiFi on Arduino3

uncomment -DTEST_NIMBLE to enable

I tried https://github.com/h2zero/NimBLE-Arduino but that didnt work in the Arduino2 setup, unclear why

dark screen (i.e no updates, eventual hang)
