#ifndef sim_7600_h
#define sim_7600_h
#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
#include "FreematicsPlus.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event_base.h"
#include "esp_event.h"
#include "config.h"
ESP_EVENT_DECLARE_BASE(MANAGER_EVENT);


#define APN "alai" // Change this to your Provider details
#define gprsUser ""
#define gprsPass ""
//#define X_Auth_Token "BBFF-qY0aPc5lETRyrHXb8dKBqVBzxSv8H3"//solo para ubidost
//#define SERVER "industrial.api.ubidots.com"// Change this to your selection
#define SERVER "44.200.220.115"// Change this to your selection
//#define resource "/api/v1.6/devices/esp32-gps/trama/values"//PATH
#define resource "/"//PATH
#define PORT 8080

//https://rotamfuel.com:8080
//http://44.200.220.115:8080/
#define SIM7600_TAG "SIM7600 TASK"

#define SIM7600_INIT           BIT0
#define SIM7600_HTTP           BIT1

void init_sim_7600();

void Http_request();

void sim7600_task(void *pvParameters);

#endif