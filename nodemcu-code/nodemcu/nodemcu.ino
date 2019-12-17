<<<<<<< HEAD
// #include <dummy.h>
||||||| merged common ancestors
<<<<<<< HEAD
/*#include <dummy.h>*/
=======
// #include <dummy.h>
>>>>>>> 339d3a40f4026c4a0351a644349802acf8dd3830
=======
#include <dummy.h>
>>>>>>> 80eadab2724fb4bac303fbc134e7fbb555efa4b0

/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <MicroGear.h>
#include "defines.h"
#include <strings.h>

#define APPID "testingNetpie"
#define KEY "Y6ynR78mqQBIlSj"
#define SECRET "wX8up1DQXlQ413t3glkRj5ehI"
//#define DEBUG(code) code
#define DEBUG(code) 

#define WIFI_SSID "ton-naao"
#define WIFI_PASSWORD "mynameisaim"
  
#define ALIAS   "esp8266"

WiFiClient client;
ESP8266WiFiMulti WiFiMulti;

int timer = 0;
MicroGear microgear(client);

// BIG TODO: เอา print debug ออกเพราะมันส่งไปบอร์ดจริงๆด้วย

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    // TODO: check topic and handle message
    char* m="onMsghandler";
    //Serial.printf("%s : %s\n",m,topic);
    
    if (strcmp(topic, "/testingNetpie/fall/command") == 0) { // prefix the topic name with /testingNetpie/
      // TODO: check message and send command to device
      Serial.write("f");
      DEBUG(Serial.println("Test\n");)
    }
    
    //เปิด / ปิด fall detect
    if (strcmp(topic, "/testingNetpie/fall/command") == 0) {
      m = (char*)msg;
      m[msglen] = '\0';
      if (strcmp(m, "ON") == 0) Serial.write("F");
      if (strcmp(m, "OFF") == 0) Serial.write("f");
      if (strcmp(m, "RESET") == 0) Serial.write("R");
    }

    //เปิด / ปิด area checking ** เทียบว่าอันแรกเป็นไงเทียบกับอันสอง
    if (strcmp(topic, "/testingNetpie/out-of-range/command") == 0) {
      DEBUG(Serial.println("/testingNetpie/out-of-range/command is detected !\n");)
      m = (char*)msg;
      m[msglen]='\0';
      if (strcmp(m, "ON")==0){ 
//        Serial.printf(t);
        Serial.write("O");
        DEBUG(Serial.printf("area checking:on\n");)
      }
      if (strcmp(m, "OFF")==0){ 
        Serial.write("o");
        DEBUG(Serial.printf("area checking:off\n");)
        delay(100);
        }
      if (strcmp(m, "RESET")==0) Serial.write("r");
    }

   
     DEBUG(Serial.print("Incoming message --> ");)
    DEBUG(Serial.printf("%s :", topic);)
    //msg[msglen] = '\0';
    DEBUG(Serial.println((char *)msg);)
  
    
    
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
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    Serial.println("Starting...");
    //WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

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

    microgear.on(MESSAGE,onMsghandler);

    /* Call onFoundgear() when new gear appear */
    microgear.on(PRESENT,onFoundgear);

    /* Call onLostgear() when some gear goes offline */
    microgear.on(ABSENT,onLostgear);

    /* Call onConnected() when NETPIE connection is established */
    microgear.on(CONNECTED,onConnected);
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
    }
    // คนกำลังเดินออกจากพื้นที่ -> HTML
    long rssi = WiFi.RSSI();
    
    /* To check if the microgear is still connected */
    if (microgear.connected()) {
        //Serial.println("connected");

        /* Call this method regularly otherwise the connection may be lost */
        microgear.loop();

        if (timer >= 1000) {
            Serial.println("Publish...");
            if( rssi < -80 ) {
      microgear.publish("/range","out");
    }
    else {
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
