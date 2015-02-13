#include "DataGenerator.h"
/*
 * Generating samples
 * */
 void DataGenerator::t_sink(void){
 	if(constants::DG_SIMULTION_TYPE == 1){
 		standardSink();
 	} else if(constants::DG_SIMULTION_TYPE == 2){
 		incrementingOccupancySink();
 	} else if(constants::DG_SIMULTION_TYPE == 3){
 		globalRandomnessSink();
 	} else {
 		sendBlackEvents();
 	}
 }

void DataGenerator::createFlux(std::vector<int> &values){
 	RandomGenerator generator;
 	for (int i = 0; i < values.size(); ++i)
 	{
 		if(generator.generate(0, 100) <= 10 && values[i] - 20 > 0){
 			values[i] -= 20;
 			if(i - 100 > 0 && values[i] + 20 <= 100){
 				values[i - 100] += 20;
 			} else if(i + 100 < values.size() - 1 && values[i] + 20 <= 100){
 				values[i + 100] += 20;
 			}
 		}

 		if(generator.generate(0, 1000) <= 1){
 			values[i] = 100;
 			values[(i - 1) < 0 ? i - 1 : i + 1] = 0;
 		}
 	}
}

int DataGenerator::createSingleFlux(){
	int value = constants::DG_OCCUPANCY;
	RandomGenerator generator;
	if(generator.generate(0, 100) <= 10){
			if(generator.generate(0, 100) <= 10){
				value -= 15;
			} else if(generator.generate(0, 100) > 50 && generator.generate(0, 100) <= 70){
				value += 15;
			}

 	} else if(generator.generate(0, 1000) <= 1){
 		value = 100;
 	} else {
 		value = generator.generate(value - 5, value + 5);
 	}
 	return value;
}

void DataGenerator::globalRandomnessSink(){
 	int64_t packetCounter = 1;
 	int currentSample = 0;
 	int currentTimeWindow = 1;
 	int lastData = 0;
 	std::vector<int> values (constants::NUMBER_OF_SAMPA_CHIPS * constants::SAMPA_NUMBER_INPUT_PORTS, 1);

 	RandomGenerator randomGenerator;
	//std::cout << "Reading black events..";
	//readBlackEvents();

 	//int rounds = ((constants::NUMBER_OF_SAMPA_CHIPS * constants::SAMPA_NUMBER_INPUT_PORTS) - 1) * 30;

 	initOccupancy();
 	while(currentTimeWindow <= constants::NUMBER_TIME_WINDOWS_TO_SIMULATE){

 		std::cout << "Sending data " << currentSample << " | data value: " << lastData <<  endl;
 		for(int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS * constants::SAMPA_NUMBER_INPUT_PORTS; i++)
 		{
 			//int flux = createSingleFlux();
 			lastData = generateCore(i, currentTimeWindow, packetCounter, occupancyPoints[currentTimeWindow - 1]);
 			//lastData = generateCore(i, currentTimeWindow, packetCounter, createSingleFlux());
			if(lastData != 0){
				packetCounter++;
			}
 		}

 		currentSample++;
		//Increments timeWindow
		if(currentSample == constants::NUMBER_OF_SAMPLES_IN_EACH_TIME_WINDOW )//1021 samples
		{
			currentTimeWindow++;
			std::cout << "Current packetCounter: " << packetCounter << " window: " << currentTimeWindow << endl;
			currentSample = 0;
		}
		wait((constants::DG_WAIT_TIME), SC_NS);
	}
}
//Standard implementation with fixed Occupancy
void DataGenerator::standardSink(){
	int64_t packetCounter = 1;
	int currentSample = 0;
	int currentTimeWindow = 1;

    //While we still have timewindows to send
	while(currentTimeWindow <= constants::NUMBER_TIME_WINDOWS_TO_SIMULATE)
	{
		//Loop each channel
		for(int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS * constants::SAMPA_NUMBER_INPUT_PORTS; i++)
		{

			if(generateCore(i, currentTimeWindow, packetCounter, constants::DG_OCCUPANCY) != 0){
				packetCounter++;
			}
		}
		currentSample++;
		std::cout << "Sending data " << currentSample << endl;
		//Increments timeWindow
		if(currentSample == constants::NUMBER_OF_SAMPLES_IN_EACH_TIME_WINDOW )//1021 samples
		{
			currentTimeWindow++;
			std::cout << "Current packetCounter: " << packetCounter << " window: " << currentTimeWindow << endl;
			currentSample = 0;
		}
		wait((constants::DG_WAIT_TIME), SC_NS);
	}
}

