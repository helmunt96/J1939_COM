#include "sim_7600.h"
#include "HardwareSerial.h"
//HardwareSerial SerialAT(1);
//HardwareSerial Serial2;
extern char data[25];

unsigned long timeout;


#include <TinyGsmClient.h>
TinyGsm modem(Serial1);
TinyGsmClient client(modem);
void init_sim_7600()

{

pinMode(27,OUTPUT);
  //pinMode(25,OUTPUT);
  digitalWrite(27,LOW);
  //digitalWrite(25,LOW);
  delay(1000);
  digitalWrite(27,HIGH);
  //digitalWrite(25,HIGH);
  delay(1000);
  
  SerialMon.begin(115200);
  delay(10);
  SerialMon.println("Wait...");
  Serial1.begin(115200,SERIAL_8N1,35,2, false);
  delay(600);
  SerialMon.println("Initializing modem...");
   modem.restart();
  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(1000);
    return;
  }
  SerialMon.println(" success");
  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  SerialMon.print(F("Connecting to "));
  SerialMon.print(APN);
  if (!modem.gprsConnect(APN, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(1000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }



}


void Http_request() {
  
 

  if (!client.connect(SERVER, PORT)) {
    SerialMon.println(" fail");
  }
  SerialMon.println("Performing HTTP POST request...");
  //String httpRequestData = "{\"trama\":\"hola perras\"}";//coloco el JSON 
  String httpRequestData = "{\"latitude\":1,\"longitude\":2,\"speed\":3}";//coloco el JSON 
  client.print(String("POST ") + resource + " HTTP/1.1\r\n");
  client.print(String("Host: ") + SERVER + "\r\n");
  client.print(String("X-Auth-Token: ") + X_Auth_Token + "\r\n");
  client.println("Connection: close");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(httpRequestData.length());
  client.println();
  client.println(httpRequestData);
  timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    while (client.available()) {
      char c = client.read();
      SerialMon.print(c);
      timeout = millis();
    }
  }
  SerialMon.println();
  client.stop();
//  SerialMon.println(F("Server disconnected"));
//  modem.gprsDisconnect();
//  SerialMon.println(F("GPRS disconnected"));
}
