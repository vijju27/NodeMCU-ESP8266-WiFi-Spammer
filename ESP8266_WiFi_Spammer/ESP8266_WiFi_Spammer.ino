#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);


extern "C" {
#include "user_interface.h"
}

#define AP_SSID "WTF!"
#define AP_PASSWORD "@lbertPUBG"

void spam_random();
void spam_existing();
void spam_predefined();
void spam_custom();
void existing();
void Random();
void predefined();
void custom();
void evil_mode();

String networks[15] = "";
int available_networks = 0;
unsigned long junk=0;

String characters= "_ qwertyuiopasdfghjkklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM123456@";
byte channel;


byte Mode = 0;
const char homepage[]= "<style>body{background-color:#f0dfa4;}button{background-color:#4fc5b0;color:white;border-color:#4fc5b0;width:200px;height:50px;border-radius:10px;margin:10px;font-weight:bold;}</style><h1>WIFI packet injector Welcomes You</h1><h2>Select mode</h2><a href=\"/existing\"><button>Spam Existing</button></a><br><a href=\"/random\"><button>Spam RandomSSID's</button></a><br><a href=\"/predefined\"><button>predefined</button></a><br><a href=\"/custom\"><button>Custom</button></a><br><a href=\"/evil\"><button>EVIL</button></a>";

String ssids[21] = {
  "No WiFi",
  "You are spammed",
  "You can't find me",
  "No pain No gain",
  "I'm not watching you",
  "WTF",
  "Hack me if you can",
  "This is not Hacking",
  "Free Wi-Fi",
  "Confused?",
  "No free WiFi for you",
  "This is not free either",
  "Test WiFi, please ignore",
  "Ignore this too",
  "Don't Panic Bro",
  "Knowledge is everything",
  "Just for FUN",
  "Have a Great Day",
  "Keep Smiling",
  "You are GREAT",
  "Live Happy Life"
};

String custom_ssid[15] = "";
int parseCounter=0;

// Beacon Packet buffer
//uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00,
//                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff,//Target MAC i.e., BROADCAST
//                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,//AP MAC
//                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,//AP MAC
//                /*22*/  0xc0, 0x6c,
//                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00,
//                /*32*/  0x64, 0x00,
//                /*34*/  0x01, 0x04,
//                      /*Variable SSID LENGTH(below line 0x07) along with SSID NAME(after length byte you need to embed SSID characters)*/
//                /*36*/  0x00, 0x07, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72,0x72,
//                        0x01, 0x08, 0x82, 0x84,
//                        0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01,
//                /*56*/  0x04};//CHANNEL



uint8_t packet_start[10] = { 0x80, 0x00, 0x00, 0x00,
                             /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff
                           };

uint8_t packet_middle[14] = {
  /*22*/  0xc0, 0x6c,
  /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00,
  /*32*/  0x64, 0x00,
  /*34*/  0x01, 0x04
};
/* SSID */

uint8_t packet_ssid[32] = {
  /*36*/  0x00, 0x07, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72
};
uint8_t packet_end[12] = {
  0x01, 0x08, 0x82, 0x84,
  0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01
  /*56*/
};


void setup() {
  delay(5000);
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  selectMode();
  digitalWrite(2,1);
  Serial.println("before while");
  while(Mode == 0){
    server.handleClient();
    delay(10);
  }
  Serial.println("After while");
  digitalWrite(2,0);
  WiFi.mode(WIFI_STA);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(0);
  getNetworks();
  wifi_promiscuous_enable(1);
  Serial.println("setup ok");
}



void loop() {
  if(Mode == 1){
    while(1){
      spam_existing();
    }
  }
  else if(Mode == 2){
    while(1){
      spam_random();
    }
  }
  else if(Mode == 3){
    while(1){
      spam_predefined();
    }
  }
  else if(Mode == 4){
    while(1){
    spam_custom();
    }
  }
  else if(Mode == 5){
    getNetworks();
    while(1){
    evil_mode();
    }
  }
}


void getNetworks() {
  Serial.println(F("scan start"));
  wifi_promiscuous_enable(0);
  available_networks = WiFi.scanNetworks();
  Serial.println(F("scan done"));
  if (available_networks == 0) {
    Serial.println(F("no networks found"));
    return;
  }
  else
  {
    Serial.print(available_networks);
    Serial.println(F(" networks found"));
    for (int i = 0; i < available_networks; i++)
    {
      networks[i] = WiFi.SSID(i);
      //Serial.println(" -> " + WiFi.SSID(i));
      //networks[i] += ' ';
    }
  }
}



