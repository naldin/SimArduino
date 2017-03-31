/*
    SIM Arduino
    Ronaldo Rezende Junior
    ronaldo.rrj at gmail
    03/2017
    https://github.com/naldin
*/

#include <LiquidCrystal.h>
#include <Servo.h>
#define DELAYSYNC 18 //sync with pc

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

void runVal(int map1, int map2, int lcdC, int lcdR, String lcdLabel) {
  lcd.clear();
  while (Serial.available() > 0) {    //run while has byte in serial
    readserial = Serial.read();;      //read each byte
    strVal += readserial;             //load string
    delay(1);
  }
  intVal = (strVal.toInt());
  intVal = map(intVal, 0, 999, map1, map2); //use your values
  lcd.setCursor(lcdC, lcdR);
  lcd.print(lcdLabel);
  lcd.print(strVal);
}

void loop() {
  if (Serial.available()) {    //wait serial
    switch (Serial.read()) {   //read serial
      case 'l': //if data start with 'l' (left servo)
        runVal(110, 160, 0, 0, "L: ");
        myservoL.write(intVal);  //write to servo
        strVal = "";             // clear string
        delay(DELAYSYNC);        //sync with pc
        break;

      case 'r': //if data start with 'r' (right servo)
        runVal(50, 20, 0, 1, "R: ");
        myservoR.write(intVal);
        strVal = "";
        delay(DELAYSYNC);
        break;

      case 's': //if data start with 's' (speed servo)
        runVal(180, 0, 8, 0, "S: ");
        myservoS.write(intVal);
        strVal = "";
        delay(DELAYSYNC);
        break;

      case 'p': //if data start with 'p' (rpm servo)
        runVal(180, 0, 8, 1, "P: ");
        myservoP.write(intVal);
        strVal = "";
        delay(DELAYSYNC);
        break;
    }
  }
}

