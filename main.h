#ifndef _main
#define _main

#include "types.h"
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>

//structure for queue//
class queue
{
public:
	int P_SIZE;		//Packet Size
	int ID;			//Identifier for packet class
	//1 = EF Packet
	//2 = AF Packet
	//3 = BE Packet
	double A_TIME;	//packet arrival time
	
	queue()
	{
		ID		= 0;
		P_SIZE	= 0;		
		A_TIME	= 0.0;		
	}
};
//ends//

//class olt//
class olt
{
public:
	olt() 
	{
		OLT_TIMER = 0.0;
	}
	
	double OLT_TIMER;	//Timer for OLT
};
//ends//

//class onu//
class onu
{
public:
	
	double CUMM_DELAY;

	int end_flag;		//Used to identify Input stream end
	
	olt* HANDLE;
	
	double ONU_TIMER;	//Timer for ONU
	
	fstream INPUT;		//File stream for getting inputs from source file	
	
	int	T_P_STAMP;		//packet size		
	double T_TIME;		//packet arrival time
	double T_B_STAMP;	//byte stamp	
	
private:
	
	int Identifier;		//Used to identify ONU
	
	unsigned long C_CNTR;	//Cycle Counter	
	
	//Member Variables to measure Different Classes Delays
	double EF_DELAY;
	double AF_DELAY;
	double BE_DELAY;	
	
	//Member Variables to measure Different Classes Queuing Depth
	double EF_Q_USAGE;
	double AF_Q_USAGE;
	double BE_Q_USAGE;
	
private:
	//Member variables for Input Queue of ONU
	unsigned long Q_CNTR_EF;//Queue Counter for EF Queue
	unsigned long Q_CNTR_AF;//Queue Counter for AF Queue
	unsigned long Q_CNTR_BE;//Queue Counter for BE Queue
	unsigned long EF_DEPTH;	//Queue Usage of EF Queue
	unsigned long AF_DEPTH;	//Queue Usage of AF Queue
	unsigned long BE_DEPTH;	//Queue Usage of BE Queue
	queue* QUEUE_EF;		//Input Queue of ONU for EF class
	queue* QUEUE_AF;		//Input Queue of ONU for AF class
	queue* QUEUE_BE;		//Input Queue of ONU for BE class
	
	
public:
	onu();					//Constructor of ONU class
	
	~onu()					//Destructor of ONU class
	{
		free(QUEUE_EF);
		free(QUEUE_AF);
		free(QUEUE_BE);
	}
	
private:
	
	void dequeue(int I);	//This will dequeue the packets from
	//input queue of ONU, the argument shows from which class 
	//queue; element has to be dequeued. 1 = EF, 2 = AF & 3 = BE
	
	int get_from_file();	//This will get the next values from
	//the input source file
	
	//overloaded operator to assign one element of queue to another
	queue& operator= (queue& Q);
	
public:	
	
	void get_packet();		//It will update the input queues
	//after receiving gate message from OLT
	
	//upstream transmission of packets using the service class priority
	
	void transmit_packet();//It will perform the successive 
	//operations of packet transmission and queue update 
	//along with delay measurements and queuing depth measurement
	
	//This will open the source stream of any onu
	int open_file(char* PATH);
	
	void set_handle(olt* H);
	
	void close_file();
	
	double get_delay(int I);//Returns the delay of traffic
	//1 = EF
	//2 = AF
	//3 = BE
	
	double get_depth(int I);//Returns the queuing depth
	//1 = EF
	//2 = AF
	//3 = BE
	
	void set_identifier(int I);
};
//ends//

void onu::set_identifier(int I)
{
	Identifier = I;
}

double onu::get_depth(int I)
{
	double ret = 0.0;
	
	switch(I)
	{
	case 1:
		ret = EF_Q_USAGE;
		break;
		
	case 2:
		ret = AF_Q_USAGE;
		break;
		
	case 3:
		ret = BE_Q_USAGE;
		break;
	}
	
	return ret;
}

double onu::get_delay(int I)
{
	double ret = 0.0;
	
	switch(I)
	{
	case 1:
		ret = EF_DELAY;
		break;
		
	case 2:
		ret = AF_DELAY;
		break;
		
	case 3:
		ret = BE_DELAY;
		break;
	}
	
	return ret;
}

void onu::close_file()
{
	INPUT.close();
}

void onu::set_handle(olt* H)
{
	HANDLE = H;
}

