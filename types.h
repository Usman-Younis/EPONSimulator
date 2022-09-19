#ifndef _types
#define _types

const double LINK_RATE	= 0.15625*1E+12;//in bytes / sec
const double GUARD_TIME = 5*1E-6;		//in sec

const int PREAMBLE	= 0;	//in bytes
const int REPORT	= 64;	//in bytes

const double TRAVEL_TIME = 14000.0 / (3.0*1E8);//travel time for
//transmission over 20 Km fibre
#endif