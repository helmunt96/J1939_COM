#ifndef j1939_com_h
#define j1939_com_h
#include <Arduino.h>
#include "FreematicsPlus.h"
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event_base.h"
#include "esp_event.h"
#include "config.h"

#define J1939_TAG "J1939 TASK"
ESP_EVENT_DECLARE_BASE(MANAGER_EVENT);



#define J1939_TAG "J1939 TASK"

#define J1939_INIT           BIT0
#define J1939_FINNISH           BIT1


void init_j1939();

void data_j1939();

void j1939_task(void *pvParameters);

#endif