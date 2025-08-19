#include <wifi_handler.h>
#include <WiFi.h>
// WiFi credentials
static const char* WIFI_SSID = "Magenta-234609_EXT";
static const char* WIFI_PASSWORD = "**************"; // delete before commit

// Global WiFi status
static wifi_status_t wifi_status = {0};

extern "C" {
    void wifi_init(void) {
        // Initialize WiFi in station mode
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        // Update status structure
        strncpy(wifi_status.ssid, WIFI_SSID, sizeof(wifi_status.ssid) - 1);
        wifi_status.reconnectAttempts = 0;
        wifi_status.lastConnectTime = millis();
        
        Serial.print("Connecting to WiFi");
        
        int timeout = 0;
        while (WiFi.status() != WL_CONNECTED && timeout < 20) {
            delay(500);
            Serial.print(".");
            timeout++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            wifi_status.isConnected = true;
            wifi_status.signalStrength = WiFi.RSSI();
            strcpy(wifi_status.ip, WiFi.localIP().toString().c_str());
            
            Serial.println("\nWiFi Connected!");
            Serial.printf("IP Address: %s\n", wifi_status.ip);
            Serial.printf("Signal Strength: %d dBm\n", wifi_status.signalStrength);
        } else {
            wifi_status.isConnected = false;
            Serial.println("\nWiFi Connection Failed!");
        }
    }

    bool wifi_isConnected(void) {
        return wifi_status.isConnected;
    }

    const wifi_status_t* wifi_getStatus(void) {
        if (wifi_status.isConnected) {
            wifi_status.signalStrength = WiFi.RSSI();
        }
        return &wifi_status;
    }

    void wifi_reconnect(void) {
        if (!wifi_status.isConnected) {
            wifi_status.reconnectAttempts++;
            wifi_status.lastConnectTime = millis();
            WiFi.reconnect();
        }
    }
}
