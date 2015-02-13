#include <systemc.h>
#include <vector>
#include "Channel.h"

SC_HAS_PROCESS(Channel);



Channel::Channel(sc_module_name name) : sc_module(name){
	SC_THREAD(receiveData);
	
}

void Channel::receiveData(){
	Sample sample;
	int currentTimeWindow = 1;
	int numberOfSamples = 0;
	int overflow = false;
	int numberOfClockCycles = 0;
	lowestDataBufferNumber = constants::CHANNEL_DATA_BUFFER_SIZE;
	lowestHeaderBufferNumber = constants::CHANNEL_HEADER_BUFFER_SIZE;
	//Run forevers
	while(true){
			
		//Read from datagenerator
		if(port_DG_to_CHANNEL->nb_read(sample)){

			
			numberOfClockCycles++;

			//Checks to see if there is overflow.
			if(dataBuffer.size() + sample.size > constants::CHANNEL_DATA_BUFFER_SIZE){
				overflow = true;
				//numberOfSamples = 0;
				//numberOfClockCycles = constants::NUMBER_OF_SAMPLES_IN_EACH_TIME_WINDOW;
			} else {
				//Check if packet is "real"
				if(sample.sampleId > 0 || sample.size > 0){
					dataBuffer.push_back(sample);
					numberOfSamples++;
				}				
			}

			//Filtered output
			if(this->getSampaAddr() == 1 && Addr == 1){
				//std::cout << "I'm here "<< currentTimeWindow <<" \n";
				//std::cout << port_DG_to_CHANNEL->c() << endl;
			}

		}
		//When we reach the end of a timeWindow we send the header packet to its buffer and starts a new window
		if(numberOfClockCycles == constants::NUMBER_OF_SAMPLES_IN_EACH_TIME_WINDOW ){
			Packet header(currentTimeWindow, this->getAddr(), numberOfSamples, overflow, 1);//Om behov, endre packetId
				header.sampaChipId = this->getSampaAddr();				
				headerBuffer.push(header);
				if(constants::OUTPUT_TYPE == "long"){
					if(numberOfSamples > 0){
						dataBufferNumbers.push_back(constants::CHANNEL_DATA_BUFFER_SIZE - dataBuffer.size());
						headerBufferNumbers.push_back(constants::CHANNEL_HEADER_BUFFER_SIZE - headerBuffer.size()*5);
					}
						
				}
				if(overflow){
					for (int i = 0; i < numberOfSamples; ++i)
					{
						/* code */
						
						dataBuffer.pop_back();
						
					}
				}

				

				//Clean up
				overflow = false;
				readable = true;
				numberOfSamples = 0;
				currentTimeWindow++;
				numberOfClockCycles = 0;
				
				if(this->getSampaAddr() == 1 && Addr == 1){
					//std::cout << "Pushing header packet! " << currentTimeWindow << endl;
				}
		}
		if(constants::OUTPUT_TYPE == "lowest"){
			if(lowestDataBufferNumber > (constants::CHANNEL_DATA_BUFFER_SIZE - dataBuffer.size()))
				lowestDataBufferNumber = (constants::CHANNEL_DATA_BUFFER_SIZE - dataBuffer.size());

			if(lowestHeaderBufferNumber > (constants::CHANNEL_HEADER_BUFFER_SIZE - headerBuffer.size() * 5))
				lowestHeaderBufferNumber = (constants::CHANNEL_HEADER_BUFFER_SIZE - headerBuffer.size()*5);
		}
		
		


		wait(constants::SAMPA_INPUT_WAIT_TIME, SC_NS);	
		//wait(constants::SAMPA_INPUT_WAIT_TIME, SC_NS);
	}
}


