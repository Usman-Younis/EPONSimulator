#include <string.h>
#include "main.h"

void main ()
{
	fstream RESULT;


	int end_counter = 0;
	int end_identifier[16] = {0};	//Used to end the simulation
	int j = 0, k = 0;

	unsigned long i = 0; 
	unsigned long Iterations = 35000;

	char Path[25];
	char Numb[3];

	printf("hello mani\n");

	olt OLT;
	onu ONU[16];

	OLT.OLT_TIMER = 0.002;

	for(j = 0; j < 16; j++)
	{
		strcpy(Path, "Data\\output");

		if(j < 9)
			strcat(Path, "0");

		itoa(j+1, Numb, 10);

		strcat(Path, Numb);
		strcat(Path, ".txt");
		
		if(ONU[j].open_file(Path) == 0)
		{
			printf("Unable to Open ONU %d source file", j+1);
			exit(0);
		}
		
		ONU[j].set_handle(&OLT);
		ONU[j].set_identifier(j+1);
	}

	for(i = 0; i < Iterations; i++)	
	{
		for(j = 0; j < 16; j++)
		{
			ONU[j].get_packet();			
			ONU[j].transmit_packet();			
		}		
		//printf("Iteration No :: %d\n", Iterations);
		
	}

	RESULT.open("Result.txt", ios::out);

	RESULT<<"\n 0.05 ::: Hybrid ::: Scenerio 2 \n";

	RESULT<<":::AVERAGE DELAYS:::\n";
	RESULT<<"EF  :: "<<(ONU[1].get_delay(1) / (double) Iterations)<<endl;
	RESULT<<"AF  :: "<<(ONU[1].get_delay(2) / (double) Iterations)<<endl;
	RESULT<<"BE  :: "<<(ONU[1].get_delay(3) / (double) Iterations)<<endl;

	RESULT<<"\n:::CUMMULATIVE DELAY:::\n";
	RESULT<<"CUMM  :: "<<(ONU[1].CUMM_DELAY / (double) Iterations)<<endl;

	RESULT<<"\n:::AVERAGE QUEUING DEPTH:::\n";
	RESULT<<"EF  :: "<<(ONU[1].get_depth(1) / (double) Iterations)<<endl;
	RESULT<<"AF  :: "<<(ONU[1].get_depth(2) / (double) Iterations)<<endl;
	RESULT<<"BE  :: "<<(ONU[1].get_depth(3) / (double) Iterations)<<endl;

	RESULT.close();
	

	printf("\n:::AVERAGE DELAYS:::\n");
	printf("EF  ::  %lf\n", (ONU[1].get_delay(1) / (double) Iterations));
	printf("AF  ::  %lf\n", (ONU[1].get_delay(2) / (double) Iterations));
	printf("BE  ::  %lf\n", (ONU[1].get_delay(3) / (double) Iterations));

	printf("\n:::CUMMULATIVE DELAY:::\n");
	printf("CUMM  ::  %lf\n", (ONU[1].CUMM_DELAY / (double) Iterations));

	printf("\n:::AVERAGE QUEUING DEPTH:::\n");
	printf("EF  ::  %lf\n", (ONU[1].get_depth(1) / (double) Iterations));
	printf("AF  ::  %lf\n", (ONU[1].get_depth(2) / (double) Iterations));
	printf("BE  ::  %lf\n", (ONU[1].get_depth(3) / (double) Iterations));

	for(j = 0; j < 16; j++)
		ONU[j].close_file();

}
