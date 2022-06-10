#include "j1939_com.h"

extern FreematicsESP32 sys;
COBD obd;

extern esp_event_loop_handle_t manager_event_loop_handle;
EventGroupHandle_t j1939_event_group;

void data_j1939(void *pvParameters)
{
  // initialize OBD library
  obd.begin(sys.link);

  // start with J1939 protocol
  while (!obd.init(PROTO_J1939));
  ESP_LOGI(J1939_TAG,"J1939 data monitor started");
  while (true)
  {
    byte buf[128];
    String Trama;
    // load one received CAN message into buffer
    int bytes = obd.receiveData(buf, sizeof(buf));
    if (bytes > 0) {
      if (bytes < 3) {
        ESP_LOGI(J1939_TAG,"Invalid data");
        return;
      }
      uint32_t pgn = (uint32_t)buf[0] << 16 | (uint32_t)buf[1] << 8 | buf[2];
      // char dpgn[10] = "";
      // sprintf(dpgn,"%d",pgn);
      Trama += pgn;
      Trama += "  ";
      Serial.println();
      for (int nn = 3; nn < bytes; nn++) {
        byte d = buf[nn];
        char dato[5] = "";
        if (d < 0x10) sprintf(dato,"%02X",0);
        sprintf(dato,"%02X",d);
        Trama += dato;
      }
      //ESP_LOGI(J1939_TAG,"%s",Trama.c_str());
      esp_event_post_to(manager_event_loop_handle,MANAGER_EVENT,MANAGER_EVENT_J1939_DATA,&Trama, sizeof(Trama), portMAX_DELAY);
      vTaskDelay(pdMS_TO_TICKS(800));
    }
  }
}


void j1939_task(void *pvParameters){
  ESP_LOGD(J1939_TAG,"---------------------------");
	ESP_LOGD(J1939_TAG,"J1939 INITIALIZED");
	ESP_LOGD(J1939_TAG,"---------------------------");
  TaskHandle_t j1939_data_handle;
  j1939_event_group = xEventGroupCreate();
  esp_event_post_to(manager_event_loop_handle,MANAGER_EVENT,MANAGER_EVENT_J1939_CREATED,NULL, 0, portMAX_DELAY);
  while (true){
      EventBits_t bits = xEventGroupWaitBits(j1939_event_group,
                                              J1939_INIT | J1939_FINNISH,
                                              pdFALSE,
                                              pdFALSE,
                                              portMAX_DELAY);
      if(bits & J1939_INIT){
          ESP_LOGD(J1939_TAG,"------------------ J1939 INTERFACE INIT -------------- ");
          xEventGroupClearBits(j1939_event_group,J1939_INIT);
          xTaskCreatePinnedToCore(&data_j1939,"j1939_data",1024*3,NULL,uxTaskPriorityGet(NULL)+1,&j1939_data_handle,0);
          esp_event_post_to(manager_event_loop_handle,MANAGER_EVENT,MANAGER_EVENT_J1939_INIT,NULL, 0, portMAX_DELAY);
      }
      else if(bits & J1939_FINNISH){
          ESP_LOGD(J1939_TAG,"------------------ J1939 INTERFACE FINNISH -------------- ");
          xEventGroupClearBits(j1939_event_group,J1939_FINNISH);
          vTaskDelete(j1939_data_handle);
          esp_event_post_to(manager_event_loop_handle,MANAGER_EVENT,MANAGER_EVENT_J1939_FINNISH,NULL, 0, portMAX_DELAY);
      }
  }
}