
#include "j1939_com.h"
FreematicsESP32 sys;
COBD obd;
extern char data[25];
void init_j1939()
{
  // USB serial
 // Serial.begin(115200);

  // initialize co-processor link
  while (!sys.begin());

  // initialize OBD library
  obd.begin(sys.link);

  // start with J1939 protocol
  while (!obd.init(PROTO_J1939));

  Serial.println("J1939 data monitor started");
}

void data_j1939()
{
  byte buf[128];
  // load one received CAN message into buffer
  int bytes = obd.receiveData(buf, sizeof(buf));
  if (bytes > 0) {
    if (bytes < 3) {
      Serial.println("Invalid data");
      return;
    }
    // print timestamp
   // Serial.print('[');
  //  Serial.print(millis());
    //Serial.print("] ");
    // print received CAN message
    uint32_t pgn = (uint32_t)buf[0] << 16 | (uint32_t)buf[1] << 8 | buf[2];
    //Serial.print("PGN:");
    //Serial.print(pgn);
    char dpgn[10] = "";
    sprintf(dpgn,"%d",pgn);
  //  Serial.print(" Data:");
 
    strcat(data, dpgn);
    strcat(data," ");
    //char * data1;
   // for (int n = 3; n < bytes; n++) {
    //  byte d = buf[n];
    //  if (d < 0x10) Serial.print('0');
      //Serial.print(d, HEX);

    //  Serial.print(' ');
   // }

    Serial.println();
    for (int nn = 3; nn < bytes; nn++) {
      byte d = buf[nn];
    char dato[5] = "";
      if (d < 0x10) sprintf(dato,"%02X",0);
  
      sprintf(dato,"%02X",d);
     // Serial.print(dato);
     // Serial.print(' ');
      strcat(data,dato);
      strcat(data," ");
    }
    //Serial.println();
    //Serial.println(data);
    data[24] = '\0';
  }
  
}
