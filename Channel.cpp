#include <vector>
#include "Channel.h"

SC_HAS_PROCESS(Channel);



Channel::Channel(sc_module_name name) : sc_module(name){
	SC_THREAD(receiveData);

}



void Channel::receiveData(){
	Sample sample, lastSample;

	MultiPoint point;

	int currentTimeWindow = 1;
	bool insertLastSample = true;
	std::vector<Sample> zeroSamples;
	int numberOfSamples = 0;
	bool overflow = false;
	int numberOfClockCycles = 0;
	int zeroCount = 0;
	int currentOccupancy = 0;
	lowestDataBufferNumber = constants::CHANNEL_DATA_BUFFER_SIZE;
	lowestHeaderBufferNumber = constants::CHANNEL_HEADER_BUFFER_SIZE;
	//Run forevers
	while(true){

		//Read from datagenerator
		if(port_DG_to_CHANNEL->nb_read(sample)){

			currentOccupancy = sample.occupancy;
			numberOfClockCycles++;
			//sample.data -= constants::ZERO_SUPPRESION_BASELINE;
			if(sample.data == 0){
				zeroCount++;
			} else {
				if(zeroCount > 2){
					for(int i = 0; i < zeroCount - 2; i++){
						dataBuffer.pop_back();
						if(this->getSampaAddr() == 0 && Addr == 16)
							dataPoints.pop_back();
						numberOfSamples--;
					}
				}
				zeroCount = 0;
			}

			dataBuffer.push_back(sample);
			numberOfSamples++;
			if(this->getSampaAddr() == 0 && Addr == 16){
				point.push_back(std::to_string(sample.timeWindow));
				point.push_back(std::to_string(numberOfClockCycles));
				point.push_back(std::to_string(sample.data));
				point.push_back(std::to_string(sample.occupancy));
				dataPoints.push_back(point);
				point.clear();
				//std::cout << "frame: " << sample.timeWindow << " - data: " << sample.data << std::endl;
			}

		/*	if(sample.data > 0){
				int c = calcAction(sample, lastSample, insertLastSample);
				switch(c){
					case 0:
						overflow = true;
						break;
					case 1:
						dataBuffer.push_back(sample);
						numberOfSamples++;
						//Statistics
						if(this->getSampaAddr() == 4 && Addr == 16){
							point.push_back("Sampa " + std::to_string(this->getSampaAddr()));
							point.push_back("Channel " + std::to_string(this->getAddr()));
							point.push_back(std::to_string(numberOfClockCycles));
							point.push_back(std::to_string(sample.data));

							dataPoints.push_back(point);
						}

						break;
					case 2:
						dataBuffer.push_back(lastSample);
						dataBuffer.push_back(sample);

						//Statistics
						if(this->getSampaAddr() == 4 && Addr == 16){
							firstPoint.push_back("Sampa " + std::to_string(this->getSampaAddr()));
							firstPoint.push_back("Channel " + std::to_string(this->getAddr()));
							firstPoint.push_back(std::to_string(numberOfClockCycles-1));
							firstPoint.push_back(std::to_string(lastSample.data));
							dataPoints.push_back(firstPoint);
							secondPoint.push_back("Sampa " + std::to_string(this->getSampaAddr()));
							secondPoint.push_back("Channel " + std::to_string(this->getAddr()));
							secondPoint.push_back(std::to_string(numberOfClockCycles));
							secondPoint.push_back(std::to_string(sample.data));
							dataPoints.push_back(secondPoint);
						} // End stat
						insertLastSample = false;
						numberOfSamples++;
						numberOfSamples++;
						break;
					case 3:
						insertLastSample = true;
						break;
					case 4:
						break;
					default:
						break;
				}
			}

			lastSample = sample;*/

		}
		//When we reach the end of a timeWindow we send the header packet to its buffer and starts a new window
		if(numberOfClockCycles == constants::NUMBER_OF_SAMPLES_IN_EACH_TIME_WINDOW ){

			Packet header(currentTimeWindow, this->getAddr(), numberOfSamples, overflow, 1, currentOccupancy);//Om behov, endre packetId
				header.sampaChipId = this->getSampaAddr();
				headerBuffer.push(header);
				/*if(this->getSampaAddr() == 0 && Addr == 16 && numberOfSamples > 0){
					Graph g(dataPoints, labels, "SignalGraph");
					g.writeMultiGraphToFile(true);
					dataPoints.clear();
				}*/
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
				zeroCount = 0;
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

int Channel::calcAction(Sample sample, Sample lastSample, bool insertLastSample){
	if(dataBuffer.size() + sample.size > constants::CHANNEL_DATA_BUFFER_SIZE){
		return 0; //Overflow
	} else {
		if(lastSample.data > 0){
			return 1; //Insert current Sample
			if(insertLastSample){
				return 2; //Insert Last Sample and current Sample
			}
		}
		return 3; //noting
	}
}
