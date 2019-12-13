#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "defines.h" // define constants here

ESP8266WiFiMulti WiFiMulti;

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(WIFI_SSID, WIFI_SSID); // Edit passwords in defines.h

    if ((WiFiMulti.run() == WL_CONNECTED))
    {
        Serial.println("Connected");
    }
    else
    {
        Serial.println("Not connected");
    }
}

int readByte = 0;

void loop()
{
    readByte = Serial.read();
    Serial.printf("%c", readByte);
    delay(100);
    if ((WiFiMulti.run() == WL_CONNECTED))
    {
        Serial.println("Wifi Connected");

        // declare http client
        HTTPClient http;
        http.begin("http://172.20.10.3:8090/handler");
        //
        //    // send request
        //    int httpCode = http.GET();
        //    if (httpCode > 0) {
        //      String payload = http.getString();
        //      Serial.println(payload);
        //    } else {
        //      Serial.println("An error occured");
        //    }

        // check conditions and put request
        // get the received signal strength
        long rssi = WiFi.RSSI();
        Serial.print("RSSI : ");
        Serial.println(rssi);

        http.addHeader("Content-Type", "text/plain");
        int httpPostCode;
        if (rssi < -60)
        {
            httpPostCode = http.POST("warning");
        }
        else
        {
            httpPostCode = http.POST("OK");
        }
        String postPayload = http.getString();
        Serial.println(httpPostCode);
        Serial.println(postPayload);
        http.end();
    }
    else
    {
        Serial.println("Not Connected");
    }

    delay(3000);
}
