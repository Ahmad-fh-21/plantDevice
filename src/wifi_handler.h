#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>

#ifdef __cplusplus
extern "C" {
#endif

// WiFi connection status structure
typedef struct {
    bool isConnected;
    uint8_t signalStrength;
    char ssid[32];
    char ip[16];
    uint32_t lastConnectTime;
    uint16_t reconnectAttempts;
} wifi_status_t;


void wifi_init(void);
bool wifi_isConnected(void);
const wifi_status_t* wifi_getStatus(void);
void wifi_reconnect(void);



#ifdef __cplusplus
}
#endif

#endif // WIFI_HANDLER_H