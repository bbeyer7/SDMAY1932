#include <Wire.h>

#define SCL A5
#define SDA A4

#define HIGH_ADD 0x1F
#define LOW_ADD 0x1C

#define CODE_LOAD 0x01
#define CODE 0x02
#define LOAD 0x03

int i = 0;
unsigned int newNote = 0;
unsigned int oldNote = 0;
uint8_t DAC[4] = {0, 0, 0, 0};
char buff[3] {'0', '0', '0'};
int len = 0;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  
  //Get MIDI note. Wants 3 digit number, ex: 069, 120
  Serial.print("MIDI note\n");
  while(i == 0) {
    if(Serial.available() != 0) {
      Serial.readBytesUntil('\n', buff, 3);
      sscanf(buff, "%d", &newNote);
      Serial.print("I received: ");
      Serial.println(newNote, DEC);
      delay(500);
      i++;
    }
  }

  //Checks to see if user pressed a different note
  if (newNote != oldNote) {
    DAC_control(newNote, DAC);
    
    Serial.print("DAC1\n");
    Serial.println(DAC[0], BIN);
    Serial.println(DAC[1], BIN);
    Serial.println(calculateVoltage(DAC[0], DAC[1]), DEC);
    Serial.print("\n");
    Serial.print("DAC2\n");
    Serial.println(DAC[2], BIN);
    Serial.println(DAC[3], BIN);
    Serial.println(calculateVoltage(DAC[2], DAC[3]), DEC);
    Serial.print("\n");
    Serial.print("Total voltage:\n");
    Serial.println(calculateVoltage(DAC[0], DAC[1]) + calculateVoltage(DAC[2], DAC[3]), DEC);
    
    oldNote = newNote;
  }

  else {
    Serial.print("Choose different value\n");
  }
  
  i = 0;

}




//Function to determine DAC input from MIDI note.
//Pass MIDI note (12-120, C0-C9) and array of size 2. 
//DAC_data[0,1] stores data for first DAC (CV of 0 - 5V)
//DAC_data[2,3] stores data for second DAC (CV of 5.0833+)

void DAC_control(unsigned int midiNote, uint8_t *DAC_data) {
  double controlVoltage = 0;
  unsigned int dacVal = 0;
  double vRef = 5;

  if (midiNote > 120 || midiNote < 12) {
    DAC_data[0] = 0;
    DAC_data[1] = 0;
    DAC_data[2] = 0;
    DAC_data[3] = 0;
    return;
  }
  
  controlVoltage = (midiNote / 12.0) - 1;
  dacVal = controlVoltage * 16384 / vRef;
  
  if (dacVal > 16383) {
    //Checks if second DAC is needed
    DAC_data[0] = 255;
    DAC_data[1] = 255;
    DAC_data[2] = (dacVal - 16383) >> 6;
    DAC_data[3] = (dacVal - 16383) << 2;
  }

  else {
    //If second DAC is not needed, only change DAC 1
    //Force DAC 2 to zero
    DAC_data[0] = dacVal >> 6;
    DAC_data[1] = dacVal << 2;
    DAC_data[2] = 0;
    DAC_data[3] = 0;
  }

  return;
  
}


//Send data to DACs
void send2DAC(uint16_t *DAC_data) {
    Wire.beginTransmission(HIGH_ADD);
    Wire.write(CODE_LOAD);
    Wire.write(DAC_data[0]);
    Wire.write(DAC_data[1]);
    Wire.endTransmission();

    Wire.beginTransmission(LOW_ADD);
    Wire.write(CODE_LOAD);
    Wire.write(DAC_data[2]);
    Wire.write(DAC_data[3]);
    Wire.endTransmission();
}

//Function to calculate volatage based on DAC position
//Test bench only
double calculateVoltage(uint8_t x, uint8_t y) {
  double vRef = 5;
  return vRef * ((uint16_t)(x << 6) + (uint16_t)(y >> 2))/ 16384;
}


