#pragma once

#include "esp_err.h"
#include <string>
#ifdef CONFIG_IDF_TARGET_ESP32
#include "driver/sdmmc_host.h"
#endif

#define PIN_NUM_SD_MOSI (GPIO_NUM_13)
#define PIN_NUM_SD_MISO (GPIO_NUM_35)
#define PIN_NUM_SD_SCK (GPIO_NUM_14)
#define PIN_NUM_SD_CS (GPIO_NUM_15)
#define PIN_NUM_SD_CD (GPIO_NUM_38)
#define SPI_DMA_CHAN 1

#define MOUNT_POINT "/sdcard"

namespace BSP
{
    class SD
    {
    private:
        const char *SDTAG = "SDTAG";
        const char *mount_point = MOUNT_POINT;
        bool initialized;
        sdmmc_card_t *card;
        FILE *f;
        sdmmc_host_t host;

    public:
        void Mount();
        void UnMount();
        void PrintDiskInfo();
        esp_err_t OpenFile(std::string &file_name);
        esp_err_t WriteLine(std::string &line);
        void CloseFile();
    };
} // namespace BSP