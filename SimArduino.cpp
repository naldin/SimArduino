/*
    SIM Arduino
    Copyright (C) 2017  Ronaldo Rezende Junior (ronaldo.rrj at gmail)
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

#define MAP_OBJECT_NAME "$pcars$" //Name of the pCars memory mapped file
#define ANGLE 57.30 //Factor radians to degrees
#define UPVAL 355   //555 + DOWNVAL(197) + breakVal(100) below = 852 (value to Arduino)
#define DOWNVAL 197

using namespace std;

// Serial Initiate
char port[] = "COM5";
Serial *SP = new Serial(port);

// Print Values on screen
void printValue(string label, float value)
{
    cout.precision(6);
    cout << label << value << endl;
}

// Print RMP VU on screen
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

// Running values on screen and send data to Arduino
int runValues()
{
// *** Memory-mapped game ***

// Open the memory-mapped file
    HANDLE fileHandle = OpenFileMapping( PAGE_READONLY, FALSE, MAP_OBJECT_NAME );
    if (fileHandle == NULL)
    {
        printf( "Could not open file mapping object %(d).\n", GetLastError() );
        return 1;
    }
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

// *** End memory-mapped ***
    // Variables
    char outData[5];
    char intChr[4];
    int brakeVal;

    float speed = sharedData->mSpeed;
    float rpm = sharedData->mRpm;
    float maxrpm = sharedData->mMaxRPM;
    float throttle = sharedData->mThrottle;
    float brake = sharedData->mBrake;
    float angularVelocity = sharedData->mAngularVelocity[1];

    // Printing...
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

    if (brake > 0){
        brakeVal = (exp(brake)*3.6*speed)/2.1;
    }
    else{
        brakeVal = 100;
    }
    cout << " brakeVal: " << brakeVal << endl;
    // Separating left and right data send data to Arduino
    if (angularVelocity <= 1.2 && angularVelocity > 0) //1.2 is approximately max angular velocity (rad/s) of the F1 car
    {
        sprintf(outData, "%c%.0f", 'l', (angularVelocity*UPVAL)+DOWNVAL+brakeVal); //Max Sum = 852 to Arduino
        SP->WriteData(outData, (unsigned)strlen(outData));
        brakeVal = 0;
        Sleep(60); //Same time with Arduino to sync

        sprintf(outData, "%c%.0f", 'r', (1.2-angularVelocity)*DOWNVAL);
        SP->WriteData(outData, (unsigned)strlen(outData));
        Sleep(60); //Same time with Arduino to sync

    }
    else if (angularVelocity >= -1.2 && angularVelocity < 0)
    {
        sprintf(outData, "%c%.0f", 'r', (angularVelocity*-UPVAL)+DOWNVAL+brakeVal); //Max Sum = 852 to Arduino
        SP->WriteData(outData, (unsigned)strlen(outData));
        brakeVal = 0;
        Sleep(60); //Same time with Arduino to sync

        sprintf(outData, "%c%.0f", 'l', (1.2-(angularVelocity*-1))*DOWNVAL);

        SP->WriteData(outData, (unsigned)strlen(outData));
        Sleep(60); //Same time with Arduino to sync

    }
    system("cls");
    return 0;
}

int main ()
{
    // loop
    while(1)
    {
        runValues();
    }
    return 0;
}
