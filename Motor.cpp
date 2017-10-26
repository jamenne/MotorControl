//	Class to communicate with the motors
//
//	Created by Janine Müller on 30.07.2015

//Restore factory settings; Nur im Notfall benutzen
//SendCmd(Motor_handle[0], 1, 137, 0, 0, 1234); 	
//SendCmd(Motor_handle[1], 1, 137, 0, 0, 1234);

#include "../RS232com/RS232communication.h"
#include "Motor.h"
#include <fstream>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <ctime>
#include <vector>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>

using namespace std;

//Constructor
Motor::Motor(unsigned char &Address, unsigned char &Status, int &Value, bool verbosity):
_Address(Address),
_Status(Status),
_Value(Value)
{

	if (verbosity)
		{
			this->CheckComports();
		}	
	
	// Connect motors, motors are saved in an array	
	//this->ConnectMotor(verbosity);

	//this->_NumOfMotor = this->_Motor_storage.size();

}


void Motor::CheckComports(){

	int n=number_of_comports();

	switch(n){
		case 0: cerr << "no comports present."; break;
		case 1: cout << "1 comport present." << endl; break;
		default: cout << n << " comports present." << endl; break;
	}

}

void Motor::ConnectMotor(bool verbosity){

	cout << "Looking for Motors. This may take a few seconds..." << endl;

	// Get Number of Comports, from RS232Communication
	int n = number_of_comports();

	// Check for every comport, if a motor is present
	vector<int> Motor_handle(n);

	for (int a=0; a<n ;a++){

		//cout << "errno " << errno << endl;
		// needs to be set to 0...but why??
		errno = 0;


		// Try to open RS232 connection
		Motor_handle[a] = OpenRS232(a+1, 9600); // Baudrate=9600 is defined in RS232communication.cpp, this parameter has no effect
		//cout << a << Motor_handle[a] << endl;


		if (Motor_handle[a] >= 0){

			// just try to speak with motor, used GAP to not change the position
			SendCmd(Motor_handle[a], 1, TMCL_GAP, 0, 0, 0);
			sleep(1);

			//if (GetResult(Motor_handle[a], &Address, &Status, &Value)==TMCL_RESULT_OK) break;
			GetResult(Motor_handle[a], this->_Address, this->_Status, this->_Value);
			if (verbosity) cout << "errno = " << errno << ":\t" << strerror(errno) << endl;
				if (errno != 0 ){
					if (verbosity) cout << "com device " << a+1 << " did not respond" << endl;
				}
			else{
				this->_Motor_storage.push_back(Motor_handle[a]);
				//cout << Motor_handle[a] << endl;
				cout << "Motor " << a+1 << " OK." << endl;
			}
		}
	}

	switch(this->_Motor_storage.size()){
		case 0: {
			cerr << "No Motor detected - Please check RS232 connections and power plugs. Assuming Motor at COM1." << endl; 
			this->_Motor_storage.push_back(Motor_handle[1]);
			break;
		}
		case 1: cout << "1 Motor detected." << endl; break;
		default: cout << this->_Motor_storage.size() << " Motors detected." << endl; break;
	}

	//Enable end switches 
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 12, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 13, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 12, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 13, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);

	this->_NumOfMotor = this->_Motor_storage.size();

	cout << "Finished Initialization of Motors" << endl;

}

