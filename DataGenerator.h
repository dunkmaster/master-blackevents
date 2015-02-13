#ifndef _DATAGENERATOR_H
#define _DATAGENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <systemc.h>
#include "GlobalConstants.h"
#include "Sample.h"
#include "RandomGenerator.h"
#include <iostream>
#include <string>
#include <fstream>
#include "Monitor.h"
#include "Mapper.h"
#include <map>
#include <list>
#include <vector>

SC_MODULE(DataGenerator)
{
public:

	int occupancyPoints[constants::NUMBER_TIME_WINDOWS_TO_SIMULATE];
	typedef std::map< int, std::list<Sample> > DataEntry;
	typedef std::vector< DataEntry > Datamap;


	void t_sink(void);
	void write_log_to_file_sink(int _packetCounter, int _port, int _currentTimeWindow);
	int generateCore(int portNumber, int currentTimeWindow, int packetCounter, int occupancy);
	void incrementingOccupancySink();
	void standardSink();
	void globalRandomnessSink();
	int createSingleFlux();

	//Huffman methods


	//End
	Datamap readBlackEvents();
	void sendBlackEvents();

	//Create flux and variety functions
	void createFlux(std::vector<int> &values);
	void initOccupancy();
	sc_port < sc_fifo_out_if< Sample > > porter_DG_to_SAMPA[constants::NUMBER_OF_SAMPA_CHIPS*constants::SAMPA_NUMBER_INPUT_PORTS];//antall sampa * antall input porter per sampa

	// Constructor
	SC_CTOR(DataGenerator)
	{
		SC_THREAD(t_sink);
	}

private:

};
#endif
