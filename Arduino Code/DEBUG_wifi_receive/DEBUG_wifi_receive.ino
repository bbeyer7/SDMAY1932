#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <Wire.h>

// constants////////////
// I2C
#define SCL 4
#define SDA 5
#define LED 12

char ssid[] = "CenturyLink9713";
char pass[] = "4efda8d46777v3";

WiFiUDP Udp;
const IPAddress destIpad(192,168,0,4); //iPad
//const IPAddress destIphone(192,168,0,3); //iPhone
const unsigned int destPort = 9000;
const unsigned int localPort = 8000;
char incomingPacket[255];
//int preset = 0;     //preset initializes to patch/preset 
int btn_mute = 1;
OSCErrorCode errorerer;

void setup() {
  pinMode(LED, OUTPUT);
 //I2C
 Wire.begin();
 Serial.begin(115200);
 delay(100);
//WIFI
// Specify a static IP address for NodeMCU - only needeed for receiving messages)
    // If you erase this line, your ESP8266 will get a dynamic IP address
    WiFi.config(IPAddress(192,168,0,123),IPAddress(192,168,0,4), IPAddress(255,255,255,0)); 
  
    // Connect to WiFi network255
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" ");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Udp.begin(localPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localPort);
}

void loop(){
  GUI_IN();
  //sendOSCBACK();
  //inputContents();
  
   delay(5);
   /*
  OSCBundle msgIN;
  int size;
  if((size = Udp.parsePacket())>0){
    while(size--)
      msgIN.fill(Udp.read());
    if(!msgIN.hasError()){
        msgIN.route("/1/fader",debug_fader);
        msgIN.route("/1/MUTE", mute);
        Serial.println("no rrerrer");
     // msgIN.route("/1/shift",toggleOnOff);
    }
    else{
      Serial.println("ERROR ERROR ERROR ERROR ERROR ");
    }
  }
  else{
    Serial.println("ERROR ERROR ");
    errorerer = msgIN.getError();
    Serial.println(errorerer);
  }
  delay(5);*/
}

void inputContents(){
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

  }
}
void sendOSCBACK(){
  Serial.println("debug:entered sendback");
  Serial.println(btn_mute);
  OSCMessage msg("/1/label3 ");
  msg.add(btn_mute);
  Udp.beginPacket(destIpad,destPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

void GUI_IN(){
  Serial.println("debug:entered gui_in");
  OSCMessage gui_in;
  int msgSize;
  if((msgSize = Udp.parsePacket())>0){
    while(msgSize--)
      gui_in.fill(Udp.read());
      if(!gui_in.hasError()){
        gui_in.route("/1/fader",debug_fader);
        gui_in.route("/1/MUTE", mute);
      }
  } 
}

void mute(OSCMessage &msg, int addrOffset){
  Serial.println("debug:entered mute");
  bool  LED_val = (boolean) msg.getFloat(0);
  OSCMessage node_out("/1/label3 ");
  digitalWrite(LED, LED_val);
  node_out.add(btn_mute);
  if(LED_val){
    Serial.print("ON ");
    Serial.println(LED_val);
  }
  else{
    Serial.print("OFF ");
    Serial.println(LED_val);
  }
  LED_val = !LED_val;
  btn_mute = !btn_mute;
   node_out.add(btn_mute);
digitalWrite(LED, LED_val);
  Udp.beginPacket(destIpad,destIpad);
  node_out.send(Udp);
  Udp.endPacket();
  node_out.empty();
}

void debug_fader(OSCMessage &msg, int addrOffset){
  Serial.println("debug:entered fader");
  float fad_val = msg.getFloat(0);
  OSCMessage node_out("/1/label");
  byte fad_val_b = floor(fad_val*255);
  Serial.println("Fader value is: ");
  Serial.println(fad_val_b);
  Serial.print(" , ");
  Serial.print(fad_val_b);
  node_out.add(fad_val);
  Udp.beginPacket(Udp.remoteIP(), destIpad);
  node_out.send(Udp);
  Udp.endPacket();
  node_out.empty();
}

byte res_con_1t256(int val){
  Serial.println("debug:entered res_con");
  int digi_write_val;
  byte digi_byte;
  digi_write_val = (254*val)+1;
  digi_byte = (byte) digi_write_val;
  return digi_byte;
}
