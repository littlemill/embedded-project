/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "defines.h"
#include <strings.h>

#define APPID "testingNetpie"
#define KEY "Y6ynR78mqQBIlSj"
#define SECRET "wX8up1DQXlQ413t3glkRj5ehI"

#define WIFI_SSID "ton-naao"
#define WIFI_PASSWORD "mynameisaim"
  
#define ALIAS   "esp8266"

WiFiClient client;

int timer = 0;
MicroGear microgear(client);

// BIG TODO: เอา print debug ออกเพราะมันส่งไปบอร์ดจริงๆด้วย

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {

    // TODO: check topic and handle message
    char* m
    
    if (strcmp(topic, "/testingNetpie/fall/command") == 0) { // prefix the topic name with /testingNetpie/
      // TODO: check message and send command to device
      Serial.write("f");
    }
    if (strcmp(topic, "/testingNetpie/fall/command") == 0) {
      m = (char*)msg;
      if (strcmp(m, "ON")) Serial.write("F");
      if (strcmp(m, "OFF")) Serial.write("f");
      if (strcmp(m, "RESET")) Serial.write("R");
    }
    if (strcmp(topic, "/testingNetpie/out-of-range/command") == 0) {
      m = (char*)msg;
      if (strcmp(m, "ON")) Serial.write("O");
      if (strcmp(m, "OFF")) Serial.write("o");
      if (strcmp(m, "RESET")) Serial.write("r");
    }
    
    Serial.print("Incoming message --> ");
    Serial.printf("%s :", topic);
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Found new member --> ");
    for (int i=0; i<msglen; i++)
        Serial.print((char)msg[i]);
    Serial.println();  
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Lost member --> ");
    for (int i=0; i<msglen; i++)
        Serial.print((char)msg[i]);
    Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    /* Set the alias of this microgear ALIAS */
    microgear.subscribe("/fall/command"); // TODO: subscribe to more topic 
    microgear.subscribe("/out-of-range/command");
    microgear.subscribe("/status/command");
    microgear.setAlias(ALIAS);
}


void setup() {
    /* Add Event listeners */
    /* Call onMsghandler() when new message arraives */
    microgear.on(MESSAGE,onMsghandler);

    /* Call onFoundgear() when new gear appear */
    microgear.on(PRESENT,onFoundgear);

    /* Call onLostgear() when some gear goes offline */
    microgear.on(ABSENT,onLostgear);

    /* Call onConnected() when NETPIE connection is established */
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    /* Initial WIFI, this is just a basic method to configure WIFI on ESP8266.                       */
    /* You may want to use other method that is more complicated, but provide better user experience */
    if (WiFi.begin(WIFI_SSID, WIFI_PASSWORD)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    /* Initial with KEY, SECRET and also set the ALIAS here */
    microgear.init(KEY,SECRET,ALIAS);

    /* connect to NETPIE to a specific APPID */
    microgear.connect(APPID);
}

int cmd;
void loop() {
    // read command from MCU TODO: handle more command
    cmd = Serial.read();
    switch (cmd) { 
    case 'F': Serial.println("Fall"); break; // send to NETPIE ?
    case 'f': Serial.println("Not Fall"); break;
    case 'E': Serial.println("Error"); break;
    case 'e': Serial.println("Not error"); break;
    case 'S': Serial.println("Status Fall detector : ON"); {
      microgear.publish("/status/commard","fall_on");
      break;
    }
    case 's': Serial.println("Status Fall detector : OFF"); {
      microgear.publish("/status/commard","fall_off");
      break;
    }
    case 'U': Serial.println("Status Out of range : ON"); {
      microgear.publish("/status/commard","out_on");
      break;
    }
    case 'u': Serial.println("Status Out of range : OFF"); {
      microgear.publish("/status/commard","out_off");
      break;
    }
    }

    // คนกำลังเดินออกจากพื้นที่ -> HTML
    long rssi = WiFi.RSSI();
    if( rssi < -80 ) {
      Serial.write("X");
      microgear.publish("/range","out");
    }
    else {
      Serial.write("x");
      microgear.publish("/range","in");
    }

    // user status -> HTML
    if( cmd == 'F' ) {
      microgear.publish("/fall","fall");
    }
    else if( cmd == 'f' ) {
      microgear.publish("/fall","ok");
    }

    // board status -> HTML
    if( cmd == 'E' ) {
      microgear.publish("/board","error");
    }
    else if( cmd == 'e' ) {
      microgear.publish("/board","fine");
    }
    
    /* To check if the microgear is still connected */
    if (microgear.connected()) {
        // Serial.println("connected");

        /* Call this method regularly otherwise the connection may be lost */
        microgear.loop();

        if (timer >= 1000) {
            Serial.println("Publish...");

            /* Chat with the microgear named ALIAS which is myself */
            microgear.chat(ALIAS,"Hello");
            timer = 0;
        } 
        else timer += 100;
    }
    else {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5000) {
            microgear.connect(APPID);
            timer = 0;
        }
        else timer += 100;
    }
    delay(100);
}
