#include "sim_7600.h"

extern esp_event_loop_handle_t manager_event_loop_handle;
EventGroupHandle_t sim7600_event_group;

extern FreematicsESP32 sys;
HTTPClientSIM7600 net;
extern String Trama_http;

#define SERVER_HOST SERVER
#define SERVER_PORT 80
#define SERVER_PATH resource
#define CELL_APN "alai"
//#define CELL_APN "internet.movistar.com.co"
#define CONN_TIMEOUT 20000


void init_sim_7600()
{
  ESP_LOGI(SIM7600_TAG,"Init cellular module...");
  if (net.begin(&sys)) {
    ESP_LOGI(SIM7600_TAG,"%s",net.deviceName());
    ESP_LOGI(SIM7600_TAG," OK");
  } else {
    ESP_LOGI(SIM7600_TAG,"NO");
    return;
  }
  ESP_LOGI(SIM7600_TAG,"IMEI:");
  ESP_LOGI(SIM7600_TAG,"%s",net.IMEI);

  if (net.checkSIM()) {
    ESP_LOGI(SIM7600_TAG,"SIM Card OK");
  } else {
    ESP_LOGI(SIM7600_TAG,"No SIM Card");
  }

  ESP_LOGI(SIM7600_TAG,"Registering on network...");
  if (net.setup(CELL_APN,CONN_TIMEOUT)) {
    ESP_LOGI(SIM7600_TAG,"OK");
  } else {
    ESP_LOGI(SIM7600_TAG,"NO");
    return;
  }

  String op = net.getOperatorName();
  if (op.length()) {
    ESP_LOGI(SIM7600_TAG,"Operator:");
    ESP_LOGI(SIM7600_TAG,"%s",op.c_str());
  }

  ESP_LOGI(SIM7600_TAG,"Obtaining IP address...");
  String ip = net.getIP();
  if (ip) {
    ESP_LOGI(SIM7600_TAG,"%s",ip);
  } else {
    ESP_LOGI(SIM7600_TAG,"N/A");
  }

  int signal = net.getSignal();
  if (signal) {
    ESP_LOGI(SIM7600_TAG,"RSSI:");
    ESP_LOGI(SIM7600_TAG,"%d",signal);
    ESP_LOGI(SIM7600_TAG,"dBm");
  }
}


char devid[12] = {0};
void genDeviceID(char* buf)
{
    uint64_t seed = ESP.getEfuseMac() >> 8;
    for (int i = 0; i < 8; i++, seed >>= 5) {
      byte x = (byte)seed & 0x1f;
      if (x >= 10) {
        x = x - 10 + 'A';
        switch (x) {
          case 'B': x = 'W'; break;
          case 'D': x = 'X'; break;
          case 'I': x = 'Y'; break;
          case 'O': x = 'Z'; break;
        }
      } else {
        x += '0';
      }
      buf[i] = x;
    }
    buf[8] = 0;
}



void Http_request() {



   genDeviceID(devid);
  //ESP_LOGI(SIM7600_TAG,"%s",devid);


  ESP_LOGI(SIM7600_TAG,"Init HTTPS stack...");
  if (net.open()) {
    ESP_LOGI(SIM7600_TAG,"OK");
  } else {
    ESP_LOGI(SIM7600_TAG,"NO");
  }
  ESP_LOGI(SIM7600_TAG,"Connecting...");
  if (net.open(SERVER_HOST, SERVER_PORT)) {
    ESP_LOGI(SIM7600_TAG,"OK");
  } else {
    ESP_LOGI(SIM7600_TAG,"failed");
    net.close();
    return;
  }

  String idd(devid);
  
  // send HTTP request
   //String httpRequestData = "{\"value\":"+idd+",\"context\":{\"trama\":\""+Trama_http+"\"}}";//coloco el JSON 
  // String httpRequestData = "{\"value\":{\"device_id\":\""+idd+"\"},\"context\":{\"trama\":\""+Trama_http+"\"}}";//coloco el JSON 
   //String httpRequestData = "{\"value\":1, \"context\":{\"trama\":\""+Trama_http+"\"}}";
   String httpRequestData = "{\"value\":1, \"context\":{\"trama\":\""+Trama_http+"\"}}";
   //  String httpRequestData = "{\"value\":1, \"context\":{\"trama\":\""+idd+"\"}}";
  // sprintf(&httpRequestData[0],"{\"value\":1,\"context\":{\"trama\":\"%s\"}}",Trama_http.c_str());
  ESP_LOGI(SIM7600_TAG,"Sending request...");
  if (!net.send(METHOD_POST, SERVER_PATH, false,httpRequestData.c_str(),strlen(httpRequestData.c_str()))) {
    ESP_LOGI(SIM7600_TAG,"failed");
    net.close();
    return;
  } else {
    ESP_LOGI(SIM7600_TAG,"OK");
  }

  // receive HTTP response
  ESP_LOGI(SIM7600_TAG,"Receiving...");
  char *response;
  int bytes;
  response = net.receive(&bytes,CONN_TIMEOUT);
  Serial.print("afdssdf");
  Serial.print(response);
    Serial.print('\n');
  Serial.print("fgbgfbf");
  if (response) {
    ESP_LOGI(SIM7600_TAG,"OK");
    ESP_LOGI(SIM7600_TAG,"-----HTTP RESPONSE-----");
    ESP_LOGI(SIM7600_TAG,"%s",response);
    ESP_LOGI(SIM7600_TAG,"-----------------------");
  } else {
    ESP_LOGI(SIM7600_TAG,"failed");
    net.close();
  }
}





void sim7600_task(void *pvParameters){
  ESP_LOGD(SIM7600_TAG,"---------------------------");
	ESP_LOGD(SIM7600_TAG,"SIM7600 INITIALIZED");
	ESP_LOGD(SIM7600_TAG,"---------------------------");
  sim7600_event_group = xEventGroupCreate();
  esp_event_post_to(manager_event_loop_handle,MANAGER_EVENT,MANAGER_EVENT_SIM7600_CREATED,NULL, 0, portMAX_DELAY);
  while (true){
      EventBits_t bits = xEventGroupWaitBits(sim7600_event_group,
                                              SIM7600_INIT | SIM7600_HTTP,
                                              pdFALSE,
                                              pdFALSE,
                                              portMAX_DELAY);
      if(bits & SIM7600_INIT){
          ESP_LOGD(SIM7600_TAG,"------------------ SIM 7600 INIT -------------- ");
          xEventGroupClearBits(sim7600_event_group,SIM7600_INIT);
          init_sim_7600();
          esp_event_post_to(manager_event_loop_handle,MANAGER_EVENT,MANAGER_EVENT_SIM7600_INIT,NULL, 0, portMAX_DELAY);
      }
      else if(bits & SIM7600_HTTP){
          ESP_LOGD(SIM7600_TAG,"------------------ SIM 7600 HTTP -------------- ");
          xEventGroupClearBits(sim7600_event_group,SIM7600_HTTP);
          Http_request();
          esp_event_post_to(manager_event_loop_handle,MANAGER_EVENT,MANAGER_EVENT_SIM7600_HTTP,NULL, 0, portMAX_DELAY);
      }
  }
}