void DataGenerator::initOccupancy(){
	occupancyPoints[0] = 30;
	occupancyPoints[1] = 30;
	occupancyPoints[2] = 30;
	occupancyPoints[3] = 30;
	occupancyPoints[4] = 90;
	occupancyPoints[5] = 90;
	occupancyPoints[6] = 100;
	occupancyPoints[7] = 100;
	occupancyPoints[8] = 100;
	occupancyPoints[9] = 100;
	occupancyPoints[10] = 100;
	occupancyPoints[11] = 90;
	occupancyPoints[12] = 90;
	occupancyPoints[13] = 80;
	occupancyPoints[14] = 80;
	occupancyPoints[15] = 60;
	occupancyPoints[16] = 60;
	occupancyPoints[17] = 30;
	occupancyPoints[18] = 30;
	occupancyPoints[19] = 30;
	occupancyPoints[20] = 30;
	occupancyPoints[21] = 30;
	occupancyPoints[22] = 30;
	occupancyPoints[23] = 0;
	occupancyPoints[24] = 0;

}

//Implementation with increasing occupancy
void DataGenerator::incrementingOccupancySink(){
	int currentTimeWindow = 1;
	int64_t packetCounter = 1;
	int occupancy = 10;
	int currentSample = 0;
	RandomGenerator g;
	int newOccupancy = 10;
	int lastData = 0;
	while(currentTimeWindow <= constants::NUMBER_TIME_WINDOWS_TO_SIMULATE)
	{
		for(int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS * constants::SAMPA_NUMBER_INPUT_PORTS; i++)
		{

			newOccupancy = g.generate(occupancy - 5, occupancy + 5);
			lastData = generateCore(i, currentTimeWindow, packetCounter, newOccupancy);
			if(lastData != 0){
				packetCounter++;
			}


		}
		std::cout << "Sending data " << currentSample << " | data value: " << lastData <<  endl;

		currentSample++;
		if(currentSample == constants::NUMBER_OF_SAMPLES_IN_EACH_TIME_WINDOW )//1021 samples
		{
			occupancyPoints[currentTimeWindow - 1] = occupancy;
			if(currentTimeWindow % (constants::NUMBER_TIME_WINDOWS_TO_SIMULATE / constants::TIME_WINDOW_OCCUPANCY_SPLIT) == 0){

				std::cout << "Occupancy: " << occupancy << endl;
				std::cout << "Current packetCounter: " << packetCounter << " window: " << currentTimeWindow << endl;
				occupancy = occupancy + constants::TIME_WINDOW_OCCUPANCY_SPLIT;

			}
			currentTimeWindow++;
			currentSample = 0;
		}
		wait((constants::DG_WAIT_TIME), SC_NS);
	}
}

int DataGenerator::generateCore(int portNumber, int currentTimeWindow, int packetCounter, int occupancy){

	RandomGenerator randomGenerator;

	if(randomGenerator.generate(0, 100) <= occupancy){

		Sample sample(currentTimeWindow, packetCounter, 1, randomGenerator.generate(20, 60));
		porter_DG_to_SAMPA[portNumber]->nb_write(sample);
		//write_log_to_file_sink(packetCounter, portNumber, currentTimeWindow);
		return sample.data;

	}
	Sample emptySample;
	porter_DG_to_SAMPA[portNumber]->nb_write(emptySample);
	return emptySample.data;
}

