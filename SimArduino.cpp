#include<stdio.h>
#include<iostream>
#include<windows.h>
#include<time.h>
#include "sharedmemory.h"
#include "SerialClass.h"
// Name of the pCars memory mapped file
// Nome para memoria mapeada
#define MAP_OBJECT_NAME "$pcars$"
#define ANGLE 57.30 //Valor de conversão radianos para graus

using namespace std;

// Serial Initiate
char port[] = "COM5";
Serial *SP = new Serial(port);

void printValue(string label, float value)
{
    cout.precision(6);
    cout << label << value << endl;
}

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

int runValues()
{
// *** Memory-mapped game ***  *** Mapeando a memoria do jogo ***

// Open the memory-mapped file
// Abrindo o arquivo de memoria mapeada do jogo
    HANDLE fileHandle = OpenFileMapping( PAGE_READONLY, FALSE, MAP_OBJECT_NAME );
    if (fileHandle == NULL)
    {
        printf( "Could not open file mapping object %(d).\n", GetLastError() );
        return 1;
    }

// Get the data structure
// Buscando estrutura de dados
    const SharedMemory *sharedData = (SharedMemory*)MapViewOfFile( fileHandle, PAGE_READONLY, 0, 0, sizeof(SharedMemory) );
    if (sharedData == NULL)
    {
        printf( "Could not map view of file (%d).\n", GetLastError() );

        CloseHandle( fileHandle );
        return 1;
    }

// Ensure we're sync'd to the correct data version
// Verificando se a versão está  correta
    if ( sharedData->mVersion != SHARED_MEMORY_VERSION )
    {
        printf( "Data version mismatch\n");
        return 1;
    }

// *** End memory-mapped ***
// *** Fim do mapeamento de memoria ***

    // Variables
    char outData[5];
    char intChr[4];

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
    cout << " Velocidade angular em graus: " << endl;
    printValue(" Y[1]: ", ANGLE*sharedData->mAngularVelocity[1]);
    cout << endl;

    // Send data to Arduino
    // Enviando dados para Arduino
    if (angularVelocity > 0)
    {
        sprintf(intChr, "%.0f", angularVelocity*820);
        sprintf(outData, "%c", 'l');
        strcat(outData, intChr);
        SP->WriteData(outData, (unsigned)strlen(outData));

    }
    else if (angularVelocity < 0)
    {
        sprintf(intChr, "%.0f", angularVelocity*-820);
        sprintf(outData, "%c", 'r');
        strcat(outData, intChr);
        SP->WriteData(outData, (unsigned)strlen(outData));
    }
    Sleep(5);
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
