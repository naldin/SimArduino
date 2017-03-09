/*
    SIM Arduino
    Ronaldo Rezende Junior
    ronaldo.rrj at gmail
    03/2017
    https://github.com/naldin
*/

#include <LiquidCrystal.h>
#include <Servo.h>
#define DELAYSYNC 20 //sync with pc

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo myservoR; //Right servo
Servo myservoL; //Left servo
Servo myservoS; //Speed servo
Servo myservoP; //RPM servo

char readserial;
String strVal;
int intVal;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(6, OUTPUT);  //LCD pin setup
  analogWrite(6, 255); //LCD brightness
  myservoL.attach(7);  //Choice the correct pin
  myservoR.attach(A0); //Choice the correct pin
  myservoS.attach(A1); //Choice the correct pin
  myservoP.attach(A2); //Choice the correct pin

  // Start values to servos
  myservoL.write(118);
  myservoR.write(44);
  myservoS.write(180);
  myservoP.write(180);
}

void loop() {
  if (Serial.available()) {    //wait serial
    switch (Serial.read()) {   //read serial
      case 'l':                //if data start with 'l' (left servo)
        lcd.clear();
        while (Serial.available() > 0) {   //run while has byte in serial
          readserial = Serial.read();      //read each byte
          strVal += readserial;            //load string
          delay(1);
        }
        intVal = (strVal.toInt());
        intVal = map(intVal, 0, 999, 110, 160); //use your values
        myservoL.write(intVal);                 //write to servo
        lcd.setCursor(0, 0);
        lcd.print("L: ");
        lcd.print(strVal);
        strVal = "";          // clear string
        delay(DELAYSYNC);            //sync with pc
        break;

      //same above
      case 'r': //if data start with 'r' (right servo)
        lcd.clear();
        while (Serial.available() > 0) {
          readserial = Serial.read();
          strVal += readserial;
          delay(1);
        }
        intVal = (strVal.toInt());
        intVal = map(intVal, 0, 999, 50, 20);
        myservoR.write(intVal);
        lcd.setCursor(0, 1);
        lcd.print("R: ");
        lcd.print(strVal);
        strVal = "";
        delay(DELAYSYNC);
        break;

      case 's': //if data start with 's' (speed servo)
        lcd.clear();
        while (Serial.available() > 0) {
          readserial = Serial.read();
          strVal += readserial;
          delay(1);
        }
        intVal = (strVal.toInt());
        intVal = map(intVal, 0, 999, 180, 0);
        myservoS.write(intVal);
        lcd.setCursor(8, 0);
        lcd.print("S: ");
        lcd.print(strVal);
        strVal = "";
        delay(DELAYSYNC);
        break;

      case 'p': //if data start with 'p' (rpm servo)
        lcd.clear();
        while (Serial.available() > 0) {
          readserial = Serial.read();
          strVal += readserial;
          delay(1);
        }
        intVal = (strVal.toInt());
        intVal = map(intVal, 0, 999, 180, 0);
        myservoP.write(intVal);
        lcd.setCursor(8, 1);
        lcd.print("P: ");
        lcd.print(strVal);
        strVal = "";
        delay(DELAYSYNC);
        break;
    }
  }
}
