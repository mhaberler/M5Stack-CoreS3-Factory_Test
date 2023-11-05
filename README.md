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