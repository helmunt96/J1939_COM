#ifndef sim_7600_h
#define sim_7600_h
#define APN "alai" // Change this to your Provider details
#define gprsUser ""
#define gprsPass ""
#define X_Auth_Token "BBFF-qY0aPc5lETRyrHXb8dKBqVBzxSv8H3"//solo para ubidost
#define SERVER "industrial.api.ubidots.com"// Change this to your selection
#define resource "/api/v1.6/devices/esp32-gps"//PATH
#define PORT 80

#define TINY_GSM_MODEM_SIM7600
#define SerialMon Serial
#if !defined(TINY_GSM_RX_BUFFER)
#define TINY_GSM_RX_BUFFER 650
#endif
#define TINY_GSM_YIELD() { delay(2); }


void init_sim_7600();

void Http_request();

#endif