int onu::open_file(char* PATH)
{
	INPUT.open(PATH, ios::in);
	
	if(INPUT.eof())
	{
		printf("Unable to Open tyhe Source file");	
		return 0;
	}
	
	get_from_file();
	ONU_TIMER = T_TIME;
	
	return 1;
}

queue& onu::operator = (queue& Q)
{
	queue temp;
	
	temp.A_TIME = Q.A_TIME;
	temp.ID		= Q.ID;
	temp.P_SIZE = Q.P_SIZE;
	
	return temp;
}

onu::onu()
{
	end_flag = 0;
	Identifier = 0;
	
	C_CNTR = 0;
	
	Q_CNTR_EF = 0;
	Q_CNTR_AF = 0;
	Q_CNTR_BE = 0;
	
	EF_DEPTH = 0;
	AF_DEPTH = 0;
	BE_DEPTH = 0;
	
	HANDLE = NULL;
	QUEUE_EF  = NULL; 
	QUEUE_AF  = NULL; 
	QUEUE_BE  = NULL; 
	
	ONU_TIMER = 0.0;
	
	CUMM_DELAY = 0.0;

	EF_DELAY = 0.0;
	AF_DELAY = 0.0;
	BE_DELAY = 0.0;
	
	EF_Q_USAGE = 0.0;
	AF_Q_USAGE = 0.0;
	BE_Q_USAGE = 0.0;
	
	//Allocating Memory for EF class Queue
	QUEUE_EF = (queue*) malloc(152650 * sizeof(queue));
	
	if(QUEUE_EF == NULL)
	{
		printf("Unable to allocate memory for EF");
		exit(0);
	}
	
	//Allocating Memory for AF class Queue
	QUEUE_AF = (queue*) malloc(152650 * sizeof(queue));
	
	if(QUEUE_AF == NULL)
	{
		printf("Unable to allocate memory for AF");
		exit(0);
	}
	
	//Allocating Memory for BE class Queue
	QUEUE_BE = (queue*) malloc(152650 * sizeof(queue));
	
	if(QUEUE_BE == NULL)
	{
		printf("Unable to allocate memory for BE");
		exit(0);
	}
}

int onu::get_from_file()
{
	int ret = 0;
	
	if(INPUT.eof())
	{
		ret = 1;
		printf("ONU %d is out of Traffic\n", Identifier);
		return ret;
	}
	
	INPUT >> T_TIME >> T_P_STAMP >> T_B_STAMP;
	return ret;
}

void onu::dequeue(int I)
{
	unsigned long j = 0;
	
	switch(I)
	{
	case 1:			//dequeue from EF class of service
		
		if(Q_CNTR_EF == 0)
			return;	
		
		for(j = 0; j < (Q_CNTR_EF - 1); j++)
		{		
			QUEUE_EF[j].ID		= QUEUE_EF[j+1].ID;
			QUEUE_EF[j].P_SIZE	= QUEUE_EF[j+1].P_SIZE;
			QUEUE_EF[j].A_TIME	= QUEUE_EF[j+1].A_TIME;		
		}
		Q_CNTR_EF--;
		break;
		
	case 2:			//dequeue from AF class of service
		
		if(Q_CNTR_AF == 0)
			return;		
		
		for(j = 0; j < (Q_CNTR_AF - 1); j++)
		{		
			QUEUE_AF[j].ID		= QUEUE_AF[j+1].ID;
			QUEUE_AF[j].P_SIZE	= QUEUE_AF[j+1].P_SIZE;
			QUEUE_AF[j].A_TIME	= QUEUE_AF[j+1].A_TIME;		
		}
		Q_CNTR_AF--;
		break;
		
	case 3:			//dequeue from BE class of service
		
		if(Q_CNTR_BE == 0)
			return;		
		
		for(j = 0; j < (Q_CNTR_BE - 1); j++)
		{		
			QUEUE_BE[j].ID		= QUEUE_BE[j+1].ID;
			QUEUE_BE[j].P_SIZE	= QUEUE_BE[j+1].P_SIZE;
			QUEUE_BE[j].A_TIME	= QUEUE_BE[j+1].A_TIME;		
		}
		Q_CNTR_BE--;
		break;	
	}	
}

