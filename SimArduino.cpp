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

// Inicializando serial
char port[] = "COM5";
Serial *SP = new Serial(port);
//char outData[]= "1234";
char outData[5];
char intChr[4];
//char dirChr[5];

void printValue(string label, float value){
	cout.precision(6);
	cout << label << value << endl;
	}

void printVu(float vuRpm, float vuMrpm){
	int max = 40;
	int set = (int)((vuRpm / vuMrpm) * max);
	string vuShow(" VU RPM [");
	for (int i = 0; i < max; i++){
		if (set > i){
			vuShow += "=";
		}
		else{
			vuShow += " ";
		}
	}
	if (set == max){
		vuShow += "]";
	}
	cout << vuShow << endl;
}

int runValues(){

    // *** Mapeando a memoria do jogo ***

	// Abrindo o arquivo de memoria mapeada do jogo
	// Open the memory-mapped file
	HANDLE fileHandle = OpenFileMapping( PAGE_READONLY, FALSE, MAP_OBJECT_NAME );
	if (fileHandle == NULL)
	{
		printf( "Could not open file mapping object (%d).\n", GetLastError() );
		return 1;
	}

	// Buscando estrutura de dados
	// Get the data structure
	const SharedMemory* sharedData = (SharedMemory*)MapViewOfFile( fileHandle, PAGE_READONLY, 0, 0, sizeof(SharedMemory) );
	if (sharedData == NULL)
	{
		printf( "Could not map view of file (%d).\n", GetLastError() );

		CloseHandle( fileHandle );
		return 1;
	}

	// Verificando se a versão está  correta
	// Ensure we're sync'd to the correct data version
	if ( sharedData->mVersion != SHARED_MEMORY_VERSION )
	{
		printf( "Data version mismatch\n");
		return 1;
	}

	// *** Fim do mapeamento de memoria ***

	// Para ser usado com struct "ParticipantInfo"
	const ParticipantInfo& viewedParticipantInfo = sharedData->mParticipantInfo[sharedData->mViewedParticipantIndex];

    float speed = sharedData->mSpeed;
	float rpm = sharedData->mRpm;
	float maxrpm = sharedData->mMaxRPM;
	float throttle = sharedData->mThrottle;
	float AngularVelocity = sharedData->mAngularVelocity[1];

	// Somente teste de variavel array da struct "ParticipantInfo"
	float worldPosition = viewedParticipantInfo.mWorldPosition[0];   // [ UNITS = World Space  X  Y  Z ]

	// Imprimindo...
	printValue(" Test : ", worldPosition);
	printValue(" Speed: ", 3.6*speed);
	printValue(" mRPM.: ", maxrpm);
	printValue(" RPM..: ", rpm);
	printVu(rpm, maxrpm);
	cout << endl;
	printValue(" Steering: ", sharedData->mSteering);
	printValue(" Throttle: ", sharedData->mThrottle);
	printValue(" Brake...: ", sharedData->mBrake);
	cout << endl;
	cout << " Velocidade angular em graus: " << endl;
	printValue(" X[0]: ", ANGLE*sharedData->mAngularVelocity[0]);
	printValue(" Y[1]: ", ANGLE*sharedData->mAngularVelocity[1]);
	printValue(" Z[2]: ", ANGLE*sharedData->mAngularVelocity[2]);
	cout << endl;
	cout << " Orientacao em algulos de Euler:" <<endl;
	printValue(" X[0]: ", sharedData->mOrientation[0]);
	printValue(" Y[1]: ", sharedData->mOrientation[1]);
	printValue(" Z[2]: ", sharedData->mOrientation[2]);

	// enviando dados para Arduino
    if (AngularVelocity > 0){
        sprintf(intChr, "%.0f", AngularVelocity*820);
        sprintf(outData, "%c", 'l');
        strcat(outData, intChr);
        SP->WriteData(outData, (unsigned)strlen(outData));

    }
    else if (AngularVelocity < 0){
        sprintf(intChr, "%.0f", AngularVelocity*-820);
        sprintf(outData, "%c", 'r');
        strcat(outData, intChr);
        SP->WriteData(outData, (unsigned)strlen(outData));
    }


	/*
	sprintf(outData, "%.0f", throttle*999);
	printValue(" Acel x 999: ", throttle*999);
    SP->WriteData(outData, (unsigned)strlen(outData));
    */

	Sleep(20);
	system("cls");


}

int main (){
    // loop
	while(1){
	    runValues();
	}
	return 0;
}
