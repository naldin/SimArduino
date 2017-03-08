/*
    SIM Arduino
    Ronaldo Rezende Junior
    ronaldo.rrj at gmail
    03/2017
    https://github.com/naldin
*/

#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo myservoR;
Servo myservoL;

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

  // Start value to servos
  myservoL.write(118);
  myservoR.write(44);
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
        delay(20);            //sync with pc
        break;

      //same above
      case 'r':
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
        delay(20);
        break;
    }
  }
}