void onu::get_packet()
{
	int escape = 0;
	
	//temporary queue for intial traffic acquire from input data stream
	unsigned long TEMP_CNTR		= 0;//Counter for No. of packets in temp array
	unsigned long TEMP_DEPTH	= 0;//Current Depth of temp array in Bytes
	queue* TEMP_QUEUE = (queue*) malloc(152650 * sizeof(queue));
	//ends
	
	unsigned long I		= 0;
	unsigned long J		= 0;
	unsigned long END	= 0;
	double TEMP_TIMER	= T_TIME;	
	
	//pick all the packets from input data stream for which the 
	//arrival time is less than current value of OLT timer
	while(T_TIME <= HANDLE->OLT_TIMER)
	{
		//if the cummlative depth of all queues become 10 MB then return
		if((EF_DEPTH + AF_DEPTH + BE_DEPTH + TEMP_DEPTH) >= 10*1E+6)
			break;
		
		TEMP_TIMER = T_TIME;
		TEMP_DEPTH = TEMP_DEPTH + T_P_STAMP;
		
		TEMP_QUEUE[TEMP_CNTR].A_TIME	= T_TIME;
		TEMP_QUEUE[TEMP_CNTR].P_SIZE	= T_P_STAMP;
		
		TEMP_CNTR++;
		
		escape = get_from_file();
		
		if(escape == 1)
			end_flag = 1;
	}
	
	/*
	QUEUE[Q_Cntr].ID = 1;
	QUEUE[Q_Cntr].P_size	= 1120;	
	QUEUE[Q_Cntr++].A_time	= ONU_timer;
	*/
	
	J = (unsigned long) ((0.3 * (double) TEMP_CNTR) + 0.5);
	END = J;
	
	for(I = 0; I < END; I++)
	{
		QUEUE_BE[Q_CNTR_BE].ID		= 1;
		QUEUE_BE[Q_CNTR_BE].P_SIZE	= TEMP_QUEUE[I].P_SIZE;		
		QUEUE_BE[Q_CNTR_BE].A_TIME	= TEMP_QUEUE[I].A_TIME;
		
		BE_DEPTH += TEMP_QUEUE[I].P_SIZE;
		
		Q_CNTR_BE++;
	}
	
	J = (unsigned long) ((0.4 * (double) TEMP_CNTR) + 0.5);
	END = I + J;
	
	for(I = I; I < END; I++)
	{
		QUEUE_AF[Q_CNTR_AF].ID		= 2;
		QUEUE_AF[Q_CNTR_AF].P_SIZE	= TEMP_QUEUE[I].P_SIZE;		
		QUEUE_AF[Q_CNTR_AF].A_TIME	= TEMP_QUEUE[I].A_TIME;
		
		AF_DEPTH += TEMP_QUEUE[I].P_SIZE;
		
		Q_CNTR_AF++;	
	}
	
	for(I = I; I < TEMP_CNTR; I++)
	{
		QUEUE_EF[Q_CNTR_EF].ID		= 3;
		QUEUE_EF[Q_CNTR_EF].P_SIZE	= TEMP_QUEUE[I].P_SIZE;		
		QUEUE_EF[Q_CNTR_EF].A_TIME	= TEMP_QUEUE[I].A_TIME;
		
		EF_DEPTH += TEMP_QUEUE[I].P_SIZE;
		
		Q_CNTR_EF++;
	}
	
	free(TEMP_QUEUE);
	ONU_TIMER = T_TIME;
}

