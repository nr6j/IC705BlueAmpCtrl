/* This code is written by
 *  Dennis Engdahl
 *  NR6J
 *  engdahl@snowcrest.net
 *  
 *  It is written specifically for the Arduino Nano 33 Iot
 *  board, and expects the SerialToSerialBT example to be
 *  loaded into the Nano's ESP32 computer.
 *  
 *  Some of the code below is used from Mooneer Salem's
 *  RlrcraftToYaesuBandData.ino routine.
 *  
 *  11/23/2020
 */
 
/* Begin user configurable settings. */
#define RADIO_BAUD 115200
#define COMPUTER_BAUD 115200

/* Change for your country. Defaults to US band plan. */
#define BAND_MIN_160M 1800000
#define BAND_MAX_160M 2000000

#define BAND_MIN_80M 3500000
#define BAND_MAX_80M 4000000

/* FT-817/818 does not define a specific 60m voltage, so you may want
 * to update the frequency ranges below to output e.g. 40M's instead.
 */
//#define BAND_MIN_60M 5330000
//#define BAND_MAX_60M 5407000

#define BAND_MIN_40M 7000000
#define BAND_MAX_40M 7300000

#define BAND_MIN_30M 10000000
#define BAND_MAX_30M 10150000

#define BAND_MIN_20M 14000000
#define BAND_MAX_20M 14350000

#define BAND_MIN_17M 18068000
#define BAND_MAX_17M 18168000

#define BAND_MIN_15M 21000000
#define BAND_MAX_15M 21450000

#define BAND_MIN_12M 24930000
#define BAND_MAX_12M 24990000

#define BAND_MIN_10M 28000000
#define BAND_MAX_10M 29700000

#define BAND_MIN_6M 50000000
#define BAND_MAX_6M 54000000

/* End user configurable settings. */
#define HZ_TO_KHZ 1000
#define COMMAND_TERMINATOR 0xfd
// #define TRANSCEIVER_INFORMATION_RSP "0xfe0xfe"
#define SERIAL_TIMEOUT_MS 1

#define ANALOG_PIN A0 /* MKR Zero pure analog pin */
#define VOLTAGE_STANDARD 330 /* 3.3V; 5V would be 500 */
#define MAX_INT_FOR_VOLTAGE_STANDARD 1023 /* 10 binary ones is 1023 */

#define VOLTAGE_160M  33
#define VOLTAGE_80M   66
//#define VOLTAGE_60M  
#define VOLTAGE_40M  100
#define VOLTAGE_30M  130
#define VOLTAGE_20M  160
#define VOLTAGE_17M  200
#define VOLTAGE_15M  230
#define VOLTAGE_12M  270
#define VOLTAGE_10M  300
#define VOLTAGE_6M   330

static int count = 0;
static int cntr = 0;
static unsigned long timer = 0;
static unsigned long oldfrequency = -1;
static unsigned int oldptt = -1;
static int state = 0;

// Converts a voltage to a value suitable for analogWrite().
int voltageToInteger(int voltage)
{
  return voltage * MAX_INT_FOR_VOLTAGE_STANDARD / VOLTAGE_STANDARD;
}

int frequencyToVoltage(long frequency)
{
  if (frequency >= BAND_MIN_160M && frequency <= BAND_MAX_160M)
  {
    return VOLTAGE_160M;
  }
  else if (frequency >= BAND_MIN_80M && frequency <= BAND_MAX_80M)
  {
    return VOLTAGE_80M;
  }
  /*else if (frequencyKhz >= BAND_MIN_KHZ_60M && frequencyKhz <= BAND_MAX_KHZ_60M)
  {
    return VOLTAGE_60M;
  }*/
  else if (frequency >= BAND_MIN_40M && frequency <= BAND_MAX_40M)
  {
    return VOLTAGE_40M;
  }
  else if (frequency >= BAND_MIN_30M && frequency <= BAND_MAX_30M)
  {
    return VOLTAGE_30M;
  }
  else if (frequency >= BAND_MIN_20M && frequency <= BAND_MAX_20M)
  {
    return VOLTAGE_20M;
  }
  else if (frequency >= BAND_MIN_17M && frequency <= BAND_MAX_17M)
  {
    return VOLTAGE_17M;
  }
  else if (frequency >= BAND_MIN_15M && frequency <= BAND_MAX_15M)
  {
    return VOLTAGE_15M;
  }
  else if (frequency >= BAND_MIN_12M && frequency <= BAND_MAX_12M)
  {
    return VOLTAGE_12M;
  }
  else if (frequency >= BAND_MIN_10M && frequency <= BAND_MAX_10M)
  {
    return VOLTAGE_10M;
  }
  else if (frequency >= BAND_MIN_6M && frequency <= BAND_MAX_6M)
  {
    return VOLTAGE_6M;
  }
  else
  {
    return 0; // disable band output
  }
}

