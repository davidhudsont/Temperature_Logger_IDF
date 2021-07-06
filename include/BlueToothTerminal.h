#pragma once

#include <stdint.h>
#include <string>
#include "esp_spp_api.h"

#define SPP_TAG "SPP_ACCEPTOR_DEMO"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_ACCEPTOR"

enum BTCommandID
{
    LCDDSIP,
};

struct BTEvent
{
    uint32_t code;
    uint32_t value;
};

void StartBlueTooth();
bool CheckBTEventQueue(BTEvent *event);