void buildPacket(String ssid) {
  byte packetSize = 10 + 14 + 12 + (ssid.length() + 2) + 12 + 1;
  uint8_t packet[packetSize];
  Serial.print("packet size : ");
  Serial.println(packetSize);
  byte i;
  byte count = 0;
  for (i = 0; i < 10; i++) {
    packet[count] = packet_start[i];
    count++;
  }

  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);
  count = count + 12;
  for (i = 0; i < 14; i++) {
    packet[count] = packet_middle[i];
    count++;
  }
  packet[count] = 0x00;
  count++;
  packet[count] = ssid.length();
  count++;
  for (i = 0; i < ssid.length(); i++) {
    packet[count] = ssid[i];
    count++;
  }
  //  packet[count] = characters[random(65)];
  //  count++;
  for (i = 0; i < 12; i++) {
    packet[count] = packet_end[i];
    count++;
  }
  //Serial.print("count : ");
  //Serial.println(count);
  packet[count] = channel;
  //  for(i = 0 ;i<packetSize;i++){
  //    Serial.print(packet[i],HEX);
  //    Serial.print(",");
  //  }
  //  Serial.println("");
  wifi_send_pkt_freedom(packet, packetSize, 0);
  wifi_send_pkt_freedom(packet, packetSize, 0);
  wifi_send_pkt_freedom(packet, packetSize, 0);
  delay(10);
}


//byte getPacketSize(String ssid){
//  byte packetSize = packet_start.length + packet_middle.length + (ssid.length + 2) + packet_end.length;
//  return packetSize;
//}


void spam_random(){
  channel = random(1, 12);
  wifi_set_channel(channel);
  byte junk_length = random(1,15);
    char junk_ssid[junk_length];
    for(int i = 0; i < junk_length; i++){
      junk_ssid[i] = characters[random(65)];
    }
    buildPacket(junk_ssid);
}


void spam_existing(){
  for (int i = 0; i < available_networks; i++) {
    channel = random(1, 12);
    wifi_set_channel(channel);
    for (int j = networks[i].length(); j >= 0; j--) {
      String ssid = networks[i];
      String space = "";
      for (int k = 0; k < j; k++) {
        space += " ";
      }
      ssid = ssid + space;
      buildPacket(ssid);
    }
    //buildPacket(networks[i]);
    Serial.println(networks[i]);
  }
}


void spam_predefined(){
  channel = random(1, 12);
  wifi_set_channel(channel);
  for(int i=0; i<21;i++){
    buildPacket(ssids[i]);
  }
}


void selectMode(){
  wifi_promiscuous_enable(0);
  WiFi.mode(WIFI_AP);
  wifi_set_opmode(SOFTAP_MODE);
  WiFi.softAP(AP_SSID,AP_PASSWORD);
  Serial.println(WiFi.softAPIP());
  server.on("/",root);
  server.on("/existing",existing);
  server.on("/random",Random);
  server.on("/predefined",predefined);
  server.on("/custom",custom);
  server.on("/customval",customval);
  server.on("/evil",evil);
  server.begin();
}


void root(){
  server.send(200,"text/html",homepage);
}


void existing(){
  Mode = 1;
  server.send(200,"text/html","<h1>Spamming existing networks</h1>");
}

void Random(){
  Mode = 2;
  server.send(200,"text/html","<h1>Spamming with Random networks</h1>");
}

void predefined(){
  Mode = 3;
  server.send(200,"text/html","<h1>Spamming with built-in ssid's</h1>");
}

void custom(){
  server.send(200,"text/html","<h1>Enter ssid seperated by ,</h1><form action=\"/customval\"><input type=\"text\" name=\"data\"><button type=\"submit\">Submit</button></form>");
}

void spam_custom(){
  channel = random(1, 12);
  wifi_set_channel(channel);
  for(int i=0; i<parseCounter;i++){
    buildPacket(custom_ssid[i]);
  }
}

void customval(){
  String data = server.arg(data);
  Serial.println(data);
  byte parse_start=0;
    for(int i=0;i<data.length();i++){
      if(data[i]==','){
        custom_ssid[parseCounter] = data.substring(parse_start,i);
        Serial.println(custom_ssid[parseCounter]);
        parse_start=i+1;
        parseCounter++;
      }
    }
    custom_ssid[parseCounter]=data.substring(parse_start,data.length());
    parseCounter++;
    data = "<h1>I got</h1><br>";
    for(int i =0;i<parseCounter;i++){
      data += custom_ssid[i];
      data +="<br>";
    }
    server.send(200,"text/html",data);
    Mode = 4;
    data = "";
}

void evil(){
  server.send(200,"text/html","Evil Mode :(");
  Mode = 5;
}


void evil_mode(){
  int i;
  spam_predefined();
  for(i=0;i<15;i++){
    spam_random();
  }
  spam_existing();
}
