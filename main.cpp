#ifndef SYSTEM_H
#define SYSTEM_H

#include <systemc.h>
#include <sstream>
#include <ctime>
#include "GBT.h"
#include "CRU.h"
#include "SAMPA.h"
#include "GlobalConstants.h"
#include "DataGenerator.h"
//#include "Graph.h"
#include "Monitor.h"


using namespace std;

int sc_main(int argc, char* argv[]) {
	std::cout << "Working..." << std::endl;

	stringstream module_name_stream;
	string module_name;
	int gbt_number = 0;
	int gbt_port = 0;
	int sampa_number = 0;
	int sampa_port = 0;
	int cru_number = 0;
	int cru_port = 0;

	//Clear output file
	std::ofstream outputFile;
	outputFile.open(constants::OUTPUT_FILE_NAME);
	outputFile << "";
	outputFile.close();

	//Monitor *monitor;
	DataGenerator dg("DataGenerator");
	SAMPA *sampas[constants::NUMBER_OF_SAMPA_CHIPS];
	GBT *gbts[constants::NUMBER_OF_GBT_CHIPS];
	//CRU cru ("CRU");
	CRU *crus[constants::NUMBER_OF_CRU_CHIPS];

	//Module initialization

    //monitor = new Monitor();
	//dg.monitor = monitor;

	//SAMPA
	for(int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS; i++)
	{
		module_name_stream << "SAMPA_" << i;
		module_name = module_name_stream.str();
		sampas[i] = new SAMPA(module_name.c_str());
		sampas[i]->setAddr(i);
		//sampas[i]->monitor = monitor;

		sampas[i]->initChannels();

		module_name_stream.str(string());
		module_name_stream.clear();
	}

  	//GBT
	for(int i = 0; i < constants::NUMBER_OF_GBT_CHIPS; i++)
	{
		module_name_stream << "GBT_" << i;
		module_name = module_name_stream.str();
		gbts[i] = new GBT(module_name.c_str());
		module_name_stream.str(string());
		module_name_stream.clear();
	}

	//CRU
	for(int i = 0; i < constants::NUMBER_OF_CRU_CHIPS; i++)
	{
		module_name_stream << "CRU_" << i;
		module_name = module_name_stream.str();
		crus[i] = new CRU(module_name.c_str());
		module_name_stream.str(string());
		module_name_stream.clear();
	}

   //Channel initialization

	//GBT-CRU
	sc_fifo<Packet>* fifo_GBT_CRU[constants::NUMBER_OF_CHANNELS_BETWEEN_GBT_AND_CRU * constants::NUMBER_OF_GBT_CHIPS];
	for(int i = 0; i < constants::NUMBER_OF_CHANNELS_BETWEEN_GBT_AND_CRU * constants::NUMBER_OF_GBT_CHIPS; i++)
	{
		fifo_GBT_CRU[i] = new sc_fifo<Packet>(constants::BUFFER_SIZE_BETWEEN_GBT_AND_CRU * constants::NUMBER_OF_CRU_CHIPS);
	}

	//SAMPA->GBT
	sc_fifo<Packet>* fifo_SAMPA_GBT[constants::NUMBER_OF_SAMPA_CHIPS * constants::NUMBER_OUTPUT_PORTS_TO_GBT]; // Only even numbers
	for(int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS * constants::NUMBER_OUTPUT_PORTS_TO_GBT; i++)
	{
		fifo_SAMPA_GBT[i] = new sc_fifo<Packet>(10000);
	}

	//DataGenerator->SAMPA channels
	sc_fifo<Sample>* fifo_DG_SAMPA[constants::NUMBER_OF_SAMPA_CHIPS * constants::SAMPA_NUMBER_INPUT_PORTS];
	for(int i = 0; i < (constants::NUMBER_OF_SAMPA_CHIPS * constants::SAMPA_NUMBER_INPUT_PORTS); i++)
	{
		fifo_DG_SAMPA[i] = new sc_fifo<Sample>(10000);
	}

   //Connecting Port-Channel-Port

	//GBT-CRU
	gbt_number = 0;
	gbt_port = 0;
	cru_number = 0;
	cru_port = 0;
	for (int i = 0; i < constants::NUMBER_OF_GBT_CHIPS * constants::NUMBER_OF_CHANNELS_BETWEEN_GBT_AND_CRU; i++)	//48
	{
		if (i != 0 && i % constants::NUMBER_OF_CHANNELS_BETWEEN_GBT_AND_CRU == 0)//number of channels mellom gbt og cru
		{
			gbt_number++;
			gbt_port = 0;
		}

		if (i != 0 && i % constants::CRU_NUMBER_INPUT_PORTS == 0)//24 gbt per 1 cru
		{
			cru_number++;
			cru_port = 0;
		}
		gbts[gbt_number]->porter_GBT_to_CRU[gbt_port++](*fifo_GBT_CRU[i]);
		crus[cru_number]->porter[cru_port++](*fifo_GBT_CRU[i]);

		//std::cout << "Kobler GBT " << gbt_number << " to CRU " << cru_number << std::endl;
	}

	/*

	gbt_number = 0;
	gbt_port = 0;
	for(int i = 0; i < constants::NUMBER_OF_CHANNELS_BETWEEN_GBT_AND_CRU * constants::NUMBER_OF_GBT_CHIPS; i++)
	{
		//if (i == 2)//NUMBER_OF_CHANNELS_BETWEEN_GBT_AND_CRU
		if (i != 0 && i % constants::NUMBER_OF_CHANNELS_BETWEEN_GBT_AND_CRU == 0)
		{
			gbt_number++;
			gbt_port = 0;
		}
		gbts[gbt_number]->porter_GBT_to_CRU[gbt_port++](*fifo_GBT_CRU[i]);
		cru.porter[i](*fifo_GBT_CRU[i]);
	}*/

	//GataGenerator-SAMPA
		sampa_number = 0;
		sampa_port = 0;
		for(int i = 0; i < (constants::NUMBER_OF_SAMPA_CHIPS * constants::SAMPA_NUMBER_INPUT_PORTS); i++)
		{
		if (i != 0 && i % constants::SAMPA_NUMBER_INPUT_PORTS == 0)//32 channel per SAMPA
		{
			sampa_number++;
			sampa_port = 0;
		}
		dg.porter_DG_to_SAMPA[i](*fifo_DG_SAMPA[i]);
		sampas[sampa_number]->porter_DG_to_SAMPA[sampa_port++](*fifo_DG_SAMPA[i]);

		//gbts[gbt_number]->porter_GBT_to_CRU[gbt_port++](*fifo_GBT_CRU[i]);
		//cru.porter[i](*fifo_GBT_CRU[i]);
	}

	//SAMPA->GBT

	gbt_number = 0;
	gbt_port = 0;
	sampa_number = 0;
	sampa_port = 0;
	for (int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS * constants::NUMBER_OUTPUT_PORTS_TO_GBT; i++)	//8
	{
		//if (i % constants::NUMBER_OF_CHANNELS_BETWEEN_SAMPA_AND_GBT == 0)
		if (i != 0 && i % constants::GBT_NUMBER_INPUT_PORTS == 0)
		{
			gbt_number++;
			gbt_port = 0;
		}
		//if (i % constants::NUMBER_OF_CHANNELS_BETWEEN_SAMPA_AND_GBT == 0)
		if (i != 0 && i % constants::NUMBER_OUTPUT_PORTS_TO_GBT == 0)
		{
			sampa_number++;
			sampa_port = 0;
		}
		sampas[sampa_number]->porter_SAMPA_to_GBT[sampa_port++](*fifo_SAMPA_GBT[i]);
		gbts[gbt_number]->porter_SAMPA_to_GBT[gbt_port++](*fifo_SAMPA_GBT[i]);
	}

	//start simulation
	sc_start(constants::SIMULATION_TOTAL_TIME, SC_NS);


	//Print Summary
	/*for(int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS; i++){
		std::cout << sampas[i]->name() << " received " << sampas[i]->numberOfSamplesReceived << std::endl;
	}

	for(int i = 0; i < constants::NUMBER_OF_GBT_CHIPS; i++)
	{
		std::cout << gbts[i]->name() << " received " << gbts[i]->numberOfSamplesReceived << std::endl; //numberOfSamplesReceived
	}

	for(int i = 0; i < constants::NUMBER_OF_CRU_CHIPS; i++)
	{
		std::cout << crus[i]->name() << " received " << crus[i]->numberOfSamplesReceived << std::endl; //numberOfSamplesReceived
	}*/
		std::cout << sc_time_stamp()  << " Finished " << std::endl;

 	// oversikt over innhold i fifo, bruk hvis cru ikke sender data
	/*outputFile.open(constants::OUTPUT_FILE_NAME, std::ios_base::app);
 	outputFile << "Stat for CRU:" << std::endl;
 	int sum = 0;
 	for(int i = 0; i < (constants::SAMPA_NUMBER_INPUT_PORTS * constants::NUMBER_OF_SAMPA_CHIPS); i++)
 	{

		outputFile << "Fifo "<< i << ". number of packets in fifo: " << cru.input_fifos[i].size();
		while(!cru.input_fifos[i].empty())
		{
			Packet packet = cru.input_fifos[i].front();
			outputFile << std::endl;
			outputFile << "\ttimeWindow: " << packet.timeWindow;
			outputFile << ", sampaChipId: " << packet.sampaChipId;
			outputFile << ", channelId: " << packet.channelId;
			outputFile << ", numberOfSamples: " << packet.numberOfSamples;
			cru.input_fifos[i].pop();
		}
		sum += cru.input_fifos[i].size();
		outputFile << std::endl;
	}
	outputFile << "Sum: " << sum << std::endl;

	outputFile.close();*/
	//----
	//std::cout << "DataGenerator shifted: " << dg.monitor->getDatageneratorInfo() << endl;
	if(constants::DG_SIMULTION_TYPE == 2){
		std::vector< Point > pointsD, pointsH;
		for(int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS; i++){
			for (int j = 0; j < constants::NUMBER_TIME_WINDOWS_TO_SIMULATE; ++j)
			{
				Point p1, p2;
				p1.y = static_cast<float>(sampas[i]->infoArray[j].lowestBufferDepth);
				p1.x = static_cast<float>(dg.occupancyPoints[j]);
				pointsD.push_back(p1);
				p2.y = static_cast<float>(sampas[i]->infoArray[j].lowestHeaderBufferDepth);
				p2.x = static_cast<float>(dg.occupancyPoints[j]);
				pointsH.push_back(p2);
			}
		//std::cout << dg.occupancyPoints[i] << endl;
		}
		Graph graph1(pointsD, "Occupancy", "Depth" , "OccupancyDataBuffer");
		Graph graph2(pointsH, "Occupancy", "Depth" , "OccupancyHeaderBuffer");
		graph1.writeGraphToFile(false);
		graph2.writeGraphToFile(false);
	} else if(constants::DG_SIMULTION_TYPE != 2){
		std::vector< MultiPoint > dataPoints, headerPoints;
		StringVector labels;
		labels.push_back("SAMPA");
		labels.push_back("Occupancy");
		labels.push_back("Max Data Depth");
		labels.push_back("Data Depth");
		labels.push_back("");
		labels.push_back("Max Header Depth");
		labels.push_back("Header Depth");

		if(constants::OUTPUT_TYPE == "lowest"){

		for(int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS; i++){
			for (int j = 0; j < constants::SAMPA_NUMBER_INPUT_PORTS; ++j)
			{
				MultiPoint pd, ph;
				std::string name = std::string(sampas[i]->name()) + "_Channel" +
					std::to_string(sampas[i]->channels[j]->getAddr());
				pd.push_back(name);
				pd.push_back(std::to_string(constants::CHANNEL_DATA_BUFFER_SIZE));
				pd.push_back(std::to_string(sampas[i]->channels[j]->lowestDataBufferNumber));
				pd.push_back("");
				pd.push_back(std::to_string(constants::CHANNEL_HEADER_BUFFER_SIZE));
				pd.push_back(std::to_string(sampas[i]->channels[j]->lowestHeaderBufferNumber));
				dataPoints.push_back(pd);
			}

		}
		Graph dataGraph(dataPoints, labels, "FullGraph-Lowest-Test2");
		dataGraph.writeMultiGraphToFile(true);
		} else if(constants::OUTPUT_TYPE == "long"){
			StringVector l;
			l.push_back("TimeFrame");
			l.push_back("Timebin");
			l.push_back("Signal");
			l.push_back("Occupancy");

			Graph g(sampas[0]->channels[16]->dataPoints, l, "SignalGraph100TW25OCC");
			g.writeMultiGraphToFile(true);
			for(int i = 0; i < constants::NUMBER_OF_SAMPA_CHIPS; i++){
				for(int j = 0; j < constants::SAMPA_NUMBER_INPUT_PORTS; j++){
					for (int k = 0; k < sampas[i]->channels[j]->dataBufferNumbers.size(); ++k){
						MultiPoint pd, ph;
						std::string name = std::string(sampas[i]->name()) + "_Channel" +
							std::to_string(sampas[i]->channels[j]->getAddr());
						pd.push_back(name);
						pd.push_back(std::to_string(constants::DG_OCCUPANCY));
						pd.push_back(std::to_string(constants::CHANNEL_DATA_BUFFER_SIZE));
						pd.push_back(std::to_string(sampas[i]->channels[j]->dataBufferNumbers[k]));
						pd.push_back(std::to_string(k));
						pd.push_back(std::to_string(constants::CHANNEL_HEADER_BUFFER_SIZE));
						pd.push_back(std::to_string(sampas[i]->channels[j]->headerBufferNumbers[k]));
						dataPoints.push_back(pd);
					}
				}

			}
			Graph dataGraph(dataPoints, labels, "FullGraph-25-100TW");
			dataGraph.writeMultiGraphToFile(true);
		}

	}



	outputFile.open(constants::OUTPUT_FILE_NAME, std::ios_base::app);

	for(int i = 0; i < constants::NUMBER_OF_CRU_CHIPS; i++)
	{
		outputFile << "Data sent by CRU" << i << ":" << std::endl;

		while(!crus[i]->sentData.empty())
		{
			outputFile << crus[i]->sentData.front() << std::endl;
			crus[i]->sentData.pop();
		}
	}
	outputFile.close();

	return 0;
}

