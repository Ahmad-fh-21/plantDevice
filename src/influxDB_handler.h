#ifndef INFLUXDB_HANDLER_H
#define INFLUXDB_HANDLER_H


#include <wifi_handler.h>
#include <WiFi.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#ifdef __cplusplus
extern "C" {
#endif

void InfluxDB_handler_init();
void InfluxDB_sendData(uint16_t soilHumidity);

#ifdef __cplusplus
}
#endif

#endif // INFLUXDB_HANDLER_H