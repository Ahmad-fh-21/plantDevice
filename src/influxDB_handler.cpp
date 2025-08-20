#include <influxDB_handler.h>


// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
// InfluxDB 1.8+  (v2 compatibility API) server url, e.g. http://192.168.1.48:8086
#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"
// InfluxDB v2 server or cloud API authentication token (Use: InfluxDB UI -> Load Data -> Tokens -> <select token>)
// InfluxDB 1.8+ (v2 compatibility API) use form user:password, eg. admin:adminpass
#define INFLUXDB_TOKEN "yy5rxwOjDxmncW-h4VxptGIVa7fiHWVd2Dd3mJ5f4woVa31fxSdO1TsvFUjVxsJJa0y13_VIvVWZeX5TU5B0ug=="
// InfluxDB v2 organization name or id (Use: InfluxDB UI -> Settings -> Profile -> <name under tile> )
// InfluxDB 1.8+ (v2 compatibility API) use any non empty string
#define INFLUXDB_ORG "10e7ac09f1803112"
// InfluxDB v2 bucket name (Use: InfluxDB UI -> Load Data -> Buckets)
// InfluxDB 1.8+ (v2 compatibility API) use database name
#define INFLUXDB_BUCKET "plant Managment Project"

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

#define DEVICE "ESP32_PMP" // Device type, used in query

// Data point for writing
Point sensorData("soil_humidity");

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

extern "C" 
{
    
void InfluxDB_handler_init() 
{
    // Accurate time is necessary for certificate validation
    // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
    // Syncing progress and the time will be printed to Serial
    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

    // check for connection to InfluxDB server
    if (client.validateConnection()) 
    {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
    } 
    else 
    {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
    }

    // Add constant tags to the data point
    sensorData.addTag("device", DEVICE);
    sensorData.addTag("location", "room");
    sensorData.addTag("sensor_type", "capacitive");
}

void InfluxDB_sendData(uint16_t soilHumidity) 
{

    float soilHumidity_precent = (soilHumidity * 100) / 4095; // Convert ADC value to percentage
    // Get WiFi signal strength
    int rssi = WiFi.RSSI();
    Serial.print("WiFi RSSI: ");
    Serial.println(rssi);
    Serial.print("Soil Humidity: ");
    Serial.println(soilHumidity_precent);
    Serial.println("Soil Humidity value:" + String(soilHumidity));


    // Clear previous data
    sensorData.clearFields();
    sensorData.clearTags();
    // Add fields to the data point
    sensorData.addField("moisture_percent", soilHumidity_precent);
    sensorData.addField("raw_value", soilHumidity);
    sensorData.addField("rssi", rssi);

    // Print what we are writing
    Serial.print("Writing: ");
    // Serial.println(client.pointToLineProtocol(sensorData));

    // Write point to InfluxDB
    if (!client.writePoint(sensorData)) 
    {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
    } 
    else 
    {
        Serial.println("Data successfully written to InfluxDB!");
    }

}

}