void setBand(long frequency)
{
  // Serial.print(F("Set Voltage for Frequency ")); Serial.println(frequency);
  Serial.print(F("Voltage is ")); Serial.println(frequencyToVoltage(frequency));
  analogWriteResolution(10);
  analogWrite(ANALOG_PIN, voltageToInteger(frequencyToVoltage(frequency)));
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(NINA_GPIO0, OUTPUT);
  digitalWrite(NINA_GPIO0, HIGH);
  pinMode(NINA_RESETN, OUTPUT);
  digitalWrite(NINA_RESETN, HIGH);
  
  // Open serial ports
  Serial.begin(COMPUTER_BAUD);
  // Serial.setTimeout(SERIAL_TIMEOUT_MS);
  SerialNina.begin(RADIO_BAUD);
  // SerialBT.begin("RadioOut");
  // Serial1.setTimeout(SERIAL_TIMEOUT_MS);

  // Set analog output pin to 0V.
  setBand(0);

  count = 0;
  timer = millis();
  oldfrequency = -1;
  oldptt = -1;
  state=0;
}
  
void loop() 
{
  static String computerBuffer;
  static unsigned char radioBuffer[64];

  static bool doneReceiving = false;
  static char buf[20];
  static unsigned char character;
  static int i;
  static int j;
  static long mult;
  static long frequency;
  static int ptt;
  static unsigned char freqcmd[6] = {0xfe,0xfe,0xa4,0xe0,0x03,0xfd};
  static unsigned char pttcmd[7] = {0xfe,0xfe,0xa4,0xe0,0x1c,0x00,0xfd};

  while (SerialNina.available() > 0)
    {
    character = SerialNina.read();
    // sprintf(buf, "%02X", character);
    // Serial.print(buf);
    if (character == COMMAND_TERMINATOR)
      {
      // Serial.println();
      radioBuffer[count++] = character;
      j=count;
      count=0;

      if ((radioBuffer[2] == 0xe0) && (radioBuffer[3] == 0xa4)) {
        // Serial.print("\r\nLine: ");
        // print the result
        // for (i=0 ; i < j ; i++) {
        //   sprintf(buf, "%02X", radioBuffer[i]);
        //   Serial.print(buf);
        // }
        // Serial.println();

        // At his point, we have a command in the buffer - process it
        if (radioBuffer[4] == 0x03) {
          mult = 1;
          frequency = 0;
          for(i = 5 ; i < 10 ; i++) {
            frequency += (radioBuffer[i] & 0xf) * mult;
            frequency += ((radioBuffer[i] >> 4) & 0xf) * mult * 10;
            mult *= 100;
          }
        timer = millis();
        state = 2;
        if (frequency != oldfrequency) {
          oldfrequency = frequency;
          Serial.print("Frequency = "); Serial.println(frequency);
          setBand(frequency);
          }
        }
        if ((radioBuffer[4] == 0x1c) && (radioBuffer[5] == 0x00)) 
          {
          ptt = radioBuffer[6];
          timer = millis();
          state = 0;
          if (ptt != oldptt) {
            oldptt = ptt;
            Serial.print("PTT = "); Serial.println(radioBuffer[6]);
            if (ptt == 1) {
              digitalWrite(LED_BUILTIN, HIGH);
            } else {
              digitalWrite(LED_BUILTIN, LOW);
            }
          }
        }
      }
    }
    else
    {
      radioBuffer[count++] = character;
    }
  }
  if (millis() - timer > 5000) {
    timer = millis();
    state = 0;
    }
  if (state == 0) {
    SerialNina.write(freqcmd, 6);
    timer = millis();
    state = 1;
    }
  if (state == 2) {
    SerialNina.write(pttcmd, 7);
    timer = millis();
    state = 3;
    }
}
