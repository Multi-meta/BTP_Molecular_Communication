#include "thingProperties.h"

#define PUMP1_IN1 D1
#define PUMP1_IN2 D2
#define PUMP1_ENA D4

#define PUMP2_IN3 D5
#define PUMP2_IN4 D6
#define PUMP2_ENB D7

#define SALINITY_SENSOR_PIN A0

const int PROPAGATION_DELAY_MS = 8000;
const int SENSOR_READ_DELAY_MS = 2000;
const int FLUSH_DURATION_MS = 10000;

const float THRESHOLD_0_UPPER = 0.55;
const float THRESHOLD_1_UPPER = 0.920;
const float THRESHOLD_2_UPPER = 0.960;
const float THRESHOLD_3_UPPER = 1.130;
const float THRESHOLD_4_UPPER = 1.300;
const float THRESHOLD_5_UPPER = 1.480;
const float THRESHOLD_6_UPPER = 1.526;

void setup() {
 Serial.begin(9600);
 delay(1500);

 initProperties();
 ArduinoCloud.begin(ArduinoIoTPreferredConnection);

 setDebugMessageLevel(2);
 ArduinoCloud.printDebugInfo();

 pinMode(PUMP1_IN1, OUTPUT);
 pinMode(PUMP1_IN2, OUTPUT);
 pinMode(PUMP1_ENA, OUTPUT);

 pinMode(PUMP2_IN3, OUTPUT);
 pinMode(PUMP2_IN4, OUTPUT);
 pinMode(PUMP2_ENB, OUTPUT);

 stopAllPumps();

 Serial.println("System Ready. Enter 0–7");
}

void loop() {
 ArduinoCloud.update();

 if (Serial.available() > 0) {
 char inputChar = Serial.read();

 if (inputChar >= '0' && inputChar <= '7') {
   int symbol = inputChar - '0';
     transmitAndMeasure(symbol);
  } else {
     Serial.println("Enter number 0–7 only");
    }
  }
}

void transmitAndMeasure(int symbol) {

   Serial.println("-------------------------");
   Serial.print("Transmitting: ");
   Serial.println(symbol);

  long pump1_duration = 0;
  long pump2_duration = 0;

  switch (symbol) {
    case 0: pump1_duration = 10000; pump2_duration = 0; break;
    case 1: pump1_duration = 7000; pump2_duration = 3000; break;
    case 2: pump1_duration = 6000; pump2_duration = 3000; break;
    case 3: pump1_duration = 5000; pump2_duration = 8000; break;
    case 4: pump1_duration = 5000; pump2_duration = 9000; break;
   case 5: pump1_duration = 5000; pump2_duration = 11000; break;
   case 6: pump1_duration = 3000; pump2_duration = 12300; break;
   case 7: pump1_duration = 0; pump2_duration = 20000; break;
  }

 unsigned long startTime = millis();

 if (pump1_duration > 0) runPump1();
 if (pump2_duration > 0) runPump2();

 while (millis() - startTime < pump1_duration || millis() - startTime < pump2_duration) {
   if (millis() - startTime >= pump1_duration) stopPump1();
     if (millis() - startTime >= pump2_duration) stopPump2();
  delay(1);
 }

 stopAllPumps();

 Serial.println("Waiting for propagation...");
 delay(PROPAGATION_DELAY_MS);

 Serial.println("Reading sensor...");
 delay(SENSOR_READ_DELAY_MS);

 int sensorValue = analogRead(SALINITY_SENSOR_PIN);

 float sensorVoltage = sensorValue * (3.3 / 1023.0);

 Serial.print("Voltage: ");
 Serial.println(sensorVoltage, 3);

 int decoded = decodeSymbol(sensorVoltage);

 Serial.print("Decoded: ");
 Serial.println(decoded);

 voltage = sensorVoltage;
 decodedSymbol = decoded;
 transmittedSymbol = symbol;

 if (decoded == symbol) {
   Serial.println("SUCCESS");
 } else {
   Serial.println("ERROR");
}

 flushChannel();

 Serial.println("Ready for next input\n");
}

int decodeSymbol(float voltage) {
 if (voltage < THRESHOLD_0_UPPER) return 0;
 else if (voltage < THRESHOLD_1_UPPER) return 1;
 else if (voltage < THRESHOLD_2_UPPER) return 2;
 else if (voltage < THRESHOLD_3_UPPER) return 3;
 else if (voltage < THRESHOLD_4_UPPER) return 4;
 else if (voltage < THRESHOLD_5_UPPER) return 5;
 else if (voltage < THRESHOLD_6_UPPER) return 6;
 else return 7;
}

void flushChannel() {
 Serial.println("Flushing...");
 runPump1();
 delay(FLUSH_DURATION_MS);
 stopPump1();
}

void runPump1() {
 digitalWrite(PUMP1_IN1, HIGH);
 digitalWrite(PUMP1_IN2, LOW);
 digitalWrite(PUMP1_ENA, HIGH);
}

void runPump2() {
 digitalWrite(PUMP2_IN3, HIGH);
 digitalWrite(PUMP2_IN4, LOW);
 digitalWrite(PUMP2_ENB, HIGH);
}

void stopPump1() {
 digitalWrite(PUMP1_ENA, LOW);
}

void stopPump2() {
 digitalWrite(PUMP2_ENB, LOW);
}

void stopAllPumps() {
 stopPump1();
 stopPump2();
} 