void DataGenerator::sendBlackEvents(){

	std::cout << "Reading events into memory" << endl;
	Datamap dataMap = readBlackEvents();
  int counter = 0;
	std::cout << sc_time_stamp() << " Finished reading events into memory" << endl;

  for(std::vector< DataEntry >::iterator it = dataMap.begin(); it != dataMap.end(); ++it){

    for(int i = 0; i < 1021; i++){
      for(std::map<int, std::list<Sample>>::iterator mit = it->begin(); mit != it->end(); ++mit){
        int channel = mit->first;
        //Reverse iterator through list
        porter_DG_to_SAMPA[channel]->nb_write(mit->second.back());
        mit->second.pop_back();
        counter++;
      }
      std::cout << "Sending data: " << counter << endl;
      wait((constants::DG_WAIT_TIME), SC_NS);
    }


  }

}
/*
Filinfo:
	ddl start pos = 4.
	hw start pos = 3.
typedef std::map< int, std::list<Sample> > DataEntry;
typedef std::vector< DataEntry > Datamap;
*/
DataGenerator::Datamap DataGenerator::readBlackEvents(){
		std::ifstream inputFile("output-final.file");

		std::string line;
    Mapper mapper;
    int samplePrefix, samplePostfix, hwAddr, nrOfSamples, startTime, sampaAddr;
    Datamap map;
    DataEntry entry;
    int sampleId = 0;
    int timeFrame = 1;
    int count = 0;

    while(!inputFile.eof()){

      if(timeFrame > constants::NUMBER_TIME_WINDOWS_TO_SIMULATE){
        break;
      }
      std::getline(inputFile, line);
      if(line.find("hw") == 0){
        std::list<Sample> list;
        hwAddr = std::stoi(line.substr(3));
        std::cout << "Hw addr: " << hwAddr;
        sampaAddr = mapper.getSampaChannel(hwAddr);
        std::cout << "Sampa: " << sampaAddr;
        if(sampaAddr < 0){
          continue;
        }
        std::getline(inputFile, line);
        nrOfSamples = std::stoi(line.substr(line.find(" ")));
        std::cout << "Sample:: " << nrOfSamples;
        startTime = std::stoi(line.substr(0, line.find(" ")));
        std::cout << "Starttime: " << startTime;
        samplePrefix = constants::NUMBER_OF_SAMPLES_IN_EACH_TIME_WINDOW - startTime;
        samplePostfix = startTime - nrOfSamples;

        for(int i = 0; i < samplePrefix; i++){
          Sample sample;
          list.push_back(sample);
        }
        for(int i = 0; i < nrOfSamples; i++){
          std::getline(inputFile, line);
          int signal = std::stoi(line.substr(line.find(" ")));
          Sample sample(timeFrame, sampleId, 1, signal);
          list.push_back(sample);
          sampleId++;
        }
        for(int i = 0; i < samplePostfix; i++){
          Sample sample;
          list.push_back(sample);
        }
        entry.insert(std::pair<int, std::list<Sample>>(sampaAddr, list));
        count++;

        if(count == 160){
          map.push_back(entry);
          entry.clear();
          count = 0;
          timeFrame++;
        }

      }

    }
		return map;
}


/*
 * Writing log data to text file.
 *
 */
void DataGenerator::write_log_to_file_sink(int _packetCounter, int _port, int _currentTimeWindow){
 	if (constants::DG_GENERATE_OUTPUT)
 	{
 		std::ofstream outputFile;
 		outputFile.open(constants::OUTPUT_FILE_NAME, std::ios_base::app);
 		outputFile << sc_time_stamp() << ": " << name() << " Sent packet "
 		<< _packetCounter << ", to port " << _port << ", current time window: " << _currentTimeWindow
 		<< std::endl;
 		outputFile.close();
 	}
 }
