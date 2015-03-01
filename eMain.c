#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

#include "messages.h"

int main(void){


	unsigned char *dataReceived;
	unsigned int *size;
	unsigned char *ack;
	unsigned char *busy;

	//set the pointer to their variables
	size = (int*)(COMMADDRESS_SIZE);
	ack = (char*)(COMMADDRESS_EPIPHANY_ACK);
	dataReceived = (char*)(COMMADDRESS_DATA_TO_EPIPHANY);
	busy = (char*)(COMMADDRESS_BUSY);

	//set the core as busy
	*busy = 1;
	//wait for signal that all data has been transfered
	while(!dataReceived[0]);
	//acknowledge receiving the data
	*ack = 1;




	//reset busy
	*busy = 0;
	return EXIT_SUCCESS;
}
