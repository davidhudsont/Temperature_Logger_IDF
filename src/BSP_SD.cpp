
#include "BSP_SD.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sdkconfig.h"

namespace BSP
{
    void SD::Mount()
    {
        initialized = false;
        f = NULL;
        esp_err_t ret;

        esp_vfs_fat_mount_config_t mount_config = {
            .format_if_mount_failed = false,
            .max_files = 5,
            .allocation_unit_size = 16 * 1024};

        gpio_pad_select_gpio(GPIO_NUM_38);
        gpio_set_direction(GPIO_NUM_38, GPIO_MODE_INPUT);

        //const char mount_point[] = MOUNT_POINT;
        ESP_LOGI(SDTAG, "Initializing SD Card");

        if (gpio_get_level(GPIO_NUM_38) == 1)
        {
            ESP_LOGI(SDTAG, "SD Card Present!");
        }
        else
        {
            ESP_LOGE(SDTAG, "SD Card Not Present!");
            return;
        }

        ESP_LOGI(SDTAG, "Using SPI peripheral");
        sdmmc_host_t host_tmp = SDSPI_HOST_DEFAULT();

        host = host_tmp;
        host.slot = HSPI_HOST;
        spi_bus_config_t bus_cfg = {
            .mosi_io_num = PIN_NUM_SD_MOSI,
            .miso_io_num = PIN_NUM_SD_MISO,
            .sclk_io_num = PIN_NUM_SD_SCK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4000,
            .flags = 0,
            .intr_flags = 0};
        ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SPI_DMA_CHAN);
        if (ret != ESP_OK)
        {
            ESP_LOGE(SDTAG, "Failed to initialize bus.");
        }
        else
        {
            // This initializes the slot without card detect (CD) and write protect (WP) signals.
            // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
            sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
            slot_config.gpio_cs = PIN_NUM_SD_CS;
            slot_config.host_id = (spi_host_device_t)host.slot;
            ESP_LOGI(SDTAG, "Mounting SD Card");
            ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
        }

        if (ret != ESP_OK)
        {
            if (ret == ESP_FAIL)
            {
                ESP_LOGE(SDTAG, "Failed to mount filesystem. "
                                "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
            }
            else
            {
                ESP_LOGE(SDTAG, "Failed to initialize the card (%s). "
                                "Make sure SD card lines have pull-up resistors in place.",
                         esp_err_to_name(ret));
                spi_bus_free((spi_host_device_t)host.slot);
            }
            return;
        }

        initialized = true;
    }

    void SD::UnMount()
    {
        if (initialized)
        {
            // All done, unmount partition and disable SDMMC or SPI peripheral
            esp_vfs_fat_sdcard_unmount(mount_point, card);
            ESP_LOGI(SDTAG, "Card unmounted");
            //deinitialize the bus after all devices are removed
            spi_bus_free((spi_host_device_t)host.slot);
        }
    }

    void SD::PrintDiskInfo()
    {
        if (initialized)
        {
            sdmmc_card_print_info(stdout, card);
        }
    }

    esp_err_t SD::OpenFile(std::string &file_name)
    {
        std::string full_file_path = MOUNT_POINT "/" + file_name;

        if (initialized && f == NULL)
        {
            f = fopen(full_file_path.c_str(), "w");
            if (f == NULL)
            {
                ESP_LOGE(SDTAG, "Failed to open file for writing");
                ESP_LOGE(SDTAG, "File Path: %s", full_file_path.c_str());
                return ESP_FAIL;
            }
            return ESP_OK;
        }
        return ESP_FAIL;
    }

    esp_err_t SD::WriteLine(std::string &line)
    {
        if (initialized && f != NULL)
        {
            fprintf(f, "%s\n", line.c_str());
            return ESP_OK;
        }
        return ESP_FAIL;
    }

    void SD::CloseFile()
    {
        if (initialized)
        {
            fclose(f);
        }
    }

    void SD::DeleteFile(std::string &file_name)
    {
        std::string full_file_path = MOUNT_POINT "/" + file_name;

        // Check if destination file exists before deleting
        struct stat st;
        if (stat(full_file_path.c_str(), &st) == 0)
        {
            // Delete it if it exists
            unlink(full_file_path.c_str());
        }
    }

    bool SD::IsFileOpen()
    {
        return f != NULL;
    }

} // namespace BSP