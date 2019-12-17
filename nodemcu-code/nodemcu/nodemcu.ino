// #include <dummy.h>

/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>

#include <MicroGear.h>

#include "defines.h"

#include <strings.h>

#define APPID "testingNetpie"
#define KEY "jUDwDvGMUwiYiRM"
#define SECRET "QarZjg3rMpeh3oBKiIz0LMPmI"
#define DEBUG(code) if (debug) { \
  code\
}
//#define DEBUG(code) 

#define WIFI_SSID "ton-naao"
#define WIFI_PASSWORD "mynameisaim"

//# define WIFI_SSID "LAPTOP-6BIT21R4 7795"
//#define WIFI_PASSWORD "0!fA4660"

# define ALIAS "esp8266"

WiFiClient client;
ESP8266WiFiMulti WiFiMulti;

int timer = 0;
MicroGear microgear(client);

int enableDebugPin = 0; // D3 Pin
int debug = 0;
int cmd;
bool fall_on_status_before = false;
bool fall_on_status_now = false;
bool range_on_status_before = false;
bool range_on_status_now = false;

// BIG TODO: เอา print debug ออกเพราะมันส่งไปบอร์ดจริงๆด้วย

/* If a new message arrives, do this */
void onMsghandler(char * topic, uint8_t * msg, unsigned int msglen) {
  // TODO: check topic and handle message
  char * m = "onMsghandler";
  msg[msglen] = 0;
  DEBUG(Serial.printf("%s : %s\n", topic, msg);)

  //เปิด / ปิด fall detect
  if (strcmp(topic, "/testingNetpie/fall/command") == 0) {
    m = (char * ) msg;
    m[msglen] = '\0';
    if (strcmp(m, "ON") == 0) Serial.write("F");
    if (strcmp(m, "OFF") == 0) Serial.write("f");
    if (strcmp(m, "RESET") == 0) Serial.write("R");
  }

/*เปิด / ปิด area checking ** เทียบว่าอันแรกเป็นไงเทียบกับอันสอง*/
  if (strcmp(topic, "/testingNetpie/out-of-range/command") == 0) {
    DEBUG(Serial.println("/testingNetpie/out-of-range/command is detected !\n");)
    m = (char * ) msg;
    m[msglen] = '\0';
    if (strcmp(m, "ON") == 0) {
      Serial.write("O");
      DEBUG(Serial.printf("area checking:on\n");)
      
    }
    if (strcmp(m, "OFF") == 0) {
      Serial.write("o");
      DEBUG(Serial.printf("area checking:off\n");)
      delay(100);
    }
    if (strcmp(m, "RESET") == 0) Serial.write("r");
  }

  //DEBUG(Se  rial.print("Incoming message --> ");)
  //DEBUG(Serial.printf("%s :", topic);)
  //msg[msglen] = '\0';
  //DEBUG(Serial.println((char *)msg);)

}

void onFoundgear(char * attribute, uint8_t * msg, unsigned int msglen) {
  //Serial.print("Found new member --> ");
  //for (int i=0; i<msglen; i++)
  //Serial.print((char)msg[i]);
  //Serial.println();  
}

void onLostgear(char * attribute, uint8_t * msg, unsigned int msglen) {
  //Serial.print("Lost member --> ");
  //for (int i=0; i<msglen; i++)
  // Serial.print((char)msg[i]);
  //Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char * attribute, uint8_t * msg, unsigned int msglen) {
  //Serial.println("Connected to NETPIE...");
  /* Set the alias of this microgear ALIAS */
  microgear.subscribe("/fall/command"); // TODO: subscribe to more topic 
  microgear.subscribe("/out-of-range/command");
  microgear.subscribe("/status/command");
  microgear.setAlias(ALIAS);
}

void setup() {
  /* Add Event listeners */
  /* Call onMsghandler() when new message arraives */
  debug = digitalRead(enableDebugPin);
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  DEBUG(Serial.println("Starting...");)
  //    WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  /* Initial WIFI, this is just a basic method to configure WIFI on ESP8266.                       */
  /* You may want to use other method that is more complicated, but provide better user experience */
  if (WiFi.begin(WIFI_SSID, WIFI_PASSWORD)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      DEBUG(Serial.print(".");)
    }
  }

  DEBUG(Serial.println("WiFi connected");)
  // Serial.println("IP address: ");
  // Serial.println(WiFi.``localIP());

  microgear.on(MESSAGE, onMsghandler);

  /* Call onFoundgear() when new gear appear */
  microgear.on(PRESENT, onFoundgear);

  /* Call onLostgear() when some gear goes offline */
  microgear.on(ABSENT, onLostgear);

  /* Call onConnected() when NETPIE connection is established */
  microgear.on(CONNECTED, onConnected);
  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY, SECRET, ALIAS);

  /* connect to NETPIE to a specific APPID */
  microgear.connect(APPID);
}


void loop() {
  debug = digitalRead(enableDebugPin);
  cmd = Serial.read();
  DEBUG(switch (cmd) {
        case 'F': Serial.println("Fall"); break; 
        case 'f': Serial.println("Not Fall"); break;
        case 'E': Serial.println("Error"); break;
        case 'e': Serial.println("Not error"); break;
        case 'S': 
            fall_on_status_now = true;
            if(fall_on_status_before!=fall_on_status_now){
                Serial.println("Status Fall detector : ON"); 
                microgear.publish("/status/command", "fall_on");
                fall_on_status_before=fall_on_status_now;
            }
            break;
        case 's': 
            fall_on_status_now = false;
            if(fall_on_status_before!=fall_on_status_now){
                Serial.println("Status Fall detector : OFF"); 
                microgear.publish("/status/command", "fall_off");
                fall_on_status_before=fall_on_status_now;
            }
            break;
        case 'U': 
            range_on_status_now = true;
            if(range_on_status_before!=range_on_status_now){
                Serial.println("Status Out of range : ON"); 
                microgear.publish("/status/command","out_on");
                range_on_status_before=range_on_status_now;
            }
            break;
        case 'u': 
            range_on_status_now = false;
            if(range_on_status_before!=range_on_status_now){
                Serial.println("Status Out of range : OFF"); 
                microgear.publish("/status/command","out_off");
                range_on_status_before=range_on_status_now;
            }
            break;
      })

  
long rssi = WiFi.RSSI();

  /* To check if the microgear is still connected */
  if (microgear.connected()) {
    DEBUG( Serial.println("connected"); )
  
    /* Call this method regularly otherwise the connection may be lost */
    microgear.loop();

    if (timer >= 1000) {
      DEBUG(Serial.println(rssi);)
      if (rssi < -80) {
        microgear.publish("/range", "out");
        Serial.write("X");
      } else {
        microgear.publish("/range", "in");
        Serial.write("x");
      }

      // user status -> HTML
      if (cmd == 'F') {
        microgear.publish("/fall", "fall");
      } else if (cmd == 'f') {
        microgear.publish("/fall", "ok");
      }

      // board status -> HTML
      if (cmd == 'E') {
        microgear.publish("/board", "error");
      } else if (cmd == 'e') {
        microgear.publish("/board", "fine");
      }
      /* Chat with the microgear named ALIAS which is myself */
      microgear.chat(ALIAS, "Hello");
      timer = 0;
    } else timer += 100;
  } else {
    DEBUG( Serial.println("connection lost, reconnect..."); )
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    } else timer += 100;
  }
  delay(100);
}