void onu::transmit_packet()
{
	double TEMP_EF_DELAY = 0.0;
	double TEMP_AF_DELAY = 0.0;
	double TEMP_BE_DELAY = 0.0;

	unsigned long CUMM_END = 0;

	unsigned long EF_END = 0;
	unsigned long AF_END = 0;
	unsigned long BE_END = 0;
	
	unsigned long I = 0;
	unsigned long J = 0;
	
	bool EF_DONE = true;
	bool AF_DONE = true;
	bool BE_DONE = true;
	
	unsigned long TEMP_B_STAMP = 0;
	unsigned long CUMM_B_STAMP = 0;	
	
	
	/////////////////////////Our Algo/////////////////////////
	
	unsigned long CUMM_DEPTH = EF_DEPTH + AF_DEPTH + BE_DEPTH;
	
	double EF_USAGE = (double) EF_DEPTH / (double) CUMM_DEPTH * 100.0;
	double AF_USAGE = (double) AF_DEPTH / (double) CUMM_DEPTH * 100.0;
	double BE_USAGE = (double) BE_DEPTH / (double) CUMM_DEPTH * 100.0;
	
	//If EF is greater than 30% of the relative queue then
	//EF -> AF -> BE
	if(EF_USAGE >= 30.0)
	{
		for(I = 0; I < Q_CNTR_EF; I++)
		{
			CUMM_B_STAMP += QUEUE_EF[I].P_SIZE;		
			
			if(CUMM_B_STAMP > 15000)
			{
				EF_DONE = false;
				break;
			}
			
			TEMP_B_STAMP = CUMM_B_STAMP;
			EF_END++;
		}
		
		//If all of the EF traffic has been exhausted then go for AF
		if(EF_DONE)		
		{
			for(I = 0; I < Q_CNTR_AF; I++)
			{
				CUMM_B_STAMP += QUEUE_AF[I].P_SIZE;		
				
				if(CUMM_B_STAMP > 15000)
				{
					AF_DONE = false;
					break;
				}
				
				TEMP_B_STAMP = CUMM_B_STAMP;
				AF_END++;
			}		
		}
		
		//If all of the AF traffic has been exhausted then go for BE
		if(AF_DONE && EF_DONE)		
		{
			for(I = 0; I < Q_CNTR_BE; I++)
			{
				CUMM_B_STAMP += QUEUE_BE[I].P_SIZE;		
				
				if(CUMM_B_STAMP > 15000)
				{
					BE_DONE = false;
					break;
				}
				
				TEMP_B_STAMP = CUMM_B_STAMP;
				BE_END++;
			}		
		}		
	}
	
	//If AF is greater than 40% of the relative queue then
	//AF -> EF -> BE
	else if(AF_USAGE >= 40.0)
	{
		for(I = 0; I < Q_CNTR_AF; I++)
		{
			CUMM_B_STAMP += QUEUE_AF[I].P_SIZE;		
			
			if(CUMM_B_STAMP > 15000)
			{
				AF_DONE = false;
				break;
			}
			
			TEMP_B_STAMP = CUMM_B_STAMP;
			AF_END++;
		}
		
		//If all of the AF traffic has been exhausted then go for EF
		if(AF_DONE)		
		{
			for(I = 0; I < Q_CNTR_EF; I++)
			{
				CUMM_B_STAMP += QUEUE_EF[I].P_SIZE;		
				
				if(CUMM_B_STAMP > 15000)
				{
					EF_DONE = false;
					break;
				}
				
				TEMP_B_STAMP = CUMM_B_STAMP;
				EF_END++;
			}		
		}
		
		//If all of the EF traffic has been exhausted then go for BE
		if(AF_DONE && EF_DONE)		
		{
			for(I = 0; I < Q_CNTR_BE; I++)
			{
				CUMM_B_STAMP += QUEUE_BE[I].P_SIZE;		
				
				if(CUMM_B_STAMP > 15000)
				{
					BE_DONE = false;
					break;
				}
				
				TEMP_B_STAMP = CUMM_B_STAMP;
				BE_END++;
			}		
		}		
	}
	
	//If BE is greater than 30% of the relative queue then
	//BE -> EF -> AF
	else if(BE_USAGE >= 30.0)
	{
		for(I = 0; I < Q_CNTR_BE; I++)
		{
			CUMM_B_STAMP += QUEUE_BE[I].P_SIZE;		
			
			if(CUMM_B_STAMP > 15000)
			{
				BE_DONE = false;
				break;
			}
			
			TEMP_B_STAMP = CUMM_B_STAMP;
			BE_END++;
		}
		
		//If all of the BE traffic has been exhausted then go for EF
		if(BE_DONE)		
		{
			for(I = 0; I < Q_CNTR_EF; I++)
			{
				CUMM_B_STAMP += QUEUE_EF[I].P_SIZE;		
				
				if(CUMM_B_STAMP > 15000)
				{
					EF_DONE = false;
					break;
				}
				
				TEMP_B_STAMP = CUMM_B_STAMP;
				EF_END++;
			}		
		}
		
		//If all of the EF traffic has been exhausted then go for AF
		if(BE_DONE && EF_DONE)		
		{
			for(I = 0; I < Q_CNTR_AF; I++)
			{
				CUMM_B_STAMP += QUEUE_AF[I].P_SIZE;		
				
				if(CUMM_B_STAMP > 15000)
				{
					AF_DONE = false;
					break;
				}
				
				TEMP_B_STAMP = CUMM_B_STAMP;
				AF_END++;
			}		
		}		
	}
	
	/////////////////////////Ends/////////////////////////////
	
	//If any of the above mentioned criteria doesnt meet then
	//follow strict priority {This will never happen}
	else{
	
		
		/////////////////////////Strict Priority//////////////////
		for(I = 0; I < Q_CNTR_EF; I++)
		{
			CUMM_B_STAMP += QUEUE_EF[I].P_SIZE;		
			
			if(CUMM_B_STAMP > 15000)
			{
				EF_DONE = false;
				break;
			}
			
			TEMP_B_STAMP = CUMM_B_STAMP;
			EF_END++;
		}
		
		//If all of the EF traffic has been exhausted then go for AF
		if(EF_DONE)		
		{
			for(I = 0; I < Q_CNTR_AF; I++)
			{
				CUMM_B_STAMP += QUEUE_AF[I].P_SIZE;		
				
				if(CUMM_B_STAMP > 15000)
				{
					AF_DONE = false;
					break;
				}
				
				TEMP_B_STAMP = CUMM_B_STAMP;
				AF_END++;
			}		
		}
		
		//If all of the AF traffic has been exhausted then go for BE
		if(AF_DONE && EF_DONE)		
		{
			for(I = 0; I < Q_CNTR_BE; I++)
			{
				CUMM_B_STAMP += QUEUE_BE[I].P_SIZE;		
				
				if(CUMM_B_STAMP > 15000)
				{
					BE_DONE = false;
					break;
				}
				
				TEMP_B_STAMP = CUMM_B_STAMP;
				BE_END++;
			}		
		}
	}
	
	////////////////////////////Ends//////////////////////////////////
	
	
	
	/*******Transmitting the packet*******/
	HANDLE->OLT_TIMER += ((double) (TEMP_B_STAMP + PREAMBLE + REPORT) 
		/ LINK_RATE) + GUARD_TIME + TRAVEL_TIME;
	/*******          END          *******/	
	
	//Removing the apckets from ONU Queues
	
	for(J = 0; J < EF_END; J++)
	{
		TEMP_EF_DELAY += (HANDLE->OLT_TIMER - QUEUE_EF[0].A_TIME);
		EF_DEPTH = (EF_DEPTH - QUEUE_EF[0].P_SIZE);
		dequeue(1);	//Dequeue EF Packet
	}
	
	for(J = 0; J < AF_END; J++)
	{
		TEMP_AF_DELAY += (HANDLE->OLT_TIMER - QUEUE_AF[0].A_TIME);
		AF_DEPTH = (AF_DEPTH - QUEUE_AF[0].P_SIZE);
		dequeue(2);	//Dequeue AF Packet
	}
	
	for(J = 0; J < BE_END; J++)
	{
		TEMP_BE_DELAY += (HANDLE->OLT_TIMER - QUEUE_BE[J].A_TIME);
		BE_DEPTH = (BE_DEPTH - QUEUE_BE[0].P_SIZE);
		dequeue(3);	//Dequeue BE Packet
	}
	
	//ends

	//The cummulative Delay//
	CUMM_END = EF_END + AF_END + BE_END;

	if(CUMM_END == 0)
		CUMM_DELAY += 0.0;
	else
		CUMM_DELAY += ((TEMP_EF_DELAY + TEMP_AF_DELAY + TEMP_BE_DELAY) / (double) CUMM_END);

	//The delay in transmitted traffic
	
	if(EF_END == 0)
		EF_DELAY += 0.0;
	else
		EF_DELAY += TEMP_EF_DELAY / (double) EF_END;

	if(AF_END == 0)
		AF_DELAY += 0.0;
	else
		AF_DELAY += TEMP_AF_DELAY / (double) AF_END;

	if(BE_END == 0)
		BE_DELAY += 0.0;
	else
		BE_DELAY += TEMP_BE_DELAY / (double) BE_END;
	
	//ends
	
	
	//Queueing Depth
	
	EF_Q_USAGE += ((double) EF_DEPTH / (10.0*1E+6) * 100.0); 
	AF_Q_USAGE += ((double) AF_DEPTH / (10.0*1E+6) * 100.0);
	BE_Q_USAGE += ((double) BE_DEPTH / (10.0*1E+6) * 100.0);
	
	//ends
	
	C_CNTR++;
}
	
#endif