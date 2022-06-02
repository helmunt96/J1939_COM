#include <Arduino.h>
#include "sim_7600.h"
#include "j1939_com.h"
char data[25] = "";
void setup() {

init_sim_7600();


init_j1939();



  // put your setup code here, to run once:
}

void loop() {

 //data_j1939(); 
//Serial.println('hola perras');
Serial.println(data);
strcpy(data, "");

 Http_request();

//delay(2000);

  // put your main code here, to run repeatedly:
}