void Motor::ReferenceRunX(){
	//Parameters maybe have to be adjusted

	//Settings for the x-axis
	//Enable end switches 
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 12, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 13, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	//Referencing search mode
	//1 – Only the left reference switch is searched.
	//2 – The right switch is searched, then the left switch is searched.
	//3 – Three-switch-mode: the right switch is searched first, then the reference switch will be searched.
	//SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 193, 0, 1);
	//GetResult(this->_Motor_storage[0], &Address, &Status, &Value);
	//Max. current; TMC109 max. 255 
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 6, 0, 200);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	//Max. acceleration
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 5, 0, 5);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	//Max. speed
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 4, 0, 300);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	//Min. speed
	//SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 130, 0, 1);
	//GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	//Disable soft stop, the Motor will stop immediately (disregarding Motor limits), when the reference or limit switch is hit.
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 149, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	//Referencing search speed (0-full speed, 1-half of max.,...)
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 194, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	//Reference switch speed (0-8), the speed for the switching point calibration can be selected
	//SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 195, 0, 8);
	//GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	sleep(1);
	

	//Reference Run
	SendCmd(this->_Motor_storage[0], 1, TMCL_RFS, RFS_START, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);

	sleep(5);

	//Value = 6; // should not be zero

	/*while(Value != 0){

		SendCmd(this->_Motor_storage[1], 1, TMCL_RFS, RFS_STATUS, 0, 0);
		GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
		cout << "Value: " << Value  << endl;
	}*/
	

	//Set zero position
	SendCmd(this->_Motor_storage[0], 1, TMCL_MST, 0, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 0, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
	SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 1, 0, 0);
	GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);


}

void Motor::ReferenceRunY(){

	//Parameters maybe have to be adjusted

	//Settings for the y-axis
	//Enable end switches 
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 12, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 13, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	//Referencing search mode
	//1 – Only the left reference switch is searched.
	//2 – The right switch is searched, then the left switch is searched.
	//3 – Three-switch-mode: the right switch is searched first, then the reference switch will be searched.
	//SendCmd(Motor[1], 1, TMCL_SAP, 193, 0, 1);
	//GetResult(Motor[1], this->_Address, this->_Status, this->_Value);
	//Max. current; Max. 1500
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 6, 0, 900);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	//Max. acceleration
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 5, 0, 3);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	//Max. speed
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 4, 0, 200);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	//Min. speed
	//SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 130, 0, 1);
	//GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	//Referencing search speed (0-full speed, 1-half of max.,...)
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 194, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	//Disable soft stop
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 149, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	//Reference switch speed (0-8), the speed for the switching point calibration can be selected
	//SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 195, 0, 8);
	//GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	sleep(1);


	//Reference Run
	SendCmd(this->_Motor_storage[1], 1, TMCL_RFS, RFS_START, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);

	sleep(5);
	/*Value = 6; // should not be zero

	while(Value != 0){

		SendCmd(this->_Motor_storage[1], 1, TMCL_RFS, RFS_STATUS, 0, 0);
		GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
		cout << "Value: " << Value  << endl;
	}*/
	
	//Set zero position
	SendCmd(this->_Motor_storage[1], 1, TMCL_MST, 0, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 0, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
	SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 1, 0, 0);
	GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);

}

void Motor::MoveRelative(string xy, int pos, int speed){

	if (xy=="x" || xy=="X"){

		SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 4, 0, speed);		
		SendCmd(this->_Motor_storage[0], 1, TMCL_MVP, MVP_REL, 0, pos);
		GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
		sleep(4);
	}

	if (xy=="y" || xy =="Y")
	{
		SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 4, 0, speed);		
		SendCmd(this->_Motor_storage[1], 1, TMCL_MVP, MVP_REL, 0, pos);
		GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
		sleep(4);
	}

	if(xy != "y" && xy != "Y" && xy != "x" && xy != "X"){
		cout << "Wrong parameter for xy used" << endl;
	}
}

void Motor::MoveAbsolute(string xy, int pos, int speed){


	if (xy=="x" || xy=="X")
	{
		SendCmd(this->_Motor_storage[0], 1, TMCL_SAP, 4, 0, speed);			
		SendCmd(this->_Motor_storage[0], 1, TMCL_MVP, MVP_ABS, 0, pos);
		GetResult(this->_Motor_storage[0], this->_Address, this->_Status, this->_Value);
		sleep(4);
	}

	if (xy=="y" || xy =="Y")
	{
		SendCmd(this->_Motor_storage[1], 1, TMCL_SAP, 4, 0, speed);			
		SendCmd(this->_Motor_storage[1], 1, TMCL_MVP, MVP_ABS, 0, pos);
		GetResult(this->_Motor_storage[1], this->_Address, this->_Status, this->_Value);
		sleep(4);
	}

	if(xy != "y" && xy != "Y" && xy != "x" && xy != "X"){
		cout << "Wrong parameter for xy used" << endl;	
	} 
}

