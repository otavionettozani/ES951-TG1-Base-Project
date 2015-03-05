#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <e-hal.h>
#include "messages.h"

#define RAM_SIZE (0x8000)

void clearFlags(){


	e_epiphany_t dev;
	int clear[2] = {0,0};
	unsigned int i,j;

	for(i=0;i<4;i++){
		for(j=0;j<4; j++){
			e_open(&dev,i,j,1,1);
			e_reset_group(&dev);
			e_write(&dev,0,0,COMMADDRESS_BUSY,clear,2*sizeof(int));
			e_close(&dev);
		}
	}
	usleep(10000);

}

void dataSent(void *dev, unsigned row, unsigned col, unsigned size){

	unsigned sizePointer[1];
	unsigned char bitOn[1];
	unsigned char *ack;

	sizePointer[0] = size;
	bitOn[0] = 1;

	//send to the core the size of the message sent
	e_write(dev, row, col, COMMADDRESS_SIZE, sizePointer, sizeof(unsigned));
	//set the data_to_epiphany bit to 1, so that the core knows that the data has been transfered
	e_write(dev, row, col,COMMADDRESS_DATA_TO_EPIPHANY,bitOn,sizeof(char));

	//wait for the core to acknowledge the receiving of the data
	while (!ack[0]){
		e_read(dev,row,col,COMMADDRESS_EPIPHANY_ACK,ack,sizeof(char));
	}

	return;
}

int main(){


	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t memory;

	e_return_stat_t result;

	unsigned read_buffer[RAM_SIZE/4];
	unsigned read_data;
	unsigned addr;
	int i,j,k;

	char filename[9] = "logs.txt";
	unsigned message = 0x0000beef;
	FILE* file = fopen(filename,"w");

	e_init(NULL);
	e_get_platform_info(&platform);

	clearFlags();

	//write to memory and execute program

	for(i=0;i<platform.rows;i++){
		for(j=0;j<platform.cols; j++){
			e_open(&dev,i,j,1,1);
			e_reset_group(&dev);

			e_write(&dev, 0,0, COMMADDRESS_DATA, &message, sizeof(unsigned));
			dataSent(&dev, 0,0, 1*sizeof(message));
			e_return_stat_t result = e_load("epiphanyProgram.srec",&dev,0,0,E_TRUE);

			if(result != E_OK){
				return EXIT_FAILURE;
			}

			e_close(&dev);
		}
	}

	usleep(20000);




	//read all memory
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	fprintf(file,"(ROW,COL)   ADDRESS   DATA\n");
	fprintf(file,"-----------------------------\n");
	for (i=0; i<(platform.rows); i++) {
		for (j=0; j<(platform.cols); j++) {
			for(k=0;k<RAM_SIZE/4;k++){
				addr=4*k;
				e_read(&dev, i, j, addr, &read_data, sizeof(int));
				read_buffer[k]=read_data;
			}
			for(k=0;k<RAM_SIZE/4;k++){
				fprintf(file,"(%2d,%2d)     0x%08x   0x%08x\n",i,j,k*4,read_buffer[k]);
			}
		}
	}


	fclose(file);
	e_close(&dev);
	e_finalize();

	return EXIT_SUCCESS;

}
