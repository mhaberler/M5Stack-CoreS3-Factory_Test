#include "AppSdFatModel.h"

using namespace Page;

#include "SdFat.h"
#include "SdFatConfig.h"

SdFat sdfat;

bool AppSdFatModel::SDInit() {
    if (!init_flag)
        init_flag = sdfat.begin(SdSpiConfig(GPIO_NUM_4, SHARED_SPI, 25000000));
    return init_flag;
}

void AppSdFatModel::SDDeinit() {
    // sdfat.end();  // DE-inits the SPI bus and kills display writing
    // init_flag = false;
}

bool AppSdFatModel::GetInitFlag() {
    return init_flag;
}

void AppSdFatModel::ClearInitFlag() {
    init_flag = false;
}

bool AppSdFatModel::IsSDCardExist() {
    return (bool)!((M5.In_I2C.readRegister8(AW9523_ADDR, 0x00, 100000L) >> 4) &
                   0x01);
}
