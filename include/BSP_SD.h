#pragma once

#include "esp_err.h"
#include <string>
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#ifdef CONFIG_IDF_TARGET_ESP32
#include "driver/sdmmc_host.h"
#endif

#define PIN_NUM_SD_MOSI (GPIO_NUM_13)
#define PIN_NUM_SD_MISO (GPIO_NUM_35)
#define PIN_NUM_SD_SCK (GPIO_NUM_14)
#define PIN_NUM_SD_CS (GPIO_NUM_15)
#define PIN_NUM_SD_CD (GPIO_NUM_38)
#define SPI_DMA_CHAN 1

namespace BSP
{
    constexpr auto MOUNT_POINT = "/sdcard";
    constexpr auto SDTAG = "SDTAG";

    class SD
    {
    private:
        const std::string mount_point;
        bool initialized;
        sdmmc_card_t *card;
        FILE *f;
        sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    public:
        SD();
        void Mount();
        void UnMount();
        void PrintDiskInfo();
        esp_err_t OpenFile(const std::string &file_name);
        esp_err_t WriteLine(std::string &line);
        void CloseFile();
        void DeleteFile(std::string &file_name);
        bool IsFileOpen() const;
        bool IsCardDetected() const;
    };
} // namespace BSP