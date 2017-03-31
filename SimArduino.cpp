/*
    SIM Arduino
    Copyright (C) 2017  Ronaldo Rezende Junior (ronaldo.rrj at gmail)
    https://github.com/naldin
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include<stdio.h>
#include<iostream>
#include<windows.h>
#include<time.h>
#include "sharedmemory.h"
#include "SerialClass.h"
#include <math.h>

#define ANGLE 57.30 // Factor radians to degrees
#define UPVAL 583   // 700/1.2 = 583.33
#define DOWNVAL 250 // 300/1.2 = 250
#define DELAYSYNC 20 //sync with Arduino
#define MAP_OBJECT_NAME "$pcars$" //Name of the pCars memory mapped file
HANDLE fileHandle; //Used in waitGame()
Serial *SP; //To initSerial()

using namespace std;

// Print Values on screen
void printValue(string label, float value)
{
    cout.precision(6);
    cout << label << value << endl;
}

// Print RPM VU on screen
void printVu(float vuRpm, float vuMrpm)
{
    int max = 40;
    int set = (int)((vuRpm / vuMrpm) * max);
    string vuShow(" VU RPM [");
    for (int i = 0; i < max; i++)
    {
        if (set > i)
        {
            vuShow += "=";
        }
        else
        {
            vuShow += " ";
        }
    }
    if (set == max)
    {
        vuShow += "]";
    }
    cout << vuShow << endl;
}

// Starting serial
void initserial(){
    string port;
    cout << endl;
    cout << " Plug Arduino in USB and enter" << endl;
    cout << " the value of the Arduino COMx port: ";
	cin >> port;
	system("cls");
	char portc[5];
	strcpy(portc, port.c_str());
	SP = new Serial(portc);
}

// Will waiting until game be open
bool waitGame(){

    // Open the memory-mapped file
    fileHandle = OpenFileMapping( PAGE_READONLY, FALSE, MAP_OBJECT_NAME );
    if (fileHandle == NULL)
    {
        //printf( "Could not open file mapping object %(d).\n", GetLastError() );
        return 0;
    }
    return 1;
}

// Running values on screen and send data to Arduino
int runValues()
{
// *** Memory-mapped game *** //

// Get the data structure
    const SharedMemory *sharedData = (SharedMemory*)MapViewOfFile( fileHandle, PAGE_READONLY, 0, 0, sizeof(SharedMemory) );
    if (sharedData == NULL)
    {
        printf( "Could not map view of file (%d).\n", GetLastError() );

        CloseHandle( fileHandle );
        return 1;
    }

// Ensure we're sync'd to the correct data version
    if ( sharedData->mVersion != SHARED_MEMORY_VERSION )
    {
        printf( "Data version mismatch\n");
        return 1;
    }

// *** End memory-mapped *** //

    // Variables
    char outData[5];
    int brakeVal = 0;
    int val;
    float speed = sharedData->mSpeed;
    float rpm = sharedData->mRpm;
    float maxrpm = sharedData->mMaxRPM;
    float throttle = sharedData->mThrottle;
    float brake = sharedData->mBrake;
    float angularVelocity = sharedData->mAngularVelocity[1];

    // Printing...
    cout << endl;
    printValue(" Speed: ", 3.6*speed);
    printValue(" M_RPM: ", maxrpm);
    printValue(" RPM..: ", rpm);
    printVu(rpm, maxrpm);
    cout << endl;
    printValue(" Throttle: ", throttle);
    printValue(" Brake...: ", brake);
    cout << endl;
    cout << " Angular Velocity in degrees:" << endl;
    printValue(" Y[1]: ", ANGLE*sharedData->mAngularVelocity[1]);
    cout << " Angular Velocity in rad/s:" << endl;
    printValue(" Y[1]: ", sharedData->mAngularVelocity[1]);
    cout << endl;

    //Send rpm to Arduino
    sprintf(outData, "%c%.0f", 'p', ((rpm/maxrpm)*999)*.7);//.7 is 70% of total for the best vision
    SP->WriteData(outData, (unsigned)strlen(outData));
    Sleep(DELAYSYNC);

    //Send speed to Arduino
    val = speed*3.6*3.3; //3.6 is m/s to km/h and 3.3 for max 300km/h
    if (val > 999) val = 999; //Security to max value to not exceed 999.
    sprintf(outData, "%c%d", 's', val); //Load char
    SP->WriteData(outData, (unsigned)strlen(outData));
    Sleep(DELAYSYNC);

    // Getting brake values
    if (brake > 0){
        brakeVal = (exp(brake)*3.6*speed);
    }
    cout << " brakeVal: " << brakeVal << endl;

    // Separating left and right data and send to Arduino
    if (angularVelocity <= 1.2 && angularVelocity > 0) //1.2 is approximately max angular velocity (rad/s) of the F1 car.
    {
        val = (angularVelocity*UPVAL)+DOWNVAL+brakeVal;
        if (val > 999) val = 999; //Security to max value to not exceed 999. Because breakVal adds with Angular Velocity sum.
        sprintf(outData, "%c%d", 'l', val); //Load char
        SP->WriteData(outData, (unsigned)strlen(outData));
        Sleep(DELAYSYNC); //Time to sync with Arduino

        val = ((1.2-angularVelocity)*DOWNVAL)+brakeVal;
        if (val > 999) val = 999;
        sprintf(outData, "%c%d", 'r', val);
        SP->WriteData(outData, (unsigned)strlen(outData));
        brakeVal = 0;
        Sleep(DELAYSYNC); //Time to sync with Arduino

    }
    else if (angularVelocity >= -1.2 && angularVelocity < 0)
    {
        val = (angularVelocity*-UPVAL)+DOWNVAL+brakeVal;
        if (val > 999) val = 999;
        sprintf(outData, "%c%d", 'r', val);
        SP->WriteData(outData, (unsigned)strlen(outData));
        Sleep(DELAYSYNC); //Time to sync with Arduino

        val = ((1.2-angularVelocity*-1)*DOWNVAL)+brakeVal;
        if (val > 999) val = 999;
        sprintf(outData, "%c%d", 'l', val);
        SP->WriteData(outData, (unsigned)strlen(outData));
        brakeVal = 0;
        Sleep(DELAYSYNC); //Time to sync with Arduino

    }
    system("cls");
    return 0;
}

int main ()
{
    initserial();
    // loop
    while(1)
    {
        while (!waitGame()){
            cout << endl;
            cout << " Waiting game...";
            Sleep(100);
            system("cls");
        }
        runValues();
    }
    return 0;
}
