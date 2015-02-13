#ifndef SAMPA_H
#define SAMPA_H

#include <systemc.h>
#include "Channel.h"
#include "GlobalConstants.h"
#include "Sample.h"
#include "Packet.h"
#include <list>
#include <queue>
#include "Monitor.h"
/*
- Serial Clock out: 320 MHz: 3.12500 nanoseconds per clock cycle.
- Clock in: 10bit per 10MHz per channel
- 32 channels in
- 4 serial e-links
	- 8 channels per elink
*/

struct Info
{
	/* data */
	Info() : gotInfo(false) {}
	unsigned int lowestBufferDepth;
	int occupancy;
	unsigned int lowestHeaderBufferDepth;
	int timeWindow;
	Channel *channelWithLowestBufferDepth;
	bool gotInfo;
};



class SAMPA : public sc_module {
public:


	Info infoArray[constants::NUMBER_TIME_WINDOWS_TO_SIMULATE];

	//Gets data from the data generator
	sc_port< sc_fifo_in_if< Sample > >  porter_DG_to_SAMPA[constants::SAMPA_NUMBER_INPUT_PORTS];
	Channel *channels[constants::SAMPA_NUMBER_INPUT_PORTS];
	sc_port< sc_fifo_out_if<Packet> > porter_SAMPA_to_GBT[constants::NUMBER_OUTPUT_PORTS_TO_GBT];
	//Monitor* monitor;
	long numberOfSamplesReceived;
	std::vector< long > dataBufferNumbers;
	std::vector< long > headerBufferNumbers;
	//void dataThread(void);
	void serialOut0(void);
	void serialOut1(void);
	void serialOut2(void);
	void serialOut3(void);

	void reportBufferDepth();
	void reportOccupancy(Packet header, Channel *channel, int bufferDepth, int headerBufferDepth);

	//void processData(Channel *channel, int channelNumber);
	int processData(int serialOut);
	void initChannels(void);

	inline void setAddr(int a) { Addr = a; };
	inline int getAddr(void) { return Addr; };
	
	
	inline void setOutput(bool b){ output = b; };
	inline bool getOutput(){ return output; };

	inline void setChannelOutput(bool b){ channelOutput = b; };
	inline bool getChannelOutput(){ return channelOutput; };

	SAMPA(sc_module_name name);
	//~SAMPA();
private:
	int Addr;
	
	bool output = false;
	bool channelOutput = false;
	bool read = false;

};
#endif

