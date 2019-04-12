#include <Wire.h>

#define SCL A5
#define SDA A4

#define HIGH_ADD 0x1F
#define LOW_ADD  0b0011100
#define FLO_ADD  0b0011101

#define CODE_LOAD 0x01

char buff[3] {'0', '0', '0'};
double newNote = 0;
double oldNote = 0;
int i = 0;


void setup() {
  Wire.begin();
  Serial.begin(9600);

}

void loop() {
 
  Serial.print("MIDI note\n");
  while(i == 0) {
    if(Serial.available() != 0) {
      Serial.readBytesUntil('\n', buff, 3);
      sscanf(buff, "%lf", &newNote);
      Serial.print("I received: ");
      Serial.println(newNote, DEC);
      delay(500);
      i++;
    }
  }


    DAC_LFO(255);

    i = 0;

}

void DAC_VCO(double midiNote) {
  unsigned int DAC1_data = 0;
  unsigned int DAC2_data = 0;
  double controlVoltage = 0;
  unsigned int dacVal = 0;
  double vRef = 5;

  //converts MIDI to CV for calculations. Change constant term to change mapping
  controlVoltage = (midiNote / 12.0) - 1;
  
  //Converts CV to DAC code
  dacVal = controlVoltage * 16384 / vRef;

  if (dacVal > 16383) {
    //Checks if second DAC is needed
    DAC1_data = 16383;
    DAC2_data = dacVal - 16383;
  }

  else {
    DAC1_data = dacVal;
    DAC2_data = 0;
  }
  
    //Begin transmission for DAC1
    Wire.beginTransmission(0x1F);
    Wire.write(0x01);
    Wire.write(DAC1_data >> 6);
    Wire.write(DAC1_data & 0x3F);
    Wire.endTransmission();

    //Begin transmission for DAC2
    Wire.beginTransmission(0x1C);
    Wire.write(0x01);
    Wire.write(DAC2_data >> 6);
    Wire.write(DAC2_data & 0x3F);
    Wire.endTransmission();

}

void DAC_PWM(double percent) {

  //Requires calibration
  double controlVoltage = 0;
  double vRef = 5;
  unsigned int DAC1_data = 0;

  controlVoltage = 5.0 * percent / 100.0;
  DAC1_data = controlVoltage * 16384 / vRef;
  
  Wire.beginTransmission(0x1D);
  Wire.write(0x01);
  Wire.write(DAC1_data >> 6);
  Wire.write(DAC1_data & 0x3F);
  Wire.endTransmission();
  
}

void DAC_LFO(unsigned int val) {
  unsigned int DAC1_data = 0;
  unsigned int DAC2_data = 0;
  double controlVoltage = 0;
  unsigned int dacVal = 0;
  double vRef = 5;

  //converts MIDI to CV for calculations. Change constant term to change mapping
  controlVoltage = (9.5 / 255.0) * val;
  
  //Converts CV to DAC code
  dacVal = controlVoltage * 16384 / vRef;

  if (dacVal > 16383) {
    //Checks if second DAC is needed
    DAC1_data = 16383;
    DAC2_data = dacVal - 16383;
  }

  else {
    DAC1_data = dacVal;
    DAC2_data = 0;
  }
  
    //Begin transmission for DAC1
    Wire.beginTransmission(0x1F);
    Wire.write(0x01);
    Wire.write(DAC1_data >> 6);
    Wire.write(DAC1_data & 0x3F);
    Wire.endTransmission();

    //Begin transmission for DAC2
    Wire.beginTransmission(0x1C);
    Wire.write(0x01);
    Wire.write(DAC2_data >> 6);
    Wire.write(DAC2_data & 0x3F);
    Wire.endTransmission();

}

