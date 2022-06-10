#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_base.h"
#include "esp_event.h"

//--------CUSTOM MODULES-------------//
#include "sim_7600.h"
#include "j1939_com.h"

//-------Task Control Element-----------//
extern EventGroupHandle_t sim7600_event_group;
extern EventGroupHandle_t j1939_event_group;
// Manager Event loop
esp_event_loop_handle_t manager_event_loop_handle;
ESP_EVENT_DEFINE_BASE(MANAGER_EVENT);    
TaskHandle_t sim7600_handler;
TaskHandle_t j1939_handler;

#define MAIN_TAG "MAIN TASK"

FreematicsESP32 sys;
String Trama_http;


static void manager_event_handler(void* args, esp_event_base_t event_base, int32_t event_id, void* event_data)
{

  //***************************** EVENTS ******************************
	if (event_base == MANAGER_EVENT && event_id == MANAGER_EVENT_SIM7600_CREATED) {
		ESP_LOGI(MAIN_TAG, "Sim7600 Ready");
    return;
	}
	else if (event_base == MANAGER_EVENT && event_id == MANAGER_EVENT_SIM7600_DESTROYED) {
		ESP_LOGI(MAIN_TAG, "Sim7600 Not Available");
    return;
	}
	else if (event_base == MANAGER_EVENT && event_id == MANAGER_EVENT_SIM7600_INIT) {
		ESP_LOGI(MAIN_TAG, "Sim7600 Init");
    xEventGroupSetBits(j1939_event_group,J1939_INIT);
    return;
	}
	else if (event_base == MANAGER_EVENT && event_id == MANAGER_EVENT_SIM7600_HTTP) {
		ESP_LOGI(MAIN_TAG, "sim7600 Http");
    //vTaskDelay(pdMS_TO_TICKS(1000));
    //xEventGroupSetBits(sim7600_event_group,SIM7600_HTTP);
    return;
	}
  if (event_base == MANAGER_EVENT && event_id == MANAGER_EVENT_J1939_CREATED) {
		ESP_LOGI(MAIN_TAG, "J1939 Ready");
    return;
	}
	else if (event_base == MANAGER_EVENT && event_id == MANAGER_EVENT_J1939_DESTROYED) {
		ESP_LOGI(MAIN_TAG, "J1939 Not Available");
    return;
	}
	else if (event_base == MANAGER_EVENT && event_id == MANAGER_EVENT_J1939_INIT) {
		ESP_LOGI(MAIN_TAG, "J1939 Init");
    return;
  }
	else if (event_base == MANAGER_EVENT && event_id == MANAGER_EVENT_J1939_FINNISH) {
		ESP_LOGI(MAIN_TAG, "J1939 Finnish");
    return;
	}
  String j1939_data = *((String*) event_data);
  if (event_base == MANAGER_EVENT && event_id == MANAGER_EVENT_J1939_DATA) {
    Trama_http = j1939_data.c_str();
		ESP_LOGI(MAIN_TAG, "J1939 data: %s",Trama_http.c_str());
    xEventGroupSetBits(sim7600_event_group,SIM7600_HTTP);
    return;
	}
}
void setup() {
  pinMode(27,OUTPUT);
  //pinMode(25,OUTPUT);
  digitalWrite(27,LOW);
  //digitalWrite(25,LOW);
  delay(1000);
  digitalWrite(27,HIGH);
  //digitalWrite(25,HIGH);
  delay(1000);
  Serial.begin(115200);
  while (!sys.begin());

  esp_event_loop_args_t manager_event_loop_args = {
    .queue_size = 5,
    .task_name = "TASK FORMATION", // task will be created
    .task_priority = uxTaskPriorityGet(NULL),
    .task_stack_size = 1024*2,
    .task_core_id = tskNO_AFFINITY
  };
  esp_event_loop_create(&manager_event_loop_args, &manager_event_loop_handle);
  esp_event_handler_instance_register_with(manager_event_loop_handle, MANAGER_EVENT, ESP_EVENT_ANY_ID, manager_event_handler, manager_event_loop_handle, NULL);
  xTaskCreatePinnedToCore(&j1939_task,
                          "j1939 task",
                          4*1024,
                          NULL,
                          1,
                          &j1939_handler,
                          1);
  vTaskDelay(pdMS_TO_TICKS(100));

  xTaskCreatePinnedToCore(&sim7600_task,
                          "sim7600 task",
                          4*1024,
                          NULL,
                          1,
                          &sim7600_handler,
                          0);
  vTaskDelay(pdMS_TO_TICKS(100));
  xEventGroupSetBits(sim7600_event_group,SIM7600_INIT);
  //xEventGroupSetBits(j1939_event_group,J1939_INIT);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(100));
}