int Motor::CalcStepsX(double pos){ // returns number of steps for a given distance in mm for the Motor on the x axis
	// For x axis: 12800 steps = 140mm -> 91,4285714steps = 1mm
	// round down to int -> +0.5
	
	return (int)(pos * 91.4285714 + 0.5);
}

int Motor::CalcStepsY(double pos){ // returns number of steps for a given distance in mm for the Motor on the y axis
	// For y axis: 12800 steps = 85mm -> 150,588235 = 1mm
	// round down to int -> +0.5

	return (int)(pos * 150.588235 + 0.5);
}

//Send a binary TMCL command
//e.g.  SendCmd(ComHandle, 1, TMCL_MVP, MVP_ABS, 1, 50000);   will be MVP ABS, 1, 50000 for a module with address 1
//Parameters: Handle: Handle of the serial port (returned by OpenRS232).
//            Address: address of the module (factory default is 1).
//            Command: the TMCL command (see the constants at the begiining of this file)
//            Type:    the "Type" parameter of the TMCL command (set to 0 if unused)
//            Motor:   the motor number (set to 0 if unused)
//            Value:   the "Value" parameter (depending on the command, set to 0 if unused)
void Motor::SendCmd(int Handle, unsigned char Address, unsigned char Command, unsigned char Type, unsigned char Motor, int Value)
{
	unsigned char TxBuffer[9];
	int i;
	int written;

	TxBuffer[0]=Address;
	TxBuffer[1]=Command;
	TxBuffer[2]=Type;
	TxBuffer[3]=Motor;
	TxBuffer[4]=Value >> 24;
	TxBuffer[5]=Value >> 16;
	TxBuffer[6]=Value >> 8;
	TxBuffer[7]=Value & 0xff;
	TxBuffer[8]=0;
	for(i=0; i<8; i++)
		TxBuffer[8]+=TxBuffer[i];

	//Send the datagram
	written = write(Handle, TxBuffer, 9);

	//cout << "Written: " << written << endl;
}

//Read the result that is returned by the module
//Parameters: Handle: handle of the serial port, as returned by OpenRS232
//            Address: pointer to variable to hold the reply address returned by the module
//            Status: pointer to variable to hold the status returned by the module (100 means okay)
//            Value: pointer to variable to hold the value returned by the module
//Return value: TMCL_RESULT_OK: result has been read without errors
//              TMCL_RESULT_NOT_READY: not enough bytes read so far (try again)
//              TMCL_RESULT_CHECKSUM_ERROR: checksum of reply packet wrong
unsigned char Motor::GetResult(int Handle, unsigned char Address, unsigned char Status, int Value)
{
	unsigned char RxBuffer[9], Checksum;
	int i,byte=1,nbytes=0,nerror=0;

	while(nbytes < 9){
		byte = read(Handle, &RxBuffer[nbytes], 1);
		if (byte > 0) nbytes++;
		else nerror++;
		if(nerror > 9) break;
	}
	//cout << "read() loop returned " << nbytes << endl;
	Checksum=0;
	for(i=0; i<8; i++)
		Checksum+=RxBuffer[i];

	if(Checksum!=RxBuffer[8]) return -1;

	Address=RxBuffer[0];
	Status=RxBuffer[2];
	Value=(RxBuffer[4] << 24) | (RxBuffer[5] << 16) | (RxBuffer[6] << 8) | RxBuffer[7];

	return nbytes;
}