/*
 *  Static connection example
	gbts[0]->porter_SAMPA_to_GBT[0](*fifo_SAMPA_GBT[0]);
	gbts[0]->porter_SAMPA_to_GBT[1](*fifo_SAMPA_GBT[1]);
	gbts[0]->porter_SAMPA_to_GBT[2](*fifo_SAMPA_GBT[2]);
	gbts[0]->porter_SAMPA_to_GBT[3](*fifo_SAMPA_GBT[3]);
	gbts[0]->porter_SAMPA_to_GBT[4](*fifo_SAMPA_GBT[4]);
	gbts[0]->porter_SAMPA_to_GBT[5](*fifo_SAMPA_GBT[5]);
	gbts[0]->porter_SAMPA_to_GBT[6](*fifo_SAMPA_GBT[6]);
	gbts[0]->porter_SAMPA_to_GBT[7](*fifo_SAMPA_GBT[7]);

	sampas[0]->porter[0](*fifo_SAMPA_GBT[0]);
	sampas[0]->porter[1](*fifo_SAMPA_GBT[1]);
	sampas[0]->porter[2](*fifo_SAMPA_GBT[2]);
	sampas[0]->porter[3](*fifo_SAMPA_GBT[3]);
	sampas[1]->porter[0](*fifo_SAMPA_GBT[4]);
	sampas[1]->porter[1](*fifo_SAMPA_GBT[5]);
	sampas[1]->porter[2](*fifo_SAMPA_GBT[6]);
	sampas[1]->porter[3](*fifo_SAMPA_GBT[7]);
	*/
//
#endif
