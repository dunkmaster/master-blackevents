#include <systemc.h>
#include "SAMPA.h"
#include <cmath>
#include "Channel.h"


SC_HAS_PROCESS(SAMPA);



SAMPA::SAMPA(sc_module_name name) : sc_module(name){

	//this->setLowestBufferDepth(constants::CHANNEL_DATA_BUFFER_SIZE);
	SC_THREAD(serialOut0);
	SC_THREAD(serialOut1);
	SC_THREAD(serialOut2);
	SC_THREAD(serialOut3);



}

void SAMPA::initChannels(){
	for(int i = 0; i < constants::SAMPA_NUMBER_INPUT_PORTS; i++){
		channels[i] = new Channel(sc_gen_unique_name("channel"));
		channels[i]->port_DG_to_CHANNEL(porter_DG_to_SAMPA[i]);
		channels[i]->setAddr(i);
		channels[i]->setSampaAddr(Addr);
		channels[i]->setOutput(channelOutput);

	}
}

int SAMPA::processData(int serialOut){

	//Go through all channels for specific serialout
	int waitTime = 0;
	long currentLowestDataBuffer = constants::CHANNEL_DATA_BUFFER_SIZE;
	long currentLowestHeaderBuffer = constants::CHANNEL_HEADER_BUFFER_SIZE;
	for(int i = 0; i < constants::CHANNELS_PER_E_LINK; i++){
		int channelId = i + (serialOut*constants::CHANNELS_PER_E_LINK);
		Channel *channel = channels[channelId];

		int bufferDepth;
		if(channel->isReadable()){
			//find header
			if(!channel->headerBuffer.empty()){

				Packet header = channel->headerBuffer.front();
			//wait(31.25 * 5, SC_NS); //Header wait
				int headerBufferDepth = constants::CHANNEL_HEADER_BUFFER_SIZE - (channel->headerBuffer.size() * 5);

				bufferDepth = constants::CHANNEL_DATA_BUFFER_SIZE - channel->dataBuffer.size();
				if(constants::DG_SIMULTION_TYPE == 2 || constants::OUTPUT_TYPE == "long"){
					reportOccupancy(header,channel, bufferDepth, headerBufferDepth);
				}

				//TODO: CHange for huffman here!
				channel->headerBuffer.pop();
				if(!header.overflow || header.numberOfSamples > 0){

					for(int j = 0; j < header.numberOfSamples; j++){
						if(!channel->dataBuffer.empty())
							channel->dataBuffer.pop_front();
					}
				}
				waitTime = (5 + header.numberOfSamples);
				numberOfSamplesReceived+=header.numberOfSamples;

				wait((constants::SAMPA_OUTPUT_WAIT_TIME * waitTime), SC_NS);

				porter_SAMPA_to_GBT[serialOut]->nb_write(header);
				if(this->output){
					std::cout << "timeWindow: " << header << " to serial: " << serialOut << endl;
				}

			}

		}


	}

	return waitTime;
}

void SAMPA::serialOut0(){
	while(true){
		wait(1, SC_NS);
		processData(0);
	}
}
void SAMPA::serialOut1(){

	while(true){
		wait(1, SC_NS);
		processData(1);
	}
}
void SAMPA::serialOut2(){
	while(true){
		wait(1, SC_NS);
		processData(2);
	}
}
void SAMPA::serialOut3(){
	//wait();
	while(true){
		wait(1, SC_NS);
		processData(3);
	}
}

void SAMPA::reportOccupancy(Packet header, Channel *channel, int bufferDepth, int headerBufferDepth){
	if(infoArray[header.timeWindow - 1].gotInfo){

		if(bufferDepth < infoArray[header.timeWindow - 1].lowestBufferDepth){

			infoArray[header.timeWindow - 1].timeWindow = header.timeWindow;
			infoArray[header.timeWindow - 1].lowestBufferDepth = bufferDepth;
			infoArray[header.timeWindow - 1].channelWithLowestBufferDepth = channel;
			infoArray[header.timeWindow - 1].occupancy = header.occupancy;

					//std::cout << sc_time_stamp() << " " << bufferDepth << endl;

		}
		if(headerBufferDepth < infoArray[header.timeWindow - 1].lowestHeaderBufferDepth){
			infoArray[header.timeWindow - 1].lowestHeaderBufferDepth = headerBufferDepth;

		}
	} else {

		infoArray[header.timeWindow - 1].gotInfo = true;
		infoArray[header.timeWindow - 1].timeWindow = header.timeWindow;
		infoArray[header.timeWindow - 1].occupancy = header.occupancy;
		infoArray[header.timeWindow - 1].lowestBufferDepth = bufferDepth;
		infoArray[header.timeWindow - 1].channelWithLowestBufferDepth = channel;
		infoArray[header.timeWindow - 1].lowestHeaderBufferDepth = headerBufferDepth;

	}
}
