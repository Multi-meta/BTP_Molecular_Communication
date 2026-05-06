#define PUMP1_IN1 8 
#define PUMP1_IN2 7 
#define PUMP1_ENA 9 

#define PUMP2_IN3 5 
#define PUMP2_IN4 3
#define PUMP2_ENB 4

#define SALINITY_SENSOR_PIN A0 

const int PROPAGATION_DELAY_MS = 8000; 
const int SENSOR_READ_DELAY_MS = 2000;  
const int FLUSH_DURATION_MS = 10000; 

const float THRESHOLD_0_UPPER = 0.55; 
const float THRESHOLD_1_UPPER = 0.920;  
const float THRESHOLD_2_UPPER = 0.960;   
const  float THRESHOLD_3_UPPER = 1.130;   
const float THRESHOLD_4_UPPER = 1.300;
const float THRESHOLD_5_UPPER = 1.480; 
const float THRESHOLD_6_UPPER = 1.526; 

void setup() { 
Serial.begin(9600); 
Serial.println("Molecular Communication Testbed Initialized (6-Pin Control)."); 
Serial.println("Enter a number (0-7) via Serial Monitor to transmit a symbol."); 
pinMode(PUMP1_IN1, OUTPUT); 
pinMode(PUMP1_IN2, OUTPUT); 
pinMode(PUMP1_ENA, OUTPUT); 
pinMode(PUMP2_IN3, OUTPUT); 
pinMode(PUMP2_IN4, OUTPUT); 
  pinMode(PUMP2_ENB, OUTPUT); 
 
  stopAllPumps(); 
  delay(100); 
} 
 
void loop() { 
  if (Serial.available() > 0) { 
    char inputChar = Serial.read(); 
    if (inputChar >= '0' && inputChar <= '7') { 
      int symbol = inputChar - '0'; 
      transmitAndMeasure(symbol); 
    } else { 
      Serial.println("Invalid input. Please enter a number between 0 and 7."); 
    } 
  } 
} 
 
void transmitAndMeasure(int symbol) { 
  Serial.println("-------------------------"); 
  Serial.print("Transmitting symbol: "); 
  Serial.println(symbol); 
 
  long pump1_duration = 0; 
  long pump2_duration = 0; 
 
  switch (symbol) { 
    case 0: pump1_duration = 10000; pump2_duration = 0; break; 
    case 1: pump1_duration = 7000;   pump2_duration = 3000; break; 
    case 2: pump1_duration = 6000;   pump2_duration = 3000; break; 
    case 3: pump1_duration = 5000;   pump2_duration = 8000; break; 
    case 4: pump1_duration = 5000;   pump2_duration = 9000; break; 
    case 5: pump1_duration = 5000;   pump2_duration = 11000; break; 
    case 6: pump1_duration = 3000;   pump2_duration = 12300; break; 
    case 7: pump1_duration = 0;      pump2_duration = 20000; break; 
    default: 
      Serial.println("Error: Unknown symbol for pump durations."); 
      stopAllPumps(); 
      return; 
  } 
 
  Serial.print("Pump 1 (Neutral Water) ON for: "); 
  Serial.print(pump1_duration); 
  Serial.println(" ms"); 
  Serial.print("Pump 2 (NaCl Solution) ON for: "); 
  Serial.print(pump2_duration); 
  Serial.println(" ms"); 
 
  unsigned long startTime = millis(); 
 
  if (pump1_duration > 0) runPump1(); 
  if (pump2_duration > 0) runPump2(); 
 
  while (millis() - startTime < pump1_duration || millis() - startTime < pump2_duration) { 
    if (millis() - startTime >= pump1_duration) stopPump1(); 
    if (millis() - startTime >= pump2_duration) stopPump2(); 
    delay(1); 
  } 
 
  stopAllPumps();
  Serial.println("Pumps stopped. Waiting for mixed solution to propagate to sensor..."); 
  delay(PROPAGATION_DELAY_MS); 
 
  Serial.println("Reading sensor..."); 
  delay(SENSOR_READ_DELAY_MS); 
 
  int sensorValue = analogRead(SALINITY_SENSOR_PIN); 
  float voltage = sensorValue * (5.0 / 1023.0);
 
  Serial.print("Received Sensor Raw Value: "); 
  Serial.print(sensorValue); 
  Serial.print(" -> Measured Voltage: "); 
  Serial.println(voltage, 3); 

  int decodedSymbol = decodeSymbol(voltage); 
  Serial.print("Decoded Symbol: "); 
  Serial.println(decodedSymbol); 
  Serial.print("Transmitted Symbol: ");
  Serial.println(symbol); 

  if (decodedSymbol == symbol) { 
      Serial.println("Decoding: SUCCESS!"); 
      Serial.println("Hello welcome to BIWIL Lab! Your data transmission is successful."); 
  } else { 
      Serial.println("Decoding: ERROR! (Mismatch)"); 
      Serial.println("Hello welcome to BIWIL Lab! Your data transfer has some error, please check."); 
  } 
 
  flushChannel(); 
  Serial.println("-------------------------\n"); 
  Serial.println("System ready for next symbol. Enter a number (0-7)."); 
} 
 
int decodeSymbol(float voltage) { 
  if (voltage < THRESHOLD_0_UPPER) { 
    return 0; 
  } else if (voltage < THRESHOLD_1_UPPER) { 
    return 1; 
  } else if (voltage < THRESHOLD_2_UPPER) { 
    return 2; 
  } else if (voltage < THRESHOLD_3_UPPER) { 
    return 3; 
  } else if (voltage < THRESHOLD_4_UPPER) { 
    return 4; 
  } else if (voltage < THRESHOLD_5_UPPER) { 
    return 5; 
  } else if (voltage < THRESHOLD_6_UPPER) { 
    return 6; 
  } else { 
    return 7; 
} 
} 
void flushChannel() { 
Serial.println("Flushing channel with neutral water..."); 
runPump1(); 
delay(FLUSH_DURATION_MS); 
stopPump1(); 
Serial.println("Flushing complete."); 
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
stopPump2();    }