//Francisco Alegria

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
int btn_mute = 0;
OSCErrorCode errCode;
float fad_val_old  = 0;
bool  LED_val = 0;

//////////////////////////////////////////////
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
//////////////////////////////////////////////////////
void loop(){
  GUI_IN();
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
    errCode = msgIN.getError();
    Serial.println(errCode);
  }
  delay(5);*/
}

/////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////
void sendNodeOSC(char destOSCAdd[], int dataSend){
  Serial.println("debug:entered sendNodeOSC");
 /* Serial.println(destOSCAdd);
  //Serial.println(charSend);
  Serial.println(dataSend);
  Serial.println(destIpad); // */
  OSCMessage send2node(destOSCAdd);
  send2node.add(dataSend);
  Udp.beginPacket(destIpad,destPort);
  send2node.send(Udp);
  Udp.endPacket();
  send2node.empty();
}
//////////////////////////////////////////////////
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
//////////////////////////////////////////////////////
void mute(OSCMessage &msg, int addrOffset){
  Serial.println("debug:entered mute");
  LED_val = (boolean) msg.getFloat(0);
  if(LED_val){
    Serial.print("ON ");
    Serial.println(LED_val);
  }
  else{
    Serial.print("OFF ");
    Serial.println(LED_val);
  }
  digitalWrite(LED, LED_val);
  sendNodeOSC("/1/label3", LED_val);
  delay(5);
}
//////////////////////////////////////////////////////////////////
void debug_fader(OSCMessage &msg, int addrOffset){
  Serial.println("debug:entered fader");
  float fad_val_new = msg.getFloat(0);
  byte fad_val_b = floor(fad_val_new*255);
  int fad_val_i;
  if( (fad_val_new == fad_val_old) || (abs(fad_val_old - fad_val_new)>5)){
    fad_val_i = floor(fad_val_new*255);
    if(LED_val)
    {
      Wire.beginTransmission(0x28);
      Wire.write(0xAF);
      Wire.write(fad_val_b);
      Wire.endTransmission();
    }
    else
    {
      Wire.beginTransmission(0x28);
      Wire.write(0xA9);
      Wire.write(fad_val_b);
      Wire.endTransmission();
    }
    sendNodeOSC("/1/label", fad_val_i);
  }
  else{
    fad_val_old = fad_val_new;
  }
  Serial.println("Fader input value is: ");
  Serial.print(fad_val_new);
  Serial.print(" , ");
  Serial.print(fad_val_b);
  delay(5);
}
