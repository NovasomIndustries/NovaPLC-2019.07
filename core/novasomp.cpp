//-----------------------------------------------------------------------------
// Copyright 2018 Fil
//
// Based on the software by Thiago Alves
// This file is part of the OpenPLC Software Stack.
//
// OpenPLC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenPLC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenPLC.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// This file is the hardware layer for the OpenPLC. If you change the platform
// where it is running, you may only need to change this file. All the I/O
// related stuff is here. Basically it provides functions to read and write
// to the OpenPLC internal buffers in order to update I/O state.
// Fil, Mar 2018
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "ladder.h"

#define MAX_INPUT 		1
#define MAX_OUTPUT 		1
#define MAX_ANALOG_OUT	0

int inBufferPinMask[MAX_INPUT]	=	{ 142 };
int outBufferPinMask[MAX_OUTPUT]=	{ 144 };

//analogOutBufferPinMask: pin mask for the analog PWM
//output of the RaspberryPi
//int analogOutBufferPinMask[MAX_ANALOG_OUT] = { 1 };

char	gpioname_in[MAX_INPUT][64];
char	gpioname_out[MAX_OUTPUT][64];

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
char	cmd[256];
	//set pins as input
	for (int i = 0; i < MAX_INPUT; i++)
	{	
		sprintf(cmd,"echo %d > /sys/class/gpio/export",inBufferPinMask[i]);
		system(cmd);
		sprintf(cmd,"echo in > /sys/class/gpio/gpio%d/direction",inBufferPinMask[i]);
		system(cmd);
		sprintf(gpioname_in[i],"/sys/class/gpio/gpio%d/value",inBufferPinMask[i]);

	}
	//set pins as output
	for (int i = 0; i < MAX_OUTPUT; i++)
	{
		sprintf(cmd,"echo %d > /sys/class/gpio/export" , outBufferPinMask[i]);
		system(cmd);
		sprintf(cmd,"echo out > /sys/class/gpio/gpio%d/direction",outBufferPinMask[i]);
		system(cmd);
		sprintf(gpioname_out[i],"/sys/class/gpio/gpio%d/value",outBufferPinMask[i]);
	}
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual state of the input pins. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
FILE *fp;
        pthread_mutex_lock(&bufferLock); //lock mutex
        //INPUT
        for (int i = 0; i < MAX_INPUT; i++)
        {
                if (bool_input[i/8][i%8] != NULL)
                {
                        fp = fopen(gpioname_in[i],"r");
                        *bool_input[i/8][i%8] = fgetc(fp) & 0x01 ;
			fclose(fp);
                }
        }
        pthread_mutex_unlock(&bufferLock); //unlock mutex
}


//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual state of the output pins. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
FILE	*fp;
        pthread_mutex_lock(&bufferLock); //lock mutex

        //OUTPUT
        for (int i = 0; i < MAX_OUTPUT; i++)
        {
                if (bool_output[i/8][i%8] != NULL)
                {
			fp = fopen(gpioname_out[i],"w");
			fprintf(fp,"%x",*bool_output[i/8][i%8]);
			fclose(fp);
		}
        }
        pthread_mutex_unlock(&bufferLock); //unlock mutex
}

void emergency_stop()
{
FILE	*fp;

	for (int i = 0; i < MAX_OUTPUT; i++)
        {
		fp = fopen(gpioname_out[i],"w");
		fprintf(fp,"0");
		fclose(fp);
